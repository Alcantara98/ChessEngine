#include "search_engine.h"
#include "attack_check.h"
#include "board_state.h"
#include "engine_constants.h"
#include "move_generator.h"
#include "move_interface.h"
#include "node_context.h"
#include "position_evaluator.h"

#include <algorithm>
#include <cmath>
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
  std::vector<Move> possible_moves = move_generator::calculate_possible_moves(
      game_board_state, false, nullptr, false);

  // Replace move scores with moves from possible moves.
  // NOTE: This is necessary because moves contain pointers to actual pieces on
  // the board.
  for (auto &move_score : move_scores)
  {
    move_score.first = possible_moves[move_score.first.list_index];
  }

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
  std::vector<BoardState> thread_board_states(MAX_SEARCH_THREADS,
                                              BoardState(game_board_state));

  // Start main thread.
  search_threads.emplace_back(
      [this, &move_scores, &thread_board_states]() {
        move_scores = run_iterative_deepening_search(0, thread_board_states[0]);
      });

  // Start helper threads.
  for (int thread_index = 1; thread_index < MAX_SEARCH_THREADS; ++thread_index)
  {
    BoardState &thread_board_state = thread_board_states[thread_index];
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

    int previous_eval = 0;
    if (!final_move_scores.empty())
    {
      previous_eval = final_move_scores[0].second;
    }

    move_scores = run_search_with_aspiration_window(
        thread_index, previous_eval, iterative_depth, board_state);

    if (running_search_flag)
    {
      sort_moves(move_scores);
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
                                         search_end_time, move_scores);
    }
  }

  return final_move_scores;
}

auto SearchEngine::run_search_with_aspiration_window(int thread_index,
                                                     int previous_eval,
                                                     int depth,
                                                     BoardState &board_state)
    -> std::vector<std::pair<Move, int>>
{
  std::vector<std::pair<Move, int>> move_scores;

  for (int aspiration_window : ASPIRATION_WINDOWS)
  {
    int alpha = previous_eval - aspiration_window;
    int beta = previous_eval + aspiration_window;
    move_scores = root_negamax_alpha_beta_search(
        new_context(board_state, alpha, beta, depth, false, false, 0,
                    thread_index, false, depth));
    if (!running_search_flag || move_scores.empty() ||
        (move_scores[0].second > alpha && move_scores[0].second < beta))
    {
      break;
    }
  }
  return move_scores;
}

