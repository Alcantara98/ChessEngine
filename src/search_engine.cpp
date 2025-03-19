#include "search_engine.h"

namespace engine::parts
{
// CONSTRUCTORS

SearchEngine::SearchEngine(BoardState &board_state)
    : game_board_state(board_state),
      transposition_table(MAX_TRANSPOSITION_TABLE_SIZE)
{
} // Initialize with a max size

// PUBLIC FUNCTIONS

void SearchEngine::handle_engine_turn()
{
  search_thread_handler.start_thread(max_search_time_milliseconds);
}

void SearchEngine::stop_engine_turn() { search_thread_handler.stop_thread(); }

void SearchEngine::start_engine_pondering()
{
  engine_is_pondering = true;
  ponder_thread_handler.start_thread(MAX_SEARCH_TIME);
}

void SearchEngine::stop_engine_pondering()
{
  ponder_thread_handler.stop_thread();
  engine_is_pondering = false;
}

void SearchEngine::clear_previous_move_evals()
{
  while (!previous_move_evals.empty())
  {
    previous_move_evals.pop();
  }
}

auto SearchEngine::last_move_eval() -> int
{
  if (!previous_move_evals.empty())
  {
    return previous_move_evals.top();
  }
  return 0;
}

void SearchEngine::pop_last_move_eval()
{
  if (!previous_move_evals.empty())
  {
    previous_move_evals.pop();
  }
}

auto SearchEngine::engine_is_searching() -> bool
{
  return running_search_flag && !engine_is_pondering;
}

// PRIVATE FUNCTIONS

auto SearchEngine::search_and_execute_best_move() -> bool
{
  running_search_flag = true;
  std::vector<std::pair<Move, int>> move_scores;
  start_iterative_search_evaluation(move_scores);
  sort_moves(move_scores);

  if (show_move_evaluations)
  {
    // Print all moves and their evaluations.
    for (auto move_score : move_scores)
    {
      printf("Move: %s, Score: %d\n",
             MoveInterface::move_to_string(move_score.first).c_str(),
             move_score.second);
    }
    printf("\n");
  }

  // Check if move leaves king in check.
  for (std::pair<Move, int> move_score : move_scores)
  {
    if (!game_board_state.move_leaves_king_in_check(move_score.first))
    {
      game_board_state.apply_move(move_score.first);
      previous_move_evals.push(move_score.second);
      printf("Engine's Move: %s\n",
             MoveInterface::move_to_string(move_score.first).c_str());
      printf("Evaluation of Engine's Move: %d\n", -move_score.second);
      return true;
    }
  }
  // No valid moves found.
  return false;
}

void SearchEngine::start_iterative_search_evaluation(
    std::vector<std::pair<Move, int>> &move_scores)
{
  std::vector<Move> possible_moves =
      move_generator::calculate_possible_moves(game_board_state);

  // Search until running_search_flag is false, or max_search_depth is reached.
  for (int iterative_depth = 1; iterative_depth <= max_search_depth;
       ++iterative_depth)
  {
    max_iterative_search_depth = iterative_depth;

    std::vector<std::thread> search_threads;
    std::vector<std::promise<int>> promises(possible_moves.size());
    std::vector<std::future<int>> futures;
    std::vector<BoardState> thread_board_states(possible_moves.size(),
                                                BoardState(game_board_state));

    auto search_start_time = std::chrono::steady_clock::now();
    // Search each move in a separate thread.
    for (int move_index = 0; move_index < possible_moves.size(); ++move_index)
    {
      // Calculate possible moves again for each thread.
      // NOTE: This is because moves contain references to pieces from the
      // board state. If game_board_state moves are used, threads will modify
      // its pieces, causing issues (piece_has_moved_flag for example may become
      // true, and if it is a pawn, it can no longer move 2 squares forward).
      std::vector<Move> thread_possible_moves =
          move_generator::calculate_possible_moves(
              thread_board_states[move_index]);

      // Apply move to the board state for the thread.
      thread_board_states[move_index].apply_move(
          thread_possible_moves[move_index]);
      futures.push_back(promises[move_index].get_future());

      // Start thread and search.
      search_threads.emplace_back(
          [this, &promises, move_index, iterative_depth, &thread_board_states]()
          {
            int eval;
            if (use_aspiration_window)
            {
              eval = run_search_with_aspiration_window(
                  thread_board_states[move_index], iterative_depth);
            }
            else
            {
              eval = -negamax_alpha_beta_search(thread_board_states[move_index],
                                                -INF, INF, iterative_depth - 1,
                                                false);
            }
            promises[move_index].set_value(eval);
          });
    }

    // Wait for all threads to finish.
    for (auto &thread : search_threads)
    {
      thread.join();
    }

    if (running_search_flag)
    {
      // Get results from threads.
      move_scores.clear();
      for (int index = 0; index < possible_moves.size(); ++index)
      {
        move_scores.emplace_back(possible_moves[index], futures[index].get());
      }
    }
    else
    {
      return;
    }

    auto search_end_time = std::chrono::steady_clock::now();
    reset_and_print_performance_matrix(iterative_depth, search_start_time,
                                       search_end_time);
  }
}

void SearchEngine::start_iterative_search_pondering()
{
  std::vector<Move> possible_moves =
      move_generator::calculate_possible_moves(game_board_state);

  // Search until running_search_flag is false, or max_search_depth is reached.
  for (int iterative_depth = 1; iterative_depth <= MAX_SEARCH_DEPTH;
       ++iterative_depth)
  {
    max_iterative_search_depth = iterative_depth;

    std::vector<std::thread> search_threads;
    std::vector<BoardState> thread_board_states(possible_moves.size(),
                                                BoardState(game_board_state));

    auto search_start_time = std::chrono::steady_clock::now();
    // Search each move in a separate thread.
    for (int move_index = 0; move_index < possible_moves.size(); ++move_index)
    {
      // Calculate possible moves again for each thread.
      // NOTE: This is because moves contain references to pieces from the
      // board state. If game_board_state moves are used, threads will modify
      // its pieces, causing issues (piece_has_moved_flag for example may become
      // true, and if it is a pawn, it can no longer move 2 squares forward).
      std::vector<Move> thread_possible_moves =
          move_generator::calculate_possible_moves(
              thread_board_states[move_index]);

      // Apply move to the board state for the thread.
      thread_board_states[move_index].apply_move(
          thread_possible_moves[move_index]);

      // Start thread and search.
      search_threads.emplace_back(
          [this, move_index, iterative_depth, &thread_board_states]()
          {
            int eval;
            if (use_aspiration_window)
            {
              eval = run_search_with_aspiration_window(
                  thread_board_states[move_index], iterative_depth);
            }
            else
            {
              eval = -negamax_alpha_beta_search(thread_board_states[move_index],
                                                -INF, INF, iterative_depth - 1,
                                                false);
            }
          });
    }

    // Wait for all threads to finish.
    for (auto &thread : search_threads)
    {
      thread.join();
    }

    if (!running_search_flag)
    {
      return;
    }

    auto search_end_time = std::chrono::steady_clock::now();
    reset_and_print_performance_matrix(iterative_depth, search_start_time,
                                       search_end_time);
  }
}

auto SearchEngine::run_search_with_aspiration_window(BoardState &board_state,
                                                     int depth) -> int
{
  int alpha;
  int beta;
  int eval = last_move_eval();

  // Try aspiration windows until a valid window is found.
  for (int window_increment : ASPIRATION_WINDOWS)
  {
    if (std::abs(last_move_eval()) == INF)
    {
      alpha = -INF;
      beta = INF;
    }
    else
    {
      beta = eval + window_increment;
      alpha = eval - window_increment;
    }

    eval = -negamax_alpha_beta_search(board_state, -beta, -alpha, depth - 1,
                                      false);

    if (!running_search_flag)
    {
      break;
    }

    // Return eval if it is within the window.
    if (eval < beta && eval > alpha)
    {
      break;
    }
  }
  return eval;
}

auto SearchEngine::negamax_alpha_beta_search(BoardState &board_state,
                                             int alpha,
                                             int beta,
                                             int depth,
                                             bool null_move_line) -> int
{
  if (!running_search_flag)
  {
    return 0;
  }
  // If the king is no longer in the board, checkmate has occurred.
  // Return -INF evaluation for the side that has lost its king.
  if (board_state.color_to_move == PieceColor::WHITE &&
      !board_state.white_king_on_board)
  {
    return -INF;
  }
  if (board_state.color_to_move == PieceColor::BLACK &&
      !board_state.black_king_on_board)
  {
    return -INF;
  }

  int original_alpha = alpha;
  nodes_visited.fetch_add(1, std::memory_order_relaxed);

  int eval;
  int tt_value;
  int tt_flag;
  int entry_search_depth;
  int entry_best_move = -1;
  uint64_t hash = board_state.compute_zobrist_hash();
  // Check transposition table if position has been searched before.
  if (transposition_table.retrieve(hash, entry_search_depth, tt_value, tt_flag,
                                   entry_best_move))
  {
    // Check if tt_value can be used.
    if (depth <= entry_search_depth)
    {
      switch (tt_flag)
      {
      case EXACT:
        return tt_value;

      case FAILED_HIGH:
        alpha = std::max(alpha, tt_value);
        break;

      case FAILED_LOW:
        beta = std::min(beta, tt_value);
        break;

      default:
        // Handle unexpected tt_flag value.
        printf("BREAKPOINT minimax_alpha_beta_search; tt_flag: %d", tt_flag);
        exit(0);
      }

      // Check if we still fail high or low with the current alpha and beta.
      // If flag is FAILED_HIGH, (tt_value >= beta)
      // If flag is FAILED_LOW, (tt_value <= alpha)
      if (alpha >= beta)
      {
        return tt_value;
      }
    }
  }

  // Evaluate leaf nodes.
  if (depth <= 0)
  {
    eval = engine::parts::position_evaluator::evaluate_position(board_state);
    leaf_nodes_visited.fetch_add(1, std::memory_order_relaxed);
    if (board_state.color_to_move == PieceColor::BLACK)
    {
      return -eval;
    }
    return eval;
  }

  // Try a null move.
  if (!null_move_line &&
      (max_iterative_search_depth - depth) >= MIN_NULL_MOVE_DEPTH &&
      !board_state.is_end_game)
  {
    do_null_move_search(board_state, alpha, beta, depth, eval);
    // If a null move (which is theoretically a losing move) is greater than
    // beta, then return null move eval.
    if (eval >= beta)
    {
      return eval;
    }
  }

  std::vector<Move> possible_moves =
      move_generator::calculate_possible_moves(board_state);
  int max_eval = -INF;
  int best_move_index = 0;

  // If there is a best move from the transposition table, move it to the
  // front to be searched first, causing more alpha beta pruning to occur.
  if (entry_best_move >= 0 && entry_best_move < possible_moves.size())
  {
    std::swap(possible_moves[0], possible_moves[entry_best_move]);
  }

  // Search and evaluate each move.
  run_negamax_procedure(board_state, alpha, beta, max_eval, eval, depth,
                        best_move_index, possible_moves, null_move_line);

  // If search has stopped, don't save the states in the transposition
  // table. This will cause invalid states to be stored with eval scores of 0.
  // This may be saved as exact values in the transposition table, causing
  // incorrect cutoffs.
  if (!running_search_flag)
  {
    return 0;
  }
  store_state_in_transposition_table(hash, depth, max_eval, original_alpha,
                                     beta, best_move_index);
  return max_eval;
}

void SearchEngine::sort_moves(std::vector<std::pair<Move, int>> &move_scores)
{
  sort(
      move_scores.begin(), move_scores.end(),
      [](const std::pair<Move, int> &move_a, const std::pair<Move, int> &move_b)
      { return move_a.second > move_b.second; });
}

void SearchEngine::run_negamax_procedure(BoardState &board_state,
                                         int &alpha,
                                         int &beta,
                                         int &max_eval,
                                         int &eval,
                                         int &depth,
                                         int &best_move_index,
                                         std::vector<Move> &possible_moves,
                                         bool &null_move_line)
{
  for (int move_index = 0; move_index < possible_moves.size(); ++move_index)
  {
    board_state.apply_move(possible_moves[move_index]);
    // We only want to know if there is an eval greater than beta, hence make
    // the search window tight.
    eval = -negamax_alpha_beta_search(board_state, -beta, -alpha, depth - 1,
                                      null_move_line);
    if (eval > max_eval)
    {
      max_eval = eval;
      best_move_index = move_index;
    }
    max_eval = std::max(max_eval, eval);
    alpha = std::max(eval, alpha);
    board_state.undo_move();
    if (alpha >= beta)
    {
      break;
    }
  }
}

void SearchEngine::do_null_move_search(
    BoardState &board_state, int &alpha, int &beta, int &depth, int &eval)
{
  // If previous move is a null move, skip this to prevent double null
  // moves. This will prevent the search from being too shallow.
  board_state.apply_null_move();
  eval = -negamax_alpha_beta_search(board_state, -beta, -(beta - 1),
                                    depth - NULL_MOVE_REDUCTION, true);
  board_state.undo_null_move();
}

void SearchEngine::store_state_in_transposition_table(uint64_t &hash,
                                                      int &depth,
                                                      int &max_eval,
                                                      int &alpha,
                                                      int &beta,
                                                      int &best_move_index)
{
  // Store in transposition table.
  int tt_flag_to_store;
  if (max_eval >= beta)
  {
    tt_flag_to_store = FAILED_HIGH;
  }
  else if (max_eval <= alpha)
  {
    tt_flag_to_store = FAILED_LOW;
  }
  else
  {
    tt_flag_to_store = EXACT;
  }
  transposition_table.store(hash, depth, max_eval, tt_flag_to_store,
                            best_move_index);
}

void SearchEngine::reset_and_print_performance_matrix(
    int iterative_depth,
    std::chrono::time_point<std::chrono::steady_clock> search_start_time,
    std::chrono::time_point<std::chrono::steady_clock> search_end_time)
{
  // Calculate duration of search.
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
                      search_end_time - search_start_time)
                      .count();

  // Print performance metrics to user.
  if ((show_performance && !engine_is_pondering) ||
      (show_ponder_performance && engine_is_pondering))
  {
    printf("Depth: %d, Time: %lldms\n", iterative_depth, duration);
    printf("Leaf Nodes Visited %d\n", leaf_nodes_visited.load());
    printf("Nodes Visited %d\n", nodes_visited.load());
    printf("Leaf Nodes per second: %d kN/s\n",
           static_cast<int>(leaf_nodes_visited /
                            (duration / MILLISECONDS_TO_SECONDS) /
                            NODES_TO_KILONODES));
    printf(
        "Nodes per second: %d kN/s\n\n",
        static_cast<int>(nodes_visited / (duration / MILLISECONDS_TO_SECONDS) /
                         NODES_TO_KILONODES));
  }

  // Reset performance metrics.
  nodes_visited = 0;
  leaf_nodes_visited = 0;
}
} // namespace engine::parts