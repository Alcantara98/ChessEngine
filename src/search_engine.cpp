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

auto SearchEngine::execute_best_move() -> bool
{
  std::vector<std::pair<Move, int>> move_scores;
  evaluate_possible_moves(move_scores);
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

// PRIVATE FUNCTIONS
void SearchEngine::evaluate_possible_moves(
    std::vector<std::pair<Move, int>> &move_scores)
{
  std::vector<Move> possible_moves =
      move_generator::calculate_possible_moves(game_board_state);

  // Search to max_search_depth with iterative deepening.
  for (int iterative_depth = 1; iterative_depth <= max_search_depth;
       ++iterative_depth)
  {
    max_iterative_search_depth = iterative_depth;
    move_scores.clear();

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
      // its pieces.
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
            int eval = run_search_with_aspiration_window(
                thread_board_states[move_index], iterative_depth);
            promises[move_index].set_value(eval);
          });
    }

    // Wait for all threads to finish.
    for (auto &thread : search_threads)
    {
      thread.join();
    }

    // Get results from threads.
    for (int index = 0; index < possible_moves.size(); ++index)
    {
      move_scores.emplace_back(possible_moves[index], futures[index].get());
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
  int eval = 0;

  // Try aspiration windows until a valid window is found.
  for (int window_increment : ASPIRATION_WINDOWS)
  {
    if (window_increment == INF)
    {
      alpha = -INF;
      beta = INF;
    }
    else
    {
      alpha = last_move_eval() - window_increment;
      beta = last_move_eval() + window_increment;
    }
    eval =
        -minimax_alpha_beta_search(board_state, alpha, beta, depth - 1, false);

    // Return eval if it is within the window.
    if (eval < beta && eval > alpha)
    {
      break;
    }
  }

  return eval;
}

auto SearchEngine::minimax_alpha_beta_search(BoardState &board_state, int alpha,
                                             int beta, int depth,
                                             bool null_move_line) -> int
{
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
      case 0:
        return tt_value;
      case 1:
        alpha = std::max(alpha, tt_value);
        break;
      case -1:
        beta = std::min(beta, tt_value);
        break;
      default:
        // Handle unexpected tt_flag value.
        printf("BREAKPOINT minimax_alpha_beta_search; tt_flag: %d", tt_flag);
        exit(0);
      }

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
      (max_iterative_search_depth - depth) >= MIN_NULL_MOVE_DEPTH)
  {
    do_null_move_search(board_state, alpha, beta, depth, eval);
    // If null move (which is theoretically a losing move) is greater than beta,
    // then return null move eval.
    if (eval >= beta)
    {
      return eval;
    }
  }

  std::vector<Move> possible_moves =
      move_generator::calculate_possible_moves(board_state);
  int max_eval = -INF;
  int best_move_index = 0;
  // Start minimax search.
  if (entry_best_move >= 0 && entry_best_move < possible_moves.size())
  {
    // Search the best move first.
    max_search(board_state, alpha, beta, max_eval, eval, depth, best_move_index,
               entry_best_move, possible_moves, null_move_line);
  }
  for (int index = 0; index < possible_moves.size(); ++index)
  {
    // Search the rest of the moves.
    max_search(board_state, alpha, beta, max_eval, eval, depth, best_move_index,
               index, possible_moves, null_move_line);
    if (alpha >= beta)
    {
      break;
    }
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

void SearchEngine::max_search(BoardState &board_state, int &alpha, int &beta,
                              int &max_eval, int &eval, int &depth,
                              int &best_move_index, int &move_index,
                              std::vector<Move> &possible_moves,
                              bool &null_move_line)
{
  board_state.apply_move(possible_moves[move_index]);
  eval = -minimax_alpha_beta_search(board_state, -beta, -alpha, depth - 1,
                                    null_move_line);
  if (eval > max_eval)
  {
    max_eval = eval;
    best_move_index = move_index;
  }
  max_eval = std::max(max_eval, eval);
  alpha = std::max(eval, alpha);
  board_state.undo_move();
}

void SearchEngine::do_null_move_search(BoardState &board_state, int &alpha,
                                       int &beta, int &depth, int &eval)
{
  // If previous move is a null move, skip this to prevent double null
  // moves. This will prevent the search from being too shallow.
  board_state.apply_null_move();
  eval = -minimax_alpha_beta_search(board_state, -beta, -alpha,
                                    depth - NULL_MOVE_REDUCTION, true);
  board_state.undo_null_move();
}

void SearchEngine::store_state_in_transposition_table(uint64_t &hash,
                                                      int &depth, int &max_eval,
                                                      int &alpha, int &beta,
                                                      int &best_move_index)
{
  // Store in transposition table.
  int tt_flag_to_store;
  if (max_eval >= beta)
  {
    tt_flag_to_store = 1;
  }
  else if (max_eval <= alpha)
  {
    tt_flag_to_store = -1;
  }
  else
  {
    tt_flag_to_store = 0;
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
  if (show_performance)
  {
    printf("Depth: %d, Time: %lldms\n", iterative_depth, duration);
    printf("Nodes Visited %d\n", nodes_visited.load());
    printf("Leaf Nodes Visited %d\n", leaf_nodes_visited.load());
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