auto SearchEngine::root_negamax_alpha_beta_search(NodeContext context)
    -> std::vector<std::pair<Move, int>>
{
  std::vector<std::pair<Move, int>> move_scores;

  // Check if the engine wants to stop searching.
  // Check if the current state has been repeated three times. If it has, the
  // game is drawn. Evaluation for a draw is 0.
  if (!running_search_flag ||
      context.board_state.current_state_has_been_repeated_three_times())
  {
    return move_scores;
  }

  if (context.thread_index == 0)
  {
    nodes_visited.fetch_add(1, std::memory_order_relaxed);
  }
  nodes_visited_all_threads.fetch_add(1, std::memory_order_relaxed);

  // TRANSPOSITION TABLE LOOKUP

  transposition_table.retrieve(context.hash, context.tt_entry_search_depth,
                               context.tt_eval, context.tt_flag,
                               context.tt_best_move_index);

  // CHECK IF IN CHECK
  context.king_in_check =
      (context.board_state.color_to_move == PieceColor::WHITE)
          ? attack_check::king_is_checked(context.board_state,
                                          PieceColor::WHITE)
          : attack_check::king_is_checked(context.board_state,
                                          PieceColor::BLACK);

  // PRINCIPAL VARIATION HEURISTIC

  std::vector<Move> possible_moves = move_generator::calculate_possible_moves(
      context.board_state, true, &history_tables[context.thread_index], false);

  put_best_move_at_front(possible_moves, context.tt_best_move_index);

  // NEGAMAX SEARCH

  context.max_eval = -INF;

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

    context.board_state.apply_move(possible_moves[move_index]);

    int search_depth = context.depth;

    // LMR HEURISTIC
    if (quiet_move_index > LMR_THRESHOLD * 3 &&
        context.depth >= MIN_LMR_DEPTH && !context.king_in_check &&
        (context.depth + context.ply) > MIN_LMR_ITERATION_DEPTH &&
        context.board_state.previous_move_stack.top().promotion_piece_type ==
            PieceType::EMPTY)
    {
      search_depth -= LATE_MOVE_REDUCTION - 1;
    }

    int alpha_search = context.alpha;
    if (context.thread_index == 0)
    {
      alpha_search = context.alpha - 2;
    }
    // Do a null window search around alpha. We just want to know
    // if there is an eval that is greater than alpha. If there is, we do a full
    // search.
    if (move_index != 0)
    {
      int beta_search = alpha_search + 1;
      context.eval = -negamax_alpha_beta_search(
          new_context(context.board_state, -beta_search, -alpha_search,
                      search_depth - 1, context.is_forward_pruning_line,
                      move_index == 0, context.ply + 1, context.thread_index,
                      context.king_in_check, context.iteration_depth));
    }
    if (move_index == 0 || context.eval > alpha_search)
    {
      context.eval = -negamax_alpha_beta_search(
          new_context(context.board_state, -context.beta, -alpha_search,
                      search_depth - 1, context.is_forward_pruning_line,
                      move_index == 0, context.ply + 1, context.thread_index,
                      context.king_in_check, context.iteration_depth));
    }

    move_scores.emplace_back(possible_moves[move_index], context.eval);

    context.board_state.undo_move();

    if (context.eval > context.max_eval)
    {
      context.max_eval = context.eval;
      context.tt_best_move_index = possible_moves[move_index].list_index;
    }

    context.max_eval = std::max(context.eval, context.max_eval);
    context.alpha = std::max(context.eval, context.alpha);

    update_history_table(possible_moves[move_index], context.eval, search_depth,
                         move_index, context.alpha, context.beta,
                         history_tables[context.thread_index]);

    if (context.alpha >= context.beta)
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
  handle_eval_adjustments(context.max_eval, context.board_state);

  store_state_in_transposition_table(context);

  return move_scores;
}

auto SearchEngine::negamax_alpha_beta_search(NodeContext context) -> int
{
  // Return if the engine wants to stop searching.
  // If the current state has been repeated three times, the game is drawn so
  // return 0.
  // TODO: Move the draw flag check before calling negamax_alpha_beta_search.
  if (!running_search_flag ||
      context.board_state.current_state_has_been_repeated_three_times())
  {
    return 0;
  }

  if (context.depth < 0)
  {
    context.depth = 0;
    printf("BREAKPOINT negamax_alpha_beta_search; depth < 0\n");
  }

  if (context.thread_index == 0)
  {
    nodes_visited.fetch_add(1, std::memory_order_relaxed);
  }
  nodes_visited_all_threads.fetch_add(1, std::memory_order_relaxed);

  context.hash = context.board_state.get_current_state_hash();
  context.max_eval = -INF;

  if (handle_tt_entry(context))
  {
    return context.tt_eval;
  }

  // If previous color to move is in check, return INF because they are in
  // checkmate.
  if ((context.board_state.color_to_move == PieceColor::WHITE)
          ? attack_check::king_is_checked(context.board_state,
                                          PieceColor::BLACK)
          : attack_check::king_is_checked(context.board_state,
                                          PieceColor::WHITE))
  {
    return INF;
  }

  context.king_in_check =
      (context.board_state.color_to_move == PieceColor::WHITE)
          ? attack_check::king_is_checked(context.board_state,
                                          PieceColor::WHITE)
          : attack_check::king_is_checked(context.board_state,
                                          PieceColor::BLACK);

  // NOTE: Limit extension to 2 plys to avoid stalling the search.
  if (context.depth <= 0 &&
      (context.king_in_check || context.previous_state_in_check) &&
      (context.ply + 2) <= context.iteration_depth)
  {
    ++context.depth;
  }

  // HANDLE LEAF NODE

  // There is a scnario where the depth is less than 0. This can happen if the
  // null move heuristic is used when the depth is 1. Null move calls negamax
  // with depth - 2 since it is skipping a turn.
  if (context.depth <= 0)
  {
    if (context.thread_index == 0)
    {
      leaf_nodes_visited.fetch_add(1, std::memory_order_relaxed);
    }
    return quiescence_search(
        new_context(context.board_state, context.alpha, context.beta, 0,
                    context.is_forward_pruning_line, context.is_pvs_line,
                    context.ply, context.thread_index, context.king_in_check,
                    context.iteration_depth, true));
  }

  context.static_eval =
      position_evaluator::evaluate_position(context.board_state);

  // NULL MOVE PRUNING HEURISTIC

  if (!do_null_move_search(context))
  {
    run_negamax_procedure(context);
  }

  // NOTE: If search has stopped, don't save the states in the transposition
  // table as they are likely invalid.
  if (!running_search_flag)
  {
    return 0;
  }

  // AFTER SEARCH PROCEDURE

  // Handle checkmate evals and correct stalemate evals before saving
  // the state into the transposition table.
  handle_eval_adjustments(context.max_eval, context.board_state);

  store_state_in_transposition_table(context);

  return context.max_eval;
}

