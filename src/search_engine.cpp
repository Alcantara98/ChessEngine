#include "search_engine.h"
#include "attack_check.h"
#include "board_state.h"
#include "engine_constants.h"
#include "move_generator.h"
#include "move_interface.h"
#include "position_evaluator.h"

#include <algorithm>
#include <thread>

namespace engine::parts
{
// CONSTRUCTORS

SearchEngine::SearchEngine(BoardState &board_state, bool is_uci)
    : game_board_state(board_state),
      transposition_table(MAX_TRANSPOSITION_TABLE_SIZE), is_uci(is_uci)
{
} // Initialize with a max size

// PUBLIC FUNCTIONS

void SearchEngine::start_engine_search()
{
  if (engine_is_searching())
  {
    return;
  }
  search_thread_handler.start_thread(max_search_time_milliseconds);
}

void SearchEngine::stop_engine_search() { search_thread_handler.stop_thread(); }

auto SearchEngine::wait_for_search_and_get_best_move() -> std::string
{
  search_thread_handler.wait_until_done();
  return best_move_string;
}

void SearchEngine::start_engine_pondering()
{
  if (engine_is_pondering)
  {
    return;
  }
  engine_is_pondering = true;
  ponder_thread_handler.start_thread(MAX_PONDER_SEARCH_TIME_MS);
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
  std::vector<std::pair<Move, int>> move_scores = run_lazy_smp_search();

  sort_moves(move_scores);

  if (!is_uci && show_move_evaluations)
  {
    // Print all moves and their evaluations.
    for (auto move_score : move_scores)
    {
      printf("Move: %s, Score: %d\n",
             parts::move_interface::move_to_string(move_score.first).c_str(),
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
  best_move_string =
      parts::move_interface::move_to_string(move_scores_filtered[0].first);

  if (!is_uci)
  {
    int eval_score = move_scores_filtered[0].second;
    eval_score = (engine_color == PieceColor::WHITE) ? eval_score : -eval_score;
    printf("Evaluation of Engine's Move: %d\n", eval_score);
    printf("Engine's Move: %s\n", best_move_string.c_str());
  }

  return true;
}

auto SearchEngine::run_lazy_smp_search() -> std::vector<std::pair<Move, int>>
{
  std::vector<std::pair<Move, int>> move_scores;

  std::vector<std::thread> search_threads;
  std::vector<BoardState> thread_board_states(MAX_SEARCH_THREADS - 1,
                                              BoardState(game_board_state));

  // Start main thread.
  search_threads.emplace_back(
      [this, &move_scores]()
      { move_scores = run_iterative_deepening_search(0, game_board_state); });

  // Start helper threads.
  for (int thread_index = 1; thread_index < MAX_SEARCH_THREADS; ++thread_index)
  {
    BoardState &thread_board_state = thread_board_states[thread_index - 1];
    {
      search_threads.emplace_back(
          [this, thread_index, &thread_board_state]() {
            (void)run_iterative_deepening_search(thread_index,
                                                 thread_board_state);
          });
    }
  }

  // TODO: Stop all helper threads when main thread is done.

  // Wait for search threads to finish.
  for (auto &search_thread : search_threads)
  {
    search_thread.join();
  }

  // Decay the all history tables after each engine move.
  for (auto &history_table : history_tables)
  {
    decay_history_table(history_table);
  }

  return move_scores;
}

auto SearchEngine::run_iterative_deepening_search(int thread_index,
                                                  BoardState &board_state)
    -> std::vector<std::pair<Move, int>>
{
  std::vector<std::pair<Move, int>> final_move_scores;
  int iterative_depth_start = 1;

  // This adds varience to helper thread search trees which is essential for
  // Lazy SMP.
  if (thread_index > MAX_SEARCH_THREADS / 2)
  {
    iterative_depth_start = 2;
  }

  // Search until running_search_flag is false, or max_search_depth is reached.
  for (int iterative_depth = iterative_depth_start;
       iterative_depth <= max_search_depth; ++iterative_depth)
  {
    std::vector<std::pair<Move, int>> move_scores;

    auto search_start_time = std::chrono::steady_clock::now();

    move_scores = root_negamax_alpha_beta_search(board_state, -INF, INF,
                                                 iterative_depth, thread_index);

    if (running_search_flag)
    {
      final_move_scores = move_scores;
    }
    else
    {
      break;
    }

    auto search_end_time = std::chrono::steady_clock::now();
    if (thread_index == 0)
    {
      // NOTE: Only the main thread should print the performance matrix.
      reset_and_print_performance_matrix(iterative_depth, search_start_time,
                                         search_end_time);
    }
  }

  return final_move_scores;
}

auto SearchEngine::root_negamax_alpha_beta_search(
    BoardState &board_state, int alpha, int beta, int depth, int thread_index)
    -> std::vector<std::pair<Move, int>>
{
  std::vector<std::pair<Move, int>> move_scores;
  bool is_pvs_line = false;
  bool is_forward_pruning_line = false;
  int ply = 0;
  int alpha_search = alpha;

  // Check if the engine wants to stop searching.
  // Check if the current state has been repeated three times. If it has, the
  // game is drawn. Evaluation for a draw is 0.
  if (!running_search_flag ||
      board_state.current_state_has_been_repeated_three_times())
  {
    return move_scores;
  }

  depth = std::max(depth, 0);

  // Increment nodes visited.
  nodes_visited.fetch_add(1, std::memory_order_relaxed);

  // TRANSPOSITION TABLE LOOKUP

  // Save the initial alpha value to later determine the correct evaluation
  // flag when we save the state in the transposition table.
  int original_alpha = alpha;

  // These values will be updated by the retrieve function of the
  // transposition table if the position has been searched before. If
  // tt_entry_best_move_index is -1, it means there is no best move associated
  // with the position.
  int eval;
  int tt_eval;
  int tt_flag;
  int tt_entry_search_depth;
  int tt_best_move_index = -1;
  uint64_t hash = board_state.get_current_state_hash();

  transposition_table.retrieve(hash, tt_entry_search_depth, tt_eval, tt_flag,
                               tt_best_move_index);

  // CHECK WHICH SIDE IS IN CHECK
  bool other_color_is_in_check =
      (board_state.color_to_move == PieceColor::WHITE)
          ? attack_check::king_is_checked(board_state, PieceColor::BLACK)
          : attack_check::king_is_checked(board_state, PieceColor::WHITE);

  if (other_color_is_in_check)
  {
    return move_scores;
  }

  bool color_to_move_is_in_check =
      (board_state.color_to_move == PieceColor::WHITE)
          ? attack_check::king_is_checked(board_state, PieceColor::WHITE)
          : attack_check::king_is_checked(board_state, PieceColor::BLACK);

  // PRINCIPAL VARIATION HEURISTIC

  std::vector<Move> possible_moves = move_generator::calculate_possible_moves(
      board_state, true, &history_tables[thread_index], false);

  put_best_move_at_front(possible_moves, tt_best_move_index);

  // NEGAMAX SEARCH

  int max_eval = -INF;
  max_eval = -INF;

  // SEARCH
  int quiet_move_index = 0;
  for (int move_index = 0; move_index < possible_moves.size(); ++move_index)
  {
    if (possible_moves[move_index].captured_piece == nullptr)
    {
      ++quiet_move_index;
    }

    // Check if the engine wants to stop searching.
    if (!running_search_flag)
    {
      break;
    }

    board_state.apply_move(possible_moves[move_index]);

    int search_depth = depth;

    // LMR HEURISTIC
    if (quiet_move_index > LMR_THRESHOLD * 3 && !color_to_move_is_in_check &&
        (depth + ply) > MIN_LMR_ITERATION_DEPTH &&
        board_state.previous_move_stack.top().promotion_piece_type ==
            PieceType::EMPTY)
    {
      search_depth -= LATE_MOVE_REDUCTION - 1;
    }

    // Do a null window search around alpha. We just want to know
    // if there is an eval that is greater than alpha. If there is, we do a
    // full search.
    alpha_search = alpha - 2;
    if (thread_index > 0 || alpha < -INF_MINUS_1000)
    {
      alpha_search = alpha;
    }
    // Do a null window search around alpha. We just want to know
    // if there is an eval that is greater than alpha. If there is, we do a full
    // search.
    int beta_search = alpha_search + 1;
    eval = -negamax_alpha_beta_search(board_state, -beta_search, -alpha_search,
                                      search_depth - 1, is_forward_pruning_line,
                                      move_index == 0, ply + 1, thread_index);

    if (eval > alpha_search)
    {
      eval = -negamax_alpha_beta_search(
          board_state, -INF, -alpha_search, search_depth - 1,
          is_forward_pruning_line, move_index == 0, ply + 1, thread_index);
    }

    move_scores.emplace_back(possible_moves[move_index], eval);

    board_state.undo_move();

    if (eval > max_eval)
    {
      max_eval = eval;
      tt_best_move_index = possible_moves[move_index].list_index;
    }

    max_eval = std::max(eval, max_eval);
    alpha = std::max(eval, alpha);

    update_history_table(possible_moves[move_index], eval, search_depth,
                         move_index, alpha, beta, history_tables[thread_index]);

    if (alpha >= beta)
    {
      break;
    }
  }

  // If search has stopped, don't save the states in the transposition
  // table. This will cause invalid states to be stored with eval scores of 0.
  // This may be saved as exact values in the transposition table, causing
  // incorrect cutoffs in future searches.
  if (!running_search_flag)
  {
    return move_scores;
  }

  // AFTER SEARCH PROCEDURE

  // Handle checkmate evals and correct stalemate evals before saving
  // the state into the transposition table.
  handle_eval_adjustments(max_eval, board_state);

  store_state_in_transposition_table(hash, depth, max_eval, original_alpha,
                                     beta, tt_best_move_index, false);

  return move_scores;
}

auto SearchEngine::negamax_alpha_beta_search(BoardState &board_state,
                                             int alpha,
                                             int beta,
                                             int depth,
                                             bool is_forward_pruning_line,
                                             bool is_pvs_line,
                                             int ply,
                                             int thread_index) -> int
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

  // These values will be updated by the retrieve function of the
  // transposition table if the position has been searched before. If
  // tt_entry_best_move_index is -1, it means there is no best move associated
  // with the position.
  int eval;
  int tt_eval;
  int tt_flag;
  int tt_entry_search_depth;
  int tt_best_move_index = -1;
  uint64_t hash = board_state.get_current_state_hash();

  if (handle_tt_entry(board_state, depth, tt_entry_search_depth, tt_flag,
                      tt_eval, alpha, beta, is_pvs_line, hash,
                      tt_best_move_index))
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
                              color_to_move_is_in_check, thread_index);
  }

  // NULL MOVE PRUNING HEURISTIC

  // We curently only allow one null move per search line, and only when
  // MIN_NULL_MOVE_DEPTH depth has been reached. Too many null moves will
  // make the search too shallow and return BS evals.
  if (!is_forward_pruning_line &&
      (depth + ply) > MIN_NULL_MOVE_ITERATION_DEPTH &&
      ply >= MIN_NULL_MOVE_DEPTH && !board_state.is_end_game &&
      !color_to_move_is_in_check)
  {
    if (do_null_move_search(board_state, alpha, beta, depth, eval, ply,
                            is_pvs_line, thread_index))
    {
      // If we played a null move (which is theoretically a losing move) and
      // get an eval still greater than beta, then there is no need to search
      // further. If we play an actual move, our eval will most likely be
      // higher and we'd fail high (eval >= beta).
      return eval;
    }
  }

  // PRINCIPAL VARIATION HEURISTIC

  std::vector<Move> possible_moves = move_generator::calculate_possible_moves(
      board_state, true, &history_tables[thread_index], false);

  put_best_move_at_front(possible_moves, tt_best_move_index);

  // NEGAMAX SEARCH

  int max_eval = -INF;

  // Search and evaluate each move.
  run_negamax_procedure(board_state, alpha, beta, max_eval, eval, depth,
                        tt_best_move_index, possible_moves,
                        is_forward_pruning_line, is_pvs_line,
                        color_to_move_is_in_check, ply, thread_index);

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
                                     beta, tt_best_move_index, false);

