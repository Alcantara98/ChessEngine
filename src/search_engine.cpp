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
  ponder_thread_handler.start_thread(MAX_SEARCH_TIME_MS);
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

void SearchEngine::clear_transposition_table() { transposition_table.clear(); }

// PRIVATE FUNCTIONS

auto SearchEngine::search_and_execute_best_move() -> bool
{
  std::vector<std::pair<Move, int>> move_scores =
      run_iterative_deepening_search_evaluation();

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
    if (!attack_check::move_leaves_king_in_check(game_board_state,
                                                 move_score.first))
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

auto SearchEngine::run_iterative_deepening_search_evaluation()
    -> std::vector<std::pair<Move, int>>
{
  std::vector<std::pair<Move, int>> move_scores;

  std::vector<Move> possible_moves =
      move_generator::calculate_possible_moves(game_board_state);

  // Create a map to store the moves to search. As the search progresses,
  // moves that are not good will be removed from this map. This allows the
  // search to focus on the most promising moves as the depth increases.
  std::map<int, bool> moves_to_search;

  // Add all possible moves to the moves_to_search map at the beginning.
  for (int move_index = 0; move_index < possible_moves.size(); ++move_index)
  {
    moves_to_search[move_index] = true;
  }

  // Search until running_search_flag is false, or max_search_depth is reached.
  for (int iterative_depth = 1; iterative_depth <= max_search_depth;
       ++iterative_depth)
  {
    best_eval_of_search_iteration.store(-INF, std::memory_order_relaxed);
    max_iterative_search_depth = iterative_depth;
    run_pvs_scout_search();

    std::vector<std::thread> search_threads;
    std::vector<std::promise<int>> promises(moves_to_search.size());
    std::vector<std::future<int>> futures;
    std::vector<BoardState> thread_board_states(moves_to_search.size(),
                                                BoardState(game_board_state));

    auto search_start_time = std::chrono::steady_clock::now();

    // search_index is used to keep track of the number of search threads
    // created. This is used as an index to access search_threads, promises,
    // futures, and thread_board_states. This is because we will only start a
    // search for moves that are in the moves_to_search map.
    int search_index = 0;

    // Search each move in a separate thread.
    for (int move_index = 0; move_index < possible_moves.size(); ++move_index)
    {
      // Check if the index is in the moves_to_search map.
      // If not, skip the move.
      if (moves_to_search.find(possible_moves[move_index].list_index) ==
          moves_to_search.end())
      {
        continue;
      }

      // Recalculate possible moves for each thread.
      // NOTE: Move objects reference board pieces directly. Reusing moves
      // generated from the main game_board_state could lead to unintended
      // modifications of the main game_board_state’s pieces. For example, a
      // pawn’s 'piece_has_moved' flag might be unintentionally updated, and can
      // no longer advance two squares in the actual game. Generating fresh
      // moves for each thread’s board copy avoids these issues.
      std::vector<Move> thread_possible_moves =
          move_generator::calculate_possible_moves(
              thread_board_states[search_index]);

      // Apply move to the thread's board state.
      thread_board_states[search_index].apply_move(
          thread_possible_moves[move_index]);
      futures.push_back(promises[search_index].get_future());

      // Start thread and search.
      search_threads.emplace_back(
          [this, &promises, search_index, iterative_depth,
           &thread_board_states]()
          {
            int eval;
            if (use_aspiration_window)
            {
              eval = run_search_with_aspiration_window(
                  thread_board_states[search_index], iterative_depth);
            }
            else
            {
              eval = -negamax_alpha_beta_search(
                  thread_board_states[search_index], -INF, INF,
                  iterative_depth - 1, false, false, 1);
            }
            promises[search_index].set_value(eval);
          });

      // Increment search_index to keep track of the index of the move in the
      // moves_to_search map.
      ++search_index;
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
      int future_index = 0;
      for (auto &possible_move : possible_moves)
      {
        if (moves_to_search.find(possible_move.list_index) !=
            moves_to_search.end())
        {
          move_scores.emplace_back(possible_move, futures[future_index].get());
          ++future_index;
        }
      }

      // Prune root moves.
      prune_root_moves(moves_to_search, move_scores, iterative_depth);
    }
    else
    {
      return move_scores;
    }

    auto search_end_time = std::chrono::steady_clock::now();
    reset_and_print_performance_matrix(iterative_depth, search_start_time,
                                       search_end_time);
  }
  // Decay the history table after each engine move.
  decay_history_table();

  return move_scores;
}

void SearchEngine::prune_root_moves(
    std::map<int, bool> &moves_to_search,
    std::vector<std::pair<Move, int>> &move_scores,
    int &current_depth)
{
  // Prune moves that are not in the top 50% of the search so far.
  if (current_depth >= MIN_ROOT_MOVE_PRUNING_DEPTH &&
      current_depth % ROOT_MOVE_PRUNING_INTERVAL == 0)
  {
    sort_moves(move_scores);

    // Get the number of moves to keep searching. This will be half of the
    // number of moves in the move_scores vector.
    int number_of_moves_to_keep_searching = moves_to_search.size() / 2;

    number_of_moves_to_keep_searching =
        std::max(number_of_moves_to_keep_searching, MIN_SEARCH_THREADS);

    moves_to_search.clear();

    for (int index = 0; index < number_of_moves_to_keep_searching; ++index)
    {
      moves_to_search[move_scores[index].first.list_index] = true;
    }
  }
}

auto SearchEngine::run_search_with_aspiration_window(BoardState &board_state,
                                                     int depth) -> int
{
  int alpha = INF;
  int beta = -INF;
  int eval = last_move_eval();

  // Try aspiration windows until a valid window is found.
  for (int index = 0; index < ASPIRATION_WINDOWS.size(); ++index)
  {
    // If abs(eval) is greater than INF_MINUS_1000, there is a checkmate
    // sequence.
    // If eval is a checkmate line, just set alpha and beta to
    // infinity as all other windows would fail high or low.
    if (std::abs(eval) > INF_MINUS_1000 || ASPIRATION_WINDOWS[index] == INF)
    {
      alpha = best_eval_of_search_iteration;
      beta = INF;
    }
    else if (index == 0)
    {
      alpha = eval - ASPIRATION_WINDOWS[index];
      beta = eval + ASPIRATION_WINDOWS[index];
    }
    else
    {
      if (eval >= beta)
      {
        beta = eval + ASPIRATION_WINDOWS[index];
        alpha = eval - 1;
      }
      if (eval <= alpha)
      {
        alpha = eval - ASPIRATION_WINDOWS[index];
      }

      if (best_eval_of_search_iteration > alpha && beta != INF)
      {
        alpha = best_eval_of_search_iteration - PAWN_VALUE / 4;
        beta = alpha + (ASPIRATION_WINDOWS[index] * 2);
      }
    }

    // Swap and negate alpha and beta and negate the eval because of the negamax
    // algorithm.
    eval = -negamax_alpha_beta_search(board_state, -beta, -alpha, depth - 1,
                                      false, false, 1);

    if (eval > best_eval_of_search_iteration && eval > alpha)
    {
      best_eval_of_search_iteration.store(eval, std::memory_order_relaxed);
    }

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
                                             bool is_forward_pruning_line,
                                             bool is_pvs_line,
                                             int ply) -> int
{
  // Check if the engine wants to stop searching.
  // Check if the current state has been repeated three times. If it has, the
  // game is drawn. Evaluation for a draw is 0.
  if (!running_search_flag ||
      board_state.current_state_has_been_repeated_three_times())
  {
    return 0;
  }

  depth = std::max(depth, 0);

  // Increment nodes visited.
  nodes_visited.fetch_add(1, std::memory_order_relaxed);

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
  int tt_best_move_index = -1;
  bool tt_move_is_singular = false;
  uint64_t hash = board_state.get_current_state_hash();

  if (handle_tt_entry(board_state, depth, tt_entry_search_depth, tt_flag,
                      tt_eval, alpha, beta, is_pvs_line, hash,
                      tt_best_move_index, tt_move_is_singular))
  {
    return tt_eval;
  }

  // CHECK WHICH SIDE IS IN CHECK
  bool other_color_is_in_check =
      (board_state.color_to_move == PieceColor::WHITE)
          ? attack_check::king_is_checked(board_state, PieceColor::BLACK)
          : attack_check::king_is_checked(board_state, PieceColor::WHITE);

  if (other_color_is_in_check)
  {
    return INF;
  }

  bool color_to_move_is_in_check =
      (board_state.color_to_move == PieceColor::WHITE)
          ? attack_check::king_is_checked(board_state, PieceColor::WHITE)
          : attack_check::king_is_checked(board_state, PieceColor::BLACK);

  if (depth == 0 && color_to_move_is_in_check)
  {
    depth = 1;
  }

  // HANDLE LEAF NODE

  // There is a scnario where the depth is less than 0. This can happen if the
  // null move heuristic is used when the depth is 1. Null move calls negamax
  // with depth - 2 since it is skipping a turn.
  if (depth <= 0)
  {
    return evaluate_leaf_node(board_state, alpha, beta,
                              color_to_move_is_in_check);
  }

  // NULL MOVE PRUNING HEURISTIC

  // We curently only allow one null move per search line, and only when
  // MIN_NULL_MOVE_DEPTH depth has been reached. Too many null moves will
  // make the search too shallow and return BS evals.
  if (!is_forward_pruning_line &&
      max_iterative_search_depth > MIN_NULL_MOVE_ITERATION_DEPTH &&
      ply >= MIN_NULL_MOVE_DEPTH && !board_state.is_end_game &&
      !color_to_move_is_in_check)
  {
    if (do_null_move_search(board_state, alpha, beta, depth, eval, ply,
                            is_pvs_line))
    {
      // If we played a null move (which is theoretically a losing move) and get
      // an eval still greater than beta, then there is no need to search
      // further. If we play an actual move, our eval will most likely be
      // higher and we'd fail high (eval >= beta).
      return eval;
    }
  }

  // PRINCIPAL VARIATION HEURISTIC

  std::vector<Move> possible_moves = move_generator::calculate_possible_moves(
      board_state, true, &history_table, false);

  put_best_move_at_front(possible_moves, tt_best_move_index);

  // NEGAMAX SEARCH

  int max_eval = -INF;
  bool best_move_is_singular = false;

  if (!do_prob_cut_search(board_state, beta, depth, eval, possible_moves,
                          color_to_move_is_in_check, is_forward_pruning_line,
                          max_eval, is_pvs_line, ply))
  {
    max_eval = -INF;

    // Search and evaluate each move.
    run_negamax_procedure(board_state, alpha, beta, max_eval, eval, depth,
                          tt_best_move_index, possible_moves,
                          is_forward_pruning_line, is_pvs_line,
                          color_to_move_is_in_check, ply, original_alpha,
                          best_move_is_singular, tt_move_is_singular);
  }

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
                                     beta, tt_best_move_index,
                                     best_move_is_singular, false);

  return max_eval;
}

auto SearchEngine::evaluate_leaf_node(BoardState &board_state,
                                      int alpha,
                                      int beta,
                                      bool color_to_move_is_in_check) -> int
{
  // Increment leaf nodes visited.
  leaf_nodes_visited.fetch_add(1, std::memory_order_relaxed);

  if (board_state.is_end_game ||
      board_state.previous_move_stack.top().captured_piece != nullptr ||
      board_state.previous_move_stack.top().promotion_piece_type !=
          PieceType::EMPTY ||
      color_to_move_is_in_check)
  {
    return quiescence_search(alpha, beta, board_state);
  }

  return position_evaluator::evaluate_position(board_state);
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
                                         bool &is_forward_pruning_line,
                                         bool &is_pvs_line,
                                         const bool &color_to_move_is_in_check,
                                         int &ply,
                                         int &original_alpha,
                                         bool &best_move_is_singular,
                                         bool &tt_move_is_singular)
{
  int quiet_move_index = 0;
  bool is_capture_move = false;
  int late_move_threshold = possible_moves.size() / 3;
  int lower_bound_move_count = 0;
  for (int move_index = 0; move_index < possible_moves.size(); ++move_index)
  {
    if (possible_moves[move_index].captured_piece != nullptr)
    {
      is_capture_move = true;
    }
    else
    {
      ++quiet_move_index;
      is_capture_move = false;
    }

    // Check if the engine wants to stop searching.
    if (!running_search_flag)
    {
      return;
    }

    board_state.apply_move(possible_moves[move_index]);

    // FUTILITY PRUNING HEURISTIC

    bool futile_move = false;

    if (!color_to_move_is_in_check && move_index != 0)
    {
      futile_move = futility_razor_prune_move(
          board_state, alpha, beta, depth, eval, move_index,
          possible_moves[move_index], ply, is_capture_move);
    }

    if (!futile_move)
    {
      run_pvs_search(
          board_state, move_index, quiet_move_index, late_move_threshold, eval,
          alpha, beta, depth, is_forward_pruning_line, is_pvs_line,
          color_to_move_is_in_check, is_capture_move, ply, tt_move_is_singular);
    }

    board_state.undo_move();

    if (eval > max_eval)
    {
      max_eval = eval;
      best_move_index = possible_moves[move_index].list_index;
    }

    if (eval < original_alpha)
    {
      ++lower_bound_move_count;
    }

    max_eval = std::max(eval, max_eval);
    alpha = std::max(eval, alpha);

    udpate_history_table(possible_moves[move_index], eval, depth, move_index,
                         alpha, beta);

    if (ply == 1 && eval > alpha && eval < beta &&
        eval < -best_eval_of_search_iteration.load(std::memory_order_relaxed))
    {
      best_eval_of_search_iteration.store(eval, std::memory_order_relaxed);
    }

    if (ply == 1 &&
        -best_eval_of_search_iteration.load(std::memory_order_relaxed) < beta)
    {
      beta = -best_eval_of_search_iteration.load(std::memory_order_relaxed);
    }

    if (alpha >= beta)
    {
      break;
    }
  }
  best_move_is_singular = possible_moves.size() - lower_bound_move_count == 1;
}

void SearchEngine::run_pvs_search(BoardState &board_state,
                                  int &move_index,
                                  int &quiet_move_index,
                                  int &late_move_threshold,
                                  int &eval,
                                  int &alpha,
                                  int &beta,
                                  int depth,
                                  bool &is_forward_pruning_line,
                                  bool &is_pvs_line,
                                  const bool &color_to_move_is_in_check,
                                  bool is_capture_move,
                                  int &ply,
                                  bool &tt_move_is_singular)
{
  int new_search_depth = depth - 1;

  if (move_index == 0 && tt_move_is_singular &&
      ply < max_iterative_search_depth + 3)
  {
    // printf("Singular move found, increasing search depth by 1.\n");
    // If tt_move_is_singular, increase search depth by 1.
    ++depth;
  }

  // LATE MOVE REDUCTION HEURISTIC

  bool make_late_move_reduction_line = false;
  if (quiet_move_index > LMR_THRESHOLD && !color_to_move_is_in_check &&
      !is_capture_move && !is_forward_pruning_line &&
      max_iterative_search_depth > MIN_LMR_ITERATION_DEPTH &&
      ply >= MIN_LMR_DEPTH &&
      board_state.previous_move_stack.top().promotion_piece_type ==
          PieceType::EMPTY)
  {
    make_late_move_reduction_line = true;
    new_search_depth -= LATE_MOVE_REDUCTION;

    if (quiet_move_index > EXTREME_LMR_THRESHOLD)
    {
      new_search_depth -= std::min(ply / LMR_PLY_REDUCTION_DIVISOR, 3);
      // If the move is late enough, we can reduce the search depth even
      // further.
      new_search_depth -=
          (quiet_move_index / LMR_EXTREME_REDUCTION_INDEX_DIVISOR);

      if (tt_move_is_singular)
      {
        // If tt_move_is_singular, increase reduction depth by 1 for quiet
        // moves.
        --new_search_depth;
      }
    }
  }

  make_late_move_reduction_line =
      (make_late_move_reduction_line || is_forward_pruning_line);

  // Do a null window search around alpha. We just want to know
  // if there is an eval that is greater than alpha. If there is, we do a full
  // search.
  eval = -negamax_alpha_beta_search(
      board_state, -alpha - 1, -alpha, new_search_depth,
      make_late_move_reduction_line, move_index == 0, ply + 1);

  if (eval > alpha && depth - 1 > new_search_depth)
  {
    eval = -negamax_alpha_beta_search(board_state, -alpha - 1, -alpha,
                                      depth - 1, is_forward_pruning_line,
                                      move_index == 0, ply + 1);
  }

  // Check if eval is greater than alpha. If it is, do a full search.
  // If window is already null, don't do a redundant search. This means parent
  // nodes are doing a null window search, and we just did a null window
  // search above.
  if (eval > alpha && beta - alpha > 1)
  {
    eval = -negamax_alpha_beta_search(board_state, -beta, -alpha, depth - 1,
                                      is_forward_pruning_line, move_index == 0,
                                      ply + 1);
  }
}

auto SearchEngine::handle_tt_entry(BoardState &board_state,
                                   int &depth,
                                   int &tt_entry_search_depth,
                                   int &tt_flag,
                                   int &tt_eval,
                                   int &alpha,
                                   int &beta,
                                   bool &is_pvs_line,
                                   uint64_t &hash,
                                   int &tt_best_move_index,
                                   bool &tt_move_is_singular) -> bool
{
  // Check transposition table if position has been searched before.
  if (transposition_table.retrieve(hash, tt_entry_search_depth, tt_eval,
                                   tt_flag, tt_best_move_index,
                                   tt_move_is_singular) &&
      !(board_state.is_end_game &&
        board_state.current_state_has_been_visited()))
  {
    if (tt_move_is_singular && tt_entry_search_depth < depth)
    {
      // Don't treat tt move as singular if the search depth is too shallow.
      tt_move_is_singular = false;
    }

    // Check if tt_value can be used.
    // If the depth of the stored position is greater than or equal to the
    // current depth, then the stored value is reliable. The higher the stored
    // depth, the deeper the node has been searched.
    if (depth <= tt_entry_search_depth)
    {
      switch (tt_flag)
      {
      case EXACT: // alpha < eval < beta
        return true;

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
        return true;
      }
    }

    if (!is_pvs_line && tt_flag == EXACT &&
        tt_eval + ((QUEEN_VALUE * 2) / tt_entry_search_depth) < alpha)
    {
      return true;
    }
  }

  return false;
}

void SearchEngine::run_pvs_scout_search()
{
  // Make a deep copy of the board state.
  BoardState board_state = BoardState(game_board_state);

  // Get TT entry.
  int tt_eval;
  int tt_flag;
  int tt_search_depth;
  int tt_best_move_index = -1;
  bool tt_move_is_singular = false;
  uint64_t hash = board_state.get_current_state_hash();
  if (!transposition_table.retrieve(hash, tt_search_depth, tt_eval, tt_flag,
                                    tt_best_move_index, tt_move_is_singular))
  {
    return;
  }

  // Get possible moves.
  std::vector<Move> possible_moves =
      move_generator::calculate_possible_moves(board_state);

  put_best_move_at_front(possible_moves, tt_best_move_index);

  // Apply the best move.
  board_state.apply_move(possible_moves[0]);

  // Run scout search.
  int alpha = last_move_eval() - (PAWN_VALUE / 4);
  int beta = alpha + 1;
  int eval = -negamax_alpha_beta_search(board_state, -beta, -alpha,
                                        max_iterative_search_depth - 1, false,
                                        true, 1);

  board_state.undo_move();

  // If eval is within the window or higher, set the best move eval to eval.
  if (eval > beta)
  {
    best_eval_of_search_iteration.store(eval, std::memory_order_relaxed);
  }
}

auto SearchEngine::do_null_move_search(BoardState &board_state,
                                       int &alpha,
                                       int &beta,
                                       int &depth,
                                       int &eval,
                                       int &ply,
                                       bool &is_pvs_line) -> bool
{
  board_state.apply_null_move();

  // We search with a null window around beta (beta to beta + 1). We just need
  // to find out if there is an eval greater than beta. If there is, we don't
  // need to search further.
  int reduction = NULL_MOVE_REDUCTION;
  if (!is_pvs_line)
  {
    reduction += depth / NULL_MOVE_ADDITIONAL_DEPTH_DIVISOR;
  }

  eval = -negamax_alpha_beta_search(board_state, -beta, -(beta - 1),
                                    depth - reduction, true, false, ply + 1);
  board_state.undo_null_move();

  return eval >= beta;
}

auto SearchEngine::do_prob_cut_search(BoardState &board_state,
                                      int &beta,
                                      int &depth,
                                      int &eval,
                                      std::vector<Move> &possible_moves,
                                      bool color_to_move_is_in_check,
                                      bool &is_forward_pruning_line,
                                      int &max_eval,
                                      bool &is_pvs_line,
                                      int &ply) -> bool
{
  // PROBABILITY CUT HEURISTIC
  if (is_forward_pruning_line || is_pvs_line || color_to_move_is_in_check ||
      depth <= PROB_CUT_DEPTH_THRESHOLD || beta > INF_MINUS_1000 ||
      ply <= MIN_PROB_CUT_DEPTH)
  {
    return false;
  }

  for (Move move : possible_moves)
  {
    if (!running_search_flag)
    {
      break;
    }

    board_state.apply_move(move);

    int prob_cut_beta_threshold = beta + PAWN_VALUE;

    // Check with quiescence search first with null window around
    // prob_cut_beta_threshold.
    eval = -quiescence_search(-prob_cut_beta_threshold,
                              -prob_cut_beta_threshold + 1, board_state);

    int prob_cut_depth = std::max(std::min(depth - 4, depth / 2), 0);
    if (eval >= prob_cut_beta_threshold && prob_cut_depth > 0)
    {
      // If move survives quiescence search, do normal reduced depth search
      // with null window around prob_cut_beta_threshold.
      eval = -negamax_alpha_beta_search(board_state, -prob_cut_beta_threshold,
                                        -prob_cut_beta_threshold + 1,
                                        prob_cut_depth, true, false, ply + 1);
    }

    board_state.undo_move();

    max_eval = std::max(eval, max_eval);

    if (eval >= prob_cut_beta_threshold)
    {
      return true;
    }
  }

  return false;
}

void SearchEngine::handle_eval_adjustments(int &eval, BoardState &board_state)
{
  // Check if stalemate. If stalemate, eval is 0 as it would be a draw.
  // Read note in function declaration for more details.
  if (eval < -INF_MINUS_1000)
  {
    if (attack_check::is_stalemate(board_state))
    {
      eval = 0;
      return;
    }
  }

  // Adjust checkmate evals accordingly so that boardstates that are closer to
  // a checkmate state have higher evals allowing the engine to follow the
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

void SearchEngine::store_state_in_transposition_table(
    uint64_t &hash,
    int &depth,
    int &max_eval,
    int &alpha,
    int &beta,
    int &best_move_index,
    bool best_move_is_singular,
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
                            best_move_index, best_move_is_singular,
                            is_quiescence);
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

auto SearchEngine::quiescence_search(int alpha,
                                     int beta,
                                     BoardState &board_state) -> int
{
  // Check if the engine wants to stop searching.
  if (!running_search_flag)
  {
    return 0;
  }

  // Check if the current state has been repeated three times. If it has, the
  // game is drawn. Evaluation for a draw is 0.
  if (board_state.current_state_has_been_repeated_three_times())
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
  bool tt_is_singular_move = false;
  if (transposition_table.retrieve(hash, tt_search_depth, tt_eval, tt_flag,
                                   tt_best_move_index, tt_is_singular_move,
                                   true))
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

  // QUIESCENCE SEARCH PRE-PROCEDURE

  int current_eval = position_evaluator::evaluate_position(board_state);

  // If the eval is not within the alpha beta window, return the eval.
  // Otherwise, we will do too many unnecessary quiescence searches.
  if (current_eval >= beta)
  {
    return current_eval;
  }

  alpha = std::max(alpha, current_eval);

  // PRINCIPAL VARIATION HEURISTIC

  std::vector<Move> possible_moves = move_generator::calculate_possible_moves(
      board_state, true, &history_table, true);

  put_best_move_at_front(possible_moves, tt_best_move_index);

  // QUIESCENCE SEARCH

  int best_eval = current_eval;

  run_quiescence_search_procedure(board_state, alpha, beta, best_eval,
                                  tt_best_move_index, current_eval,
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
  int tt_depth = 0;
  store_state_in_transposition_table(hash, tt_depth, best_eval, original_alpha,
                                     beta, tt_best_move_index, false, true);
  return best_eval;
}

void SearchEngine::run_quiescence_search_procedure(
    BoardState &board_state,
    int &alpha,
    int &beta,
    int &best_eval,
    int &best_move_index,
    int &current_eval,
    std::vector<Move> &possible_moves)
{
  for (auto move : possible_moves)
  {
    // Check if the move can be delta pruned.
    if (delta_prune_move(board_state, move, current_eval, alpha))
    {
      continue;
    }

    board_state.apply_move(move);

    int eval = -quiescence_search(-beta, -alpha, board_state);

    board_state.undo_move();

    if (eval > best_eval)
    {
      best_eval = eval;
      best_move_index = move.list_index;

      if (eval >= beta)
      {
        break;
      }

      alpha = std::max(eval, alpha);
    }
  }
}

auto SearchEngine::delta_prune_move(const BoardState &board_state,
                                    const Move &move,
                                    const int &current_eval,
                                    const int &alpha) -> bool
{
  // If the current_eval is so low that the score gained from capturing the
  // piece in the move + 2 pawn values (100) will not bring it back up to
  // alpha, then it is most likely not worth searching the move.
  // Don't do it in the end game since it is more likely that bad moves are
  // the best a player could do.

  return (
      !board_state.is_end_game &&
      (current_eval + (PAWN_VALUE * 2) +
       PIECE_VALUES[static_cast<uint8_t>(move.captured_piece->piece_type)]) <
          alpha);
}

auto SearchEngine::futility_razor_prune_move(BoardState &board_state,
                                             const int &alpha,
                                             const int &beta,
                                             const int &depth,
                                             int &eval,
                                             int &quiet_move_index,
                                             Move &move,
                                             int &ply,
                                             bool &is_capture_move) -> bool
{
  if (alpha < -INF_MINUS_1000 || ply < MIN_RAZOR_PRUNING_PLY ||
      move.promotion_piece_type != PieceType::EMPTY ||
      attack_check::king_is_checked(board_state, board_state.color_to_move))
  {
    return false;
  }

  // Get static evaluation of the board state.
  // Eval has to be negated because we are still in the perspective of the
  // parent node.
  eval = -position_evaluator::evaluate_position(board_state);

  // RAZOR HEURISTIC
  int razor_margin =
      std::min(RAZOR_BASE_MARGIN + (depth * depth * RAZOR_MARGIN_MULTIPLIER),
               RAZOR_MAX_MARGIN);

  if (eval + razor_margin < alpha)
  {
    eval = -quiescence_search(alpha, beta, board_state);
    return true;
  }

  if (is_capture_move && ply < MIN_FUTILITY_PRUNING_PLY)
  {
    return false;
  }

  // FUTILITY PRUNING

  int futility_cutoff_index = 3 + ((depth * depth) / 2);

  int futility_margin = 0;
  if (quiet_move_index < futility_cutoff_index)
  {
    futility_margin += (PAWN_VALUE * depth) - (quiet_move_index * 2);
  }

  return eval + futility_margin < alpha;
}

void SearchEngine::udpate_history_table(const Move &move,
                                        const int &eval,
                                        const int &depth,
                                        const int &move_index,
                                        const int &alpha,
                                        const int &beta)
{
  int move_value;

  // Beta cutoff - really good move.
  if (alpha >= beta)
  {
    // If PV node, we only increase by depth * 2 so we don't overfit and
    // become too biased to certain moves.
    if (move_index == 0)
    {
      move_value = depth * 2;
    }
    else
    {
      move_value = depth * depth;
    }
  }
  // Move improved alpha - Good Move.
  else if (eval > alpha)
  {
    // If PV node, we only increase by depth / 2 so we don't overfit and
    // become too biased to certain moves.
    if (move_index == 0)
    {
      move_value = depth / 2;
    }
    else
    {
      move_value = depth;
    }
  }
  // Move did not improve alpha - bad move.
  else
  {
    move_value = -depth;
  }

  const int &color = static_cast<int>(move.moving_piece->piece_color);
  const int &piece_type = static_cast<int>(move.moving_piece->piece_type);
  const int &to_x = move.to_x;
  const int &to_y = move.to_y;

  history_table[color][piece_type][to_x][to_y] += move_value;
}

void SearchEngine::decay_history_table()
{
  for (auto &color : history_table)
  {
    for (auto &piece_type : color)
    {
      for (auto &file : piece_type)
      {
        for (auto &entry : file)
        {
          // Decay all entries in the history table by 0.9.
          entry = (entry * DECAY_RATE_NUMERATOR) / DECAY_RATE_DENOMINATOR;
        }
      }
    }
  }
}

void SearchEngine::put_best_move_at_front(std::vector<Move> &possible_moves,
                                          int &best_move_index)
{
  // If there is a best move from the transposition table, move it to the
  // front to be searched first, causing more alpha beta pruning to occur.
  if (best_move_index >= 0 && best_move_index < possible_moves.size())
  {
    for (int move_idnex = 0; move_idnex < possible_moves.size(); ++move_idnex)
    {
      if (possible_moves[move_idnex].list_index == best_move_index)
      {
        std::swap(possible_moves[0],
                  possible_moves[move_idnex]); // Swap the actual elements
        break;
      }
    }
  }
}
} // namespace engine::parts