void SearchEngine::sort_moves(std::vector<std::pair<Move, int>> &move_scores)
{
  sort(
      move_scores.begin(), move_scores.end(),
      [](const std::pair<Move, int> &move_a, const std::pair<Move, int> &move_b)
      { return move_a.second > move_b.second; });
}

void SearchEngine::run_negamax_procedure(NodeContext &context)
{
  std::vector<Move> possible_moves = move_generator::calculate_possible_moves(
      context.board_state, true, &history_tables[context.thread_index], false);

  put_best_move_at_front(possible_moves, context.tt_best_move_index);

  int quiet_move_index = 0;
  bool is_capture_move = false;
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

    context.board_state.apply_move(possible_moves[move_index]);

    // FUTILITY PRUNING HEURISTIC

    if (!futility_prune_move(context, quiet_move_index,
                             possible_moves[move_index], is_capture_move))
    {
      run_pvs_search(context, move_index, quiet_move_index, is_capture_move);
    }

    context.board_state.undo_move();

    if (context.eval > context.max_eval)
    {
      context.max_eval = context.eval;
      context.tt_best_move_index = possible_moves[move_index].list_index;
    }

    context.max_eval = std::max(context.eval, context.max_eval);
    context.alpha = std::max(context.eval, context.alpha);

    update_history_table(possible_moves[move_index], context.eval,
                         context.depth, move_index, context.alpha, context.beta,
                         history_tables[context.thread_index]);

    if (context.alpha >= context.beta)
    {
      break;
    }
  }
}

void SearchEngine::run_pvs_search(NodeContext &context,
                                  int move_index,
                                  int quiet_move_index,
                                  bool is_capture_move)
{
  int new_search_depth = context.depth - 1;

  // LATE MOVE REDUCTION HEURISTIC

  bool lmr_line = context.is_forward_pruning_line;
  if (quiet_move_index > LMR_THRESHOLD && context.depth >= MIN_LMR_DEPTH &&
      !context.king_in_check && !context.previous_state_in_check &&
      !is_capture_move && !context.is_forward_pruning_line &&
      (context.depth + context.ply) > MIN_LMR_ITERATION_DEPTH &&
      context.board_state.previous_move_stack.top().promotion_piece_type ==
          PieceType::EMPTY)
  {
    lmr_line = true;
    new_search_depth -= LATE_MOVE_REDUCTION;

    if (quiet_move_index > EXTREME_LMR_THRESHOLD)
    {
      // If the move is late enough, we can reduce the search depth even
      // further.
      new_search_depth -=
          (quiet_move_index / LMR_EXTREME_REDUCTION_INDEX_DIVISOR);
    }
    // If position is equal, search quiet moves deeper.
    if (context.board_state.is_end_game ||
        (context.static_eval > -PAWN_VALUE && context.static_eval < PAWN_VALUE))
    {
      new_search_depth += 1;
    }
    if (context.static_eval < context.original_alpha - PAWN_VALUE)
    {
      new_search_depth -= 1;
    }

    // Don't reduce the search depth too far.
    if (new_search_depth < 3)
    {
      new_search_depth = 2;
    }
  }

  // Do a null window search around alpha. We just want to know
  // if there is an eval that is greater than alpha. If there is, we do a full
  // search.
  context.eval = -negamax_alpha_beta_search(new_context(
      context.board_state, -context.alpha - 1, -context.alpha, new_search_depth,
      lmr_line, (move_index == 0 && context.is_pvs_line), context.ply + 1,
      context.thread_index, context.king_in_check, context.iteration_depth));

  if (context.eval > context.alpha && context.depth - 1 > new_search_depth)
  {
    context.eval = -negamax_alpha_beta_search(new_context(
        context.board_state, -context.alpha - 1, -context.alpha,
        context.depth - 1, context.is_forward_pruning_line,
        (move_index == 0 && context.is_pvs_line), context.ply + 1,
        context.thread_index, context.king_in_check, context.iteration_depth));
  }

  // Check if eval is greater than alpha. If it is, do a full search.
  // If window is already null, don't do a redundant search. This means parent
  // nodes are doing a null window search, and we just did a null window
  // search above.
  if (context.eval > context.alpha && context.beta - context.alpha > 1)
  {
    context.eval = -negamax_alpha_beta_search(new_context(
        context.board_state, -context.beta, -context.alpha, context.depth - 1,
        context.is_forward_pruning_line,
        (move_index == 0 && context.is_pvs_line), context.ply + 1,
        context.thread_index, context.king_in_check, context.iteration_depth));
  }
}

