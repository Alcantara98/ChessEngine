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

auto SearchEngine::is_checkmate(BoardState &board_state) -> bool
{
  parts::PieceColor current_color = board_state.color_to_move;
  std::vector<parts::Move> possible_moves =
      parts::move_generator::calculate_possible_moves(board_state);

  // King needs to be in check to be checkmate.
  if (!board_state.king_is_checked(current_color))
  {
    return false;
  }

  // Check if all possible moves result in a checked king.
  for (parts::Move move : possible_moves)
  {
    board_state.apply_move(move);
    if (!board_state.king_is_checked(current_color))
    {
      board_state.undo_move();
      return false;
    }
    board_state.undo_move();
  }
  return true;
}

auto SearchEngine::is_stalemate(BoardState &board_state) -> bool
{
  parts::PieceColor current_color = board_state.color_to_move;
  std::vector<parts::Move> possible_moves =
      parts::move_generator::calculate_possible_moves(board_state);

  // King cannot be in check to be a stalemate.
  if (board_state.king_is_checked(current_color))
  {
    return false;
  }

  // If king is not in check, and all possible moves result in a checked king,
  // it is a stalemate.
  for (parts::Move move : possible_moves)
  {
    board_state.apply_move(move);
    if (!board_state.king_is_checked(current_color))
    {
      board_state.undo_move();
      return false;
    }
    board_state.undo_move();
  }
  return true;
}

void SearchEngine::clear_transposition_table() { transposition_table.clear(); }

// PRIVATE FUNCTIONS

auto SearchEngine::search_and_execute_best_move() -> bool
{
  running_search_flag = true;
  std::vector<std::pair<Move, int>> move_scores;
  run_iterative_deepening_search_evaluation(move_scores);
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

  // A move cannot leave your king in check. Filter out moves that do.
  std::vector<std::pair<Move, int>> move_scores_filtered;
  for (auto move_score : move_scores)
  {
    if (!game_board_state.move_leaves_king_in_check(move_score.first))
    {
      move_scores_filtered.push_back(move_score);
    }
  }

  // If all moves leave the king in check, no valid moves are available.
  if (move_scores_filtered.empty())
  {
    return false;
  }

  // Moves that re-visit a position are penalized, this will make the engine
  // avoid threefold repetition.
  for (auto move_score_filtered : move_scores_filtered)
  {
    game_board_state.apply_move(move_score_filtered.first);
    if (game_board_state.current_state_has_been_visited())
    {
      move_score_filtered.second -= 1;
    }
    game_board_state.undo_move();
  }

  // Sort moves by score so that the best move is first at index 0.
  sort_moves(move_scores_filtered);

  // Apply the best move.
  game_board_state.apply_move(move_scores_filtered[0].first);
  previous_move_evals.push(move_scores_filtered[0].second);
  int eval_score = move_scores_filtered[0].second;
  eval_score = (engine_color == PieceColor::WHITE) ? eval_score : -eval_score;
  printf("Engine's Move: %s\n",
         MoveInterface::move_to_string(move_scores_filtered[0].first).c_str());
  printf("Evaluation of Engine's Move: %d\n", eval_score);

  return true;
}