  return max_eval;
}

auto SearchEngine::evaluate_leaf_node(BoardState &board_state,
                                      int alpha,
                                      int beta,
                                      bool color_to_move_is_in_check,
                                      int thread_index) -> int
{
  // Increment leaf nodes visited.
  leaf_nodes_visited.fetch_add(1, std::memory_order_relaxed);

  return quiescence_search(alpha, beta, board_state, thread_index);
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
                                         int thread_index)
{
  int quiet_move_index = 0;
  bool is_capture_move = false;
  int late_move_threshold = possible_moves.size() / 3;
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
      futile_move = futility_prune_move(board_state, alpha, beta, depth, eval,
                                        move_index, possible_moves[move_index],
                                        ply, is_capture_move, thread_index);
    }

    if (!futile_move)
    {
      run_pvs_search(
          board_state, move_index, quiet_move_index, late_move_threshold, eval,
          alpha, beta, depth, is_forward_pruning_line, is_pvs_line,
          color_to_move_is_in_check, is_capture_move, ply, thread_index);
    }

    board_state.undo_move();

    if (eval > max_eval)
    {
      max_eval = eval;
      best_move_index = possible_moves[move_index].list_index;
    }

    max_eval = std::max(eval, max_eval);
    alpha = std::max(eval, alpha);

    update_history_table(possible_moves[move_index], eval, depth, move_index,
                         alpha, beta, history_tables[thread_index]);

    if (alpha >= beta)
    {
      break;
    }
  }
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
                                  int thread_index)
{
  int new_search_depth = depth - 1;

  // LATE MOVE REDUCTION HEURISTIC

  bool make_late_move_reduction_line = false;
  if (quiet_move_index > LMR_THRESHOLD && !color_to_move_is_in_check &&
      !is_capture_move && !is_forward_pruning_line &&
      (depth + ply) > MIN_LMR_ITERATION_DEPTH && ply >= MIN_LMR_DEPTH &&
      board_state.previous_move_stack.top().promotion_piece_type ==
          PieceType::EMPTY)
  {
    make_late_move_reduction_line = true;
    new_search_depth -= LATE_MOVE_REDUCTION;

    if (quiet_move_index > EXTREME_LMR_THRESHOLD)
    {
      // If the move is late enough, we can reduce the search depth even
      // further.
      new_search_depth -=
          (quiet_move_index / LMR_EXTREME_REDUCTION_INDEX_DIVISOR);
    }
  }

  make_late_move_reduction_line =
      (make_late_move_reduction_line || is_forward_pruning_line);

  // Do a null window search around alpha. We just want to know
  // if there is an eval that is greater than alpha. If there is, we do a full
  // search.
  eval = -negamax_alpha_beta_search(
      board_state, -alpha - 1, -alpha, new_search_depth,
      make_late_move_reduction_line, move_index == 0, ply + 1, thread_index);

  if (eval > alpha && depth - 1 > new_search_depth)
  {
    eval = -negamax_alpha_beta_search(board_state, -alpha - 1, -alpha,
                                      depth - 1, is_forward_pruning_line,
                                      move_index == 0, ply + 1, thread_index);
  }

  // Check if eval is greater than alpha. If it is, do a full search.
  // If window is already null, don't do a redundant search. This means parent
  // nodes are doing a null window search, and we just did a null window
  // search above.
  if (eval > alpha && beta - alpha > 1)
  {
    eval = -negamax_alpha_beta_search(board_state, -beta, -alpha, depth - 1,
                                      is_forward_pruning_line, move_index == 0,
                                      ply + 1, thread_index);
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
                                   int &tt_best_move_index) -> bool
{
  // Check transposition table if position has been searched before.
  if (transposition_table.retrieve(hash, tt_entry_search_depth, tt_eval,
                                   tt_flag, tt_best_move_index) &&
      !(board_state.is_end_game &&
        board_state.current_state_has_been_visited()))
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

auto SearchEngine::do_null_move_search(BoardState &board_state,
                                       int &alpha,
                                       int &beta,
                                       int &depth,
                                       int &eval,
                                       int &ply,
                                       bool &is_pvs_line,
                                       int thread_index) -> bool
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
                                    depth - reduction, true, false, ply + 1,
                                    thread_index);
  board_state.undo_null_move();

  return eval >= beta;
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
  // Print performance metrics to user.
  if (!is_uci && ((show_performance && !engine_is_pondering) ||
                  (show_ponder_performance && engine_is_pondering)))
  {
    // Calculate duration of search.
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(
                        search_end_time - search_start_time)
                        .count();

    // Avoid division by zero.
    if (duration == 0)
    {
      duration = 1;
    }
    auto kilo_nodes_per_second = static_cast<size_t>(
        nodes_visited * NANOSECONDS_IN_MILLISECOND / duration);

    int quiescence_node_percentage = 0;
    int normal_node_percentage = 0;

    // Avoid division by zero.
    if (nodes_visited != 0)
    {
      quiescence_node_percentage = static_cast<int>(
          (static_cast<double>(quiescence_nodes_visited) / nodes_visited) *
          PERCENTAGE);

      normal_node_percentage = static_cast<int>(
          (static_cast<double>(nodes_visited - quiescence_nodes_visited) /
           nodes_visited) *
          PERCENTAGE);
    }

    printf("Depth: %d, Time: %ldms\n", iterative_depth,
           duration / NANOSECONDS_IN_MILLISECOND);
    printf("Leaf Nodes Visited %zu\n", leaf_nodes_visited.load());
    printf("Quiessence Nodes Visited: %zu\n", quiescence_nodes_visited.load());
    printf("Nodes Visited: %zu\n", nodes_visited.load());
    printf("Quiescence Node Percentage: %d%%\n", quiescence_node_percentage);
    printf("Normal Node Percentage: %d%%\n", normal_node_percentage);
    printf("Nodes per second: %lu kN/s\n\n", kilo_nodes_per_second);
  }

  // Reset performance metrics.
  nodes_visited = 0;
  leaf_nodes_visited = 0;
  quiescence_nodes_visited = 0;
}