auto SearchEngine::handle_tt_entry(NodeContext &context) -> bool
{
  // Check transposition table if position has been searched before.
  if (transposition_table.retrieve(context.hash, context.tt_entry_search_depth,
                                   context.tt_eval, context.tt_flag,
                                   context.tt_best_move_index) &&
      !(context.board_state.is_end_game &&
        context.board_state.current_state_has_been_visited()))
  {
    // Check if tt_value can be used.
    // If the depth of the stored position is greater than or equal to the
    // current depth, then the stored value is reliable. The higher the stored
    // depth, the deeper the node has been searched.
    int tt_alpha = context.alpha;
    int tt_beta = context.beta;
    if (context.depth <= context.tt_entry_search_depth)
    {
      switch (context.tt_flag)
      {
      case EXACT: // alpha < eval < beta
        return true;

      case FAILED_HIGH: // eval >= beta
        tt_alpha = std::max(context.alpha, context.tt_eval);
        break;

      case FAILED_LOW: // eval <= alpha
        tt_beta = std::min(context.beta, context.tt_eval);
        break;

      default:
        // Handle unexpected tt_flag value.
        printf("BREAKPOINT minimax_alpha_beta_search; tt_flag: %d",
               context.tt_flag);
      }

      // Check if we still fail high or low with the current alpha and beta.
      // If flag is FAILED_HIGH, (tt_value >= beta)
      // If flag is FAILED_LOW, (tt_value <= alpha)
      if (tt_alpha > tt_beta)
      {
        return true;
      }
    }
  }

  return false;
}