void SearchEngine::run_iterative_deepening_search_evaluation(
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
      // Recalculate possible moves for each thread.
      // NOTE: Move objects reference board pieces directly. Reusing moves
      // generated from the main game_board_state could lead to unintended
      // modifications of the main game_board_state’s pieces. For example, a
      // pawn’s 'piece_has_moved' flag might be unintentionally updated, and can
      // no longer advance two squares in the actual game. Generating fresh
      // moves for each thread’s board copy avoids these issues.
      std::vector<Move> thread_possible_moves =
          move_generator::calculate_possible_moves(
              thread_board_states[move_index]);

      // Apply move to the thread's board state.
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

void SearchEngine::run_iterative_deepening_search_pondering()
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
      // Recalculate possible moves for each thread.
      // NOTE: Move objects reference board pieces directly. Reusing moves
      // generated from the main game_board_state could lead to unintended
      // modifications of the main game_board_state’s pieces. For example, a
      // pawn’s 'piece_has_moved' flag might be unintentionally updated, and can
      // no longer advance two squares in the actual game. Generating fresh
      // moves for each thread’s board copy avoids these issues.
      std::vector<Move> thread_possible_moves =
          move_generator::calculate_possible_moves(
              thread_board_states[move_index]);

      // Apply move to the thread's board state.
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
  int alpha = INF;
  int beta = -INF;
  int eval = last_move_eval();

  // Try aspiration windows until a valid window is found.
  for (int window_increment : ASPIRATION_WINDOWS)
  {
    // If abs(eval) is greater than INF_MINUS_1000, there is a checkmate
    // sequence.
    // If eval is a checkmate line, just set alpha and beta to
    // infinity as all other windows would fail high or low.
    if (std::abs(eval) > INF_MINUS_1000 || window_increment == INF)
    {
      alpha = -INF;
      beta = INF;
    }
    else
    {
      if (eval >= beta)
      {
        beta = eval + window_increment;
      }
      if (eval <= alpha)
      {
        alpha = eval - window_increment;
      }
    }

    // Swap and negate alpha and beta and negate the eval because of the negamax
    // algorithm.
    eval = -negamax_alpha_beta_search(board_state, -beta, -alpha, depth - 1,
                                      false);

    // - Return eval if it is within the window.
    // - Return eval if search has stopped.
    // - Return eval if eval if a checkmate line has been found, no need to
    // widen the window and re-search.
    if ((eval < beta && eval > alpha) || !running_search_flag ||
        std::abs(eval) > INF_MINUS_1000)
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
                                             bool is_null_move_line) -> int
{
  // Check if the engine wants to stop searching.
  if (!running_search_flag)
  {
    return 0;
  }

  // Increment nodes visited.
  nodes_visited.fetch_add(1, std::memory_order_relaxed);

  // CHECK FOR THREEFOLD REPETITION DRAW

  // Check if the current state has been repeated three times. If it has, the
  // game is drawn. Evaluation for a draw is 0.
  if (board_state.current_state_has_been_repeated_three_times())
  {
    return 0;
  }

  // CHECKMATE DETECTION

  // If the king is no longer in the board, checkmate has occurred.
  // Return -INF evaluation for the side that has lost its king.
  if ((board_state.color_to_move == PieceColor::WHITE &&
       !board_state.white_king_is_alive) ||
      (board_state.color_to_move == PieceColor::BLACK &&
       !board_state.black_king_is_alive))
  {
    return -INF;
  }

  // TRANSPOSITION TABLE LOOKUP

  // Save the initial alpha value to later determine the correct evaluation
  // flag when we save the state in the transposition table.
  int original_alpha = alpha;

  // These values will be updated by the retrieve function of the transposition
  // table if the position has been searched before.
  // If tt_entry_best_move_index is -1, it means there is no best move
  // associated with the position.
  int eval;
  int tt_eval;
  int tt_flag;
  int tt_entry_search_depth;
  int tt_entry_best_move_index = -1;
  uint64_t hash = board_state.get_current_state_hash();
  // Check transposition table if position has been searched before.
  if (transposition_table.retrieve(hash, tt_entry_search_depth, tt_eval,
                                   tt_flag, tt_entry_best_move_index))
  {
    // Check if tt_value can be used.
    // If the depth of the stored position is greater than or equal to the
    // current depth, then the stored value is reliable. The higher the stored
    // depth, the deeper the node has been searched.
    if (depth <= tt_entry_search_depth)
    {
      switch (tt_flag)
      {
      case EXACT: // alpha < eval < beta
        return tt_eval;

      case FAILED_HIGH: // eval >= beta
        alpha = std::max(alpha, tt_eval);
        break;

      case FAILED_LOW: // eval <= alpha
        beta = std::min(beta, tt_eval);
        break;

      default:
        // Handle unexpected tt_flag value.
        printf("BREAKPOINT minimax_alpha_beta_search; tt_flag: %d", tt_flag);
      }

      // Check if we still fail high or low with the current alpha and beta.
      // If flag is FAILED_HIGH, (tt_value >= beta)
      // If flag is FAILED_LOW, (tt_value <= alpha)
      if (alpha >= beta)
      {
        return tt_eval;
      }
    }
  }

  // HANDLE LEAF NODE

  // There is a scnario where the depth is less than 0. This can happen if the
  // null move heuristic is used when the depth is 1. Null move calls negamax
  // with depth - 2 since it is skipping a turn.
  if (depth <= 0)
  {
    return evaluate_leaf_node(alpha, beta, board_state);
  }

  // NULL MOVE PRUNING HEURISTIC

  // We curently only allow one null move per search line, and only when
  // MIN_NULL_MOVE_DEPTH depth has been reached. Too many null moves will make
  // the search too shallow and return BS evals.
  if (!is_null_move_line &&
      (max_iterative_search_depth - depth) >= MIN_NULL_MOVE_DEPTH &&
      !board_state.is_end_game &&
      !board_state.king_is_checked(board_state.color_to_move))
  {

    if (do_null_move_search(board_state, alpha, beta, depth, eval))
    {
      // If we played a null move (which is theoretically a losing move) and get
      // an eval still greater than beta, then there is no need to search
      // further. If we play an actual move, our eval will most likely be
      // higher and we'd fail high (eval >= beta).
      return eval;
    }
  }

  // NEGAMAX SEARCH

  std::vector<Move> possible_moves =
      move_generator::calculate_possible_moves(board_state);

  // If there is a best move from the transposition table, move it to the
  // front to be searched first, causing more alpha beta pruning to occur.
  if (tt_entry_best_move_index >= 0 &&
      tt_entry_best_move_index < possible_moves.size())
  {
    std::swap(possible_moves[0], possible_moves[tt_entry_best_move_index]);
  }
  int best_move_index = 0;

  int max_eval = -INF;

  // Search and evaluate each move.
  run_negamax_procedure(board_state, alpha, beta, max_eval, eval, depth,
                        best_move_index, possible_moves, is_null_move_line);

  // If search has stopped, don't save the states in the transposition
  // table. This will cause invalid states to be stored with eval scores of 0.
  // This may be saved as exact values in the transposition table, causing
  // incorrect cutoffs in future searches.
  if (!running_search_flag)
  {
    return 0;
  }

  // AFTER SEARCH PROCEDURE

  // Handle checkmate evals and correct stalemate evals before saving
  // the state into the transposition table.
  handle_eval_adjustments(max_eval, board_state);

  store_state_in_transposition_table(hash, depth, max_eval, original_alpha,
                                     beta, best_move_index);

  return max_eval;
}

auto SearchEngine::evaluate_leaf_node(int alpha,
                                      int beta,
                                      BoardState &board_state) -> int
{
  // Increment leaf nodes visited.
  leaf_nodes_visited.fetch_add(1, std::memory_order_relaxed);

  // I think it is better to evaluate when it is the engine's turn to move, but
  // I am not sure.
  int depth = (engine_color == board_state.color_to_move)
                  ? MAX_QUIESCENCE_DEPTH_EVEN
                  : MAX_QUIESCENCE_DEPTH_ODD;

  return quiescenceSearch(alpha, beta, depth, board_state);
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
                                         bool &is_null_move_line)
{
  for (int move_index = 0; move_index < possible_moves.size(); ++move_index)
  {
    board_state.apply_move(possible_moves[move_index]);

    // Swap and negate alpha and beta and negate the eval because of the negamax
    // algorithm.
    eval = -negamax_alpha_beta_search(board_state, -beta, -alpha, depth - 1,
                                      is_null_move_line);
    if (eval > max_eval)
    {
      max_eval = eval;
      best_move_index = move_index;
    }
    max_eval = std::max(eval, max_eval);
    alpha = std::max(eval, alpha);

    board_state.undo_move();

    if (alpha >= beta)
    {
      break;
    }
  }
}

auto SearchEngine::do_null_move_search(BoardState &board_state,
                                       int &alpha,
                                       int &beta,
                                       int &depth,
                                       int &eval) -> bool
{
  board_state.apply_null_move();

  // Swap and negate alpha and beta and negate the eval because of the negamax
  // algorithm.
  eval = -negamax_alpha_beta_search(board_state, -beta, -(beta - 1),
                                    depth - NULL_MOVE_REDUCTION, true);
  board_state.undo_null_move();

  return eval >= beta;
}

void SearchEngine::handle_eval_adjustments(int &eval, BoardState &board_state)
{
  // Check if stalemate. If stalemate, eval is 0 as it would be a draw.
  // Read note in function declaration for more details.
  if (eval < -INF_MINUS_1000)
  {
    if (is_stalemate(board_state))
    {
      eval = 0;
      return;
    }
  }

  // Adjust checkmate evals accordingly so that boardstates that are closer to a
  // checkmate state have higher evals allowing the engine to follow the
  // sequence of moves that lead to a checkmate. See note in function
  // declaration for more details.
  if (eval > INF_MINUS_1000)
  {
    --eval;
  }
  else if (eval < -INF_MINUS_1000)
  {
    ++eval;
  }
}

void SearchEngine::store_state_in_transposition_table(uint64_t &hash,
                                                      int &depth,
                                                      int &max_eval,
                                                      int &alpha,
                                                      int &beta,
                                                      int &best_move_index,
                                                      bool is_quiescence)
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
                            best_move_index, is_quiescence);
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
    int kilo_nodes_per_second = static_cast<int>(nodes_visited / duration);

    int quiescence_node_percentage = static_cast<int>(
        (static_cast<double>(quiescence_nodes_visited) / nodes_visited) *
        PERCENTAGE);

    int normal_node_percentage = static_cast<int>(
        (static_cast<double>(nodes_visited - quiescence_nodes_visited) /
         nodes_visited) *
        PERCENTAGE);

    printf("Depth: %d, Time: %lldms\n", iterative_depth, duration);
    printf("Leaf Nodes Visited %d\n", leaf_nodes_visited.load());
    printf("Quiessence Nodes Visited: %d\n", quiescence_nodes_visited.load());
    printf("Nodes Visited: %d\n", nodes_visited.load());
    printf("Quiescence Node Percentage: %d%%\n", quiescence_node_percentage);
    printf("Normal Node Percentage: %d%%\n", normal_node_percentage);
    printf("Nodes per second: %d kN/s\n\n", kilo_nodes_per_second);
  }

  // Reset performance metrics.
  nodes_visited = 0;
  leaf_nodes_visited = 0;
  quiescence_nodes_visited = 0;
}