auto SearchEngine::quiescence_search(int alpha,
                                     int beta,
                                     BoardState &board_state,
                                     int thread_index) -> int
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
  if (transposition_table.retrieve(hash, tt_search_depth, tt_eval, tt_flag,
                                   tt_best_move_index, true))
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
      board_state, true, &history_tables[thread_index], true);

  put_best_move_at_front(possible_moves, tt_best_move_index);

  // QUIESCENCE SEARCH

  int best_eval = current_eval;

  run_quiescence_search_procedure(board_state, alpha, beta, best_eval,
                                  tt_best_move_index, current_eval,
                                  possible_moves, thread_index);

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
                                     beta, tt_best_move_index, true);
  return best_eval;
}

void SearchEngine::run_quiescence_search_procedure(
    BoardState &board_state,
    int &alpha,
    int &beta,
    int &best_eval,
    int &best_move_index,
    int &current_eval,
    std::vector<Move> &possible_moves,
    int thread_index)
{
  for (auto move : possible_moves)
  {
    // Check if the move can be delta pruned.
    if (delta_prune_move(board_state, move, current_eval, alpha))
    {
      continue;
    }

    board_state.apply_move(move);

    int eval = -quiescence_search(-beta, -alpha, board_state, thread_index);

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

auto SearchEngine::futility_prune_move(BoardState &board_state,
                                       const int &alpha,
                                       const int &beta,
                                       const int &depth,
                                       int &eval,
                                       int &quiet_move_index,
                                       Move &move,
                                       int &ply,
                                       bool &is_capture_move,
                                       int thread_index) -> bool
{
  if (alpha < -INF_MINUS_1000 ||
      move.promotion_piece_type != PieceType::EMPTY ||
      attack_check::king_is_checked(board_state, board_state.color_to_move) ||
      is_capture_move || ply < MIN_FUTILITY_PRUNING_PLY ||
      depth > TT_FUTILITY_PRUNING_MIN_DEPTH)
  {
    return false;
  }

  // Get static evaluation of the board state.
  // Eval has to be negated because we are still in the perspective of the
  // parent node.
  eval = -position_evaluator::evaluate_position(board_state);

  int futility_cutoff_index = 3 + ((depth * depth) / 2);

  int futility_margin = 0;
  if (quiet_move_index < futility_cutoff_index)
  {
    futility_margin += (PAWN_VALUE * depth * depth) - (quiet_move_index * 2);
  }

  if (eval + futility_margin < alpha)
  {
    return true;
  }

  return false;
}

void SearchEngine::update_history_table(const Move &move,
                                        const int &eval,
                                        const int &depth,
                                        const int &move_index,
                                        const int &alpha,
                                        const int &beta,
                                        history_table_type &history_table)
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

void SearchEngine::decay_history_table(history_table_type &history_table)
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