auto SearchEngine::do_null_move_search(NodeContext &context) -> bool
{
  if (context.is_forward_pruning_line || context.is_pvs_line ||
      (context.depth + context.ply) <= MIN_NULL_MOVE_ITERATION_DEPTH ||
      context.board_state.is_end_game || context.king_in_check ||
      context.depth < MIN_NULL_MOVE_DEPTH ||
      context.static_eval + PAWN_VALUE < context.beta)
  {
    return false;
  }

  context.board_state.apply_null_move();

  int reduction = context.depth / NULL_MOVE_ADDITIONAL_DEPTH_DIVISOR;

  context.eval = -negamax_alpha_beta_search(new_context(
      context.board_state, -context.beta, -(context.beta - 1),
      context.depth - reduction, true, false, context.ply + 1,
      context.thread_index, context.king_in_check, context.iteration_depth));

  context.board_state.undo_null_move();

  if (context.eval >= context.beta)
  {
    context.max_eval = context.eval;
    return true;
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

void SearchEngine::store_state_in_transposition_table(NodeContext &context)
{
  // Store in transposition table.
  int tt_flag_to_store;
  if (context.max_eval >= context.beta)
  {
    tt_flag_to_store = FAILED_HIGH;
  }
  else if (context.max_eval <= context.original_alpha)
  {
    tt_flag_to_store = FAILED_LOW;
  }
  else
  {
    tt_flag_to_store = EXACT;
  }
  transposition_table.store(context.hash, context.depth, context.max_eval,
                            tt_flag_to_store, context.tt_best_move_index,
                            context.is_quiescence);
}

void SearchEngine::reset_and_print_performance_matrix(
    int iterative_depth,
    const std::chrono::time_point<std::chrono::steady_clock> &search_start_time,
    const std::chrono::time_point<std::chrono::steady_clock> &search_end_time,
    const std::vector<std::pair<Move, int>> &move_scores)
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

    auto kilo_nps = static_cast<size_t>(nodes_visited *
                                        NANOSECONDS_IN_MILLISECOND / duration);

    auto kilo_nps_all_threads = static_cast<size_t>(
        nodes_visited_all_threads * NANOSECONDS_IN_MILLISECOND / duration);

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

    // Branchind Factor
    auto branching_factor = std::pow(nodes_visited, 1.0 / iterative_depth);

    printf("Depth: %d, Time: %ldms\n", iterative_depth,
           duration / NANOSECONDS_IN_MILLISECOND);
    printf("Best Move: %s, Eval: %d\n",
           move_interface::move_to_string(move_scores[0].first).c_str(),
           move_scores[0].second);
    printf("Branching Factor: %.2f\n", branching_factor);
    printf("Leaf Nodes Visited: %zu\n", leaf_nodes_visited.load());
    printf("Quiessence Nodes Visited: %zu\n", quiescence_nodes_visited.load());
    printf("Nodes Visited: %zu\n", nodes_visited.load());
    printf("Nodes Visited - All Threads: %zu\n",
           nodes_visited_all_threads.load());
    printf("Normal Node Percentage: %d%%\n", normal_node_percentage);
    printf("Quiescence Node Percentage: %d%%\n", quiescence_node_percentage);
    printf("Nodes per second: %lu kN/s\n", kilo_nps);
    printf("Nodes per second - All Threads: %lu kN/s\n\n",
           kilo_nps_all_threads);
  }

  // Reset performance metrics.
  nodes_visited = 0;
  leaf_nodes_visited = 0;
  quiescence_nodes_visited = 0;
  nodes_visited_all_threads = 0;
}