auto SearchEngine::quiescenceSearch(int alpha,
                                    int beta,
                                    int depth,
                                    BoardState &board_state) -> int
{
  // Check if the engine wants to stop searching.
  if (!running_search_flag)
  {
    return 0;
  }

  // Increment nodes visited.
  nodes_visited.fetch_add(1, std::memory_order_relaxed);
  quiescence_nodes_visited.fetch_add(1, std::memory_order_relaxed);

  // CHECKMATE DETECTION

  // If the king is no longer in the board, checkmate has occurred.
  // Return -INF evaluation for the side that has lost its king.
  if ((board_state.color_to_move == PieceColor::WHITE &&
       !board_state.white_king_is_alive) ||
      (board_state.color_to_move == PieceColor::BLACK &&
       !board_state.black_king_is_alive))
  {
    return -INF;
  }

  int original_alpha = alpha;

  // TRANSPOSITION TABLE LOOKUP

  uint64_t hash = board_state.get_current_state_hash();
  int tt_eval;
  int tt_search_depth;
  int tt_flag;
  int tt_best_move_index = -1;
  if (transposition_table.retrieve(hash, tt_search_depth, tt_eval, tt_flag,
                                   tt_best_move_index, true))
  {
    if (depth <= tt_search_depth)
    {
      switch (tt_flag)
      {
      case EXACT: // alpha < eval < beta
        return tt_eval;

      case FAILED_HIGH: // eval >= beta
        alpha = std::max(alpha, tt_eval);
        break;

      case FAILED_LOW: // eval <= alpha
        beta = std::min(beta, tt_eval);
        break;

      default:
        // Handle unexpected tt_flag value.
        printf("BREAKPOINT minimax_alpha_beta_search; tt_flag: %d", tt_flag);
      }

      if (alpha >= beta)
      {
        return tt_eval;
      }
    }
  }

  // QUIESCENCE SEARCH PRE-PROCEDURE

  int current_eval = position_evaluator::evaluate_position(board_state);

  if (depth <= 0)
  {
    return current_eval;
  }

  // If the eval is not within the alpha beta window, return the eval.
  // Otherwise, we will do too many unnecessary quiescence searches.
  if (current_eval >= beta)
  {
    return current_eval;
  }

  alpha = std::max(alpha, current_eval);

  std::vector<Move> possible_moves =
      move_generator::calculate_possible_moves(board_state, true);

  // Put best moves first in the list to be searched first if it exists.
  if (tt_best_move_index >= 0 && tt_best_move_index < possible_moves.size())
  {
    std::swap(possible_moves[0], possible_moves[tt_best_move_index]);
  }
  int best_move_index = 0;

  // QUIESCENCE SEARCH

  int best_eval = current_eval;

  run_quiescence_search_procedure(board_state, alpha, beta, best_eval, depth,
                                  best_move_index, current_eval,
                                  possible_moves);

  // AFTER SEARCH PROCEDURE

  // If search has stopped, don't save the states in the transposition
  // table. This will cause invalid states to be stored with eval scores of 0.
  // This may be saved as exact values in the transposition table, causing
  // incorrect cutoffs in future searches.
  if (!running_search_flag)
  {
    return 0;
  }

  // Store in transposition table with quiescence flag set to true.
  store_state_in_transposition_table(hash, depth, best_eval, original_alpha,
                                     beta, best_move_index, true);
  return best_eval;
}

void SearchEngine::run_quiescence_search_procedure(
    BoardState &board_state,
    int &alpha,
    int &beta,
    int &best_eval,
    int &depth,
    int &best_move_index,
    int &current_eval,
    std::vector<Move> &possible_moves)
{
  for (int move_index = 0; move_index < possible_moves.size(); ++move_index)
  {
    // DELTA PRUNING
    // If the current_eval is so low that the score gained from capturing the
    // piece in the move + a queen's value (900) will not bring it back up to
    // alpha, then it is most likely not worth searching the move.
    // Don't do it in the end game since it is more likely that bad moves are
    // the best a player could do.
    if (!board_state.is_end_game &&
        (current_eval + QUEEN_VALUE +
         PIECE_VALUES[static_cast<uint8_t>(
             possible_moves[move_index].captured_piece->piece_type)]) < alpha)
    {
      continue;
    }

    board_state.apply_move(possible_moves[move_index]);

    int eval = -quiescenceSearch(-beta, -alpha, depth - 1, board_state);

    board_state.undo_move();

    if (eval > best_eval)
    {
      best_eval = eval;
      best_move_index = move_index;

      if (eval >= beta)
      {
        break;
      }

      alpha = std::max(eval, alpha);
    }
  }
}

} // namespace engine::parts