auto SearchEngine::quiescence_search(NodeContext context) -> int
{
  // Check if the engine wants to stop searching.
  if (!running_search_flag)
  {
    return 0;
  }

  // Check if the current state has been repeated three times. If it has, the
  // game is drawn. Evaluation for a draw is 0.
  if (context.board_state.current_state_has_been_repeated_three_times())
  {
    return 0;
  }

  // Increment nodes visited.
  if (context.thread_index == 0)
  {
    nodes_visited.fetch_add(1, std::memory_order_relaxed);
    quiescence_nodes_visited.fetch_add(1, std::memory_order_relaxed);
  }
  nodes_visited_all_threads.fetch_add(1, std::memory_order_relaxed);

  context.is_quiescence = true;
  context.original_alpha = context.alpha;
  context.hash = context.board_state.get_current_state_hash();

  // CHECKMATE DETECTION

  // CHECK WHICH SIDE IS IN CHECK
  if ((context.board_state.color_to_move == PieceColor::WHITE)
          ? attack_check::king_is_checked(context.board_state,
                                          PieceColor::BLACK)
          : attack_check::king_is_checked(context.board_state,
                                          PieceColor::WHITE))
  {
    return INF;
  }

  context.king_in_check =
      (context.board_state.color_to_move == PieceColor::WHITE)
          ? attack_check::king_is_checked(context.board_state,
                                          PieceColor::WHITE)
          : attack_check::king_is_checked(context.board_state,
                                          PieceColor::BLACK);

  // TRANSPOSITION TABLE LOOKUP

  if (transposition_table.retrieve(context.hash, context.tt_entry_search_depth,
                                   context.tt_eval, context.tt_flag,
                                   context.tt_best_move_index, true))
  {
    int tt_alpha = context.alpha;
    int tt_beta = context.beta;
    switch (context.tt_flag)
    {
    case EXACT: // alpha < eval < beta
      return context.tt_eval;

    case FAILED_HIGH: // eval >= beta
      tt_alpha = std::max(context.alpha, context.tt_eval);
      break;

    case FAILED_LOW: // eval <= alpha
      tt_beta = std::min(context.beta, context.tt_eval);
      break;

    default:
      // Handle unexpected tt_flag value.
      printf("BREAKPOINT minimax_alpha_beta_search; tt_flag: %d",
             context.tt_flag);
    }

    if (tt_alpha > tt_beta)
    {
      return context.tt_eval;
    }
  }

  // QUIESCENCE SEARCH PRE-PROCEDURE

  context.static_eval =
      position_evaluator::evaluate_position(context.board_state);

  // If the eval is not within the alpha beta window, return the eval.
  // Otherwise, we will do too many unnecessary quiescence searches.
  if (context.static_eval >= context.beta)
  {
    return context.static_eval;
  }

  context.alpha = std::max(context.alpha, context.static_eval);

  // PRINCIPAL VARIATION HEURISTIC

  // QUIESCENCE SEARCH

  run_quiescence_search_procedure(context);

  // AFTER SEARCH PROCEDURE

  // If search has stopped, don't save the states in the transposition
  // table. This will cause invalid states to be stored with eval scores of 0.
  // This may be saved as exact values in the transposition table, causing
  // incorrect cutoffs in future searches.
  if (!running_search_flag)
  {
    return 0;
  }

  context.depth = 0;
  store_state_in_transposition_table(context);
  return context.max_eval;
}

void SearchEngine::run_quiescence_search_procedure(NodeContext &context)
{
  std::vector<Move> possible_moves = move_generator::calculate_possible_moves(
      context.board_state, true, &history_tables[context.thread_index],
      !context.king_in_check);
  put_best_move_at_front(possible_moves, context.tt_best_move_index);

  context.max_eval = context.static_eval;
  for (auto &move : possible_moves)
  {
    // Check if the move can be delta pruned.
    if (!context.king_in_check && delta_prune_move(context, move))
    {
      continue;
    }

    context.board_state.apply_move(move);

    int eval = -quiescence_search(
        new_context(context.board_state, -context.beta, -context.alpha, 0,
                    false, false, context.ply, context.thread_index,
                    context.king_in_check, context.iteration_depth, true));

    context.board_state.undo_move();

    if (eval > context.max_eval)
    {
      context.max_eval = eval;
      context.tt_best_move_index = move.list_index;

      if (eval >= context.beta)
      {
        break;
      }

      context.alpha = std::max(eval, context.alpha);
    }
  }
}

auto SearchEngine::delta_prune_move(NodeContext &context,
                                    const Move &move) -> bool
{
  // If the static_eval is so low that the score gained from capturing the
  // piece in the move + 2 pawn values (100) will not bring it back up to
  // alpha, then it is most likely not worth searching the move.
  // Don't do it in the end game since it is more likely that bad moves are
  // the best a player could do.

  return (
      !context.board_state.is_end_game &&
      (context.static_eval + (PAWN_VALUE * 2) +
       PIECE_VALUES[static_cast<uint8_t>(move.captured_piece->piece_type)]) <
          context.alpha);
}

auto SearchEngine::futility_prune_move(NodeContext &context,
                                       int quiet_move_index,
                                       Move &move,
                                       bool is_capture_move) -> bool
{
  if (quiet_move_index < MIN_FP_QUIET_MOVE_INDEX ||
      context.alpha < -INF_MINUS_1000 ||
      move.promotion_piece_type != PieceType::EMPTY || context.king_in_check ||
      attack_check::king_is_checked(context.board_state,
                                    context.board_state.color_to_move) ||
      is_capture_move || context.ply < MIN_FUTILITY_PRUNING_PLY)
  {
    return false;
  }

  int futility_margin = (PAWN_VALUE * context.depth) - (quiet_move_index * 2);

  return context.static_eval + futility_margin < context.alpha;
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
