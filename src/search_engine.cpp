#include "search_engine.h"

#include <chrono>

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
  std::vector<Move> possible_moves =
      move_generator::calculate_possible_moves(game_board_state);
  std::vector<std::pair<Move, int>> move_scores;

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

    auto search_start_time = std::chrono::high_resolution_clock::now();
    // Search each move in a separate thread.
    for (int move_index = 0; move_index < possible_moves.size(); ++move_index)
    {
      thread_board_states[move_index].apply_move(possible_moves[move_index]);
      futures.push_back(promises[move_index].get_future());

      search_threads.emplace_back(
          [this, &promises, move_index, iterative_depth, &thread_board_states]()
          {
            int eval =
                -minimax_alpha_beta_search(thread_board_states[move_index],
                                           -INF, INF, iterative_depth - 1);
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

    auto search_end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
                        search_end_time - search_start_time)
                        .count();

    // Print performance metrics.
    if (show_performance)
    {
      printf("Depth: %d, Time: %lldms\n", iterative_depth, duration);
      printf("Nodes Visited %d\n", nodes_visited.load());
      printf("Leaf Nodes Visited %d\n", leaf_nodes_visited.load());
      printf("TT Size: %d\n", transposition_table.get_size());
      printf("Nodes per second: %d kN/s\n\n",
             static_cast<int>(nodes_visited /
                              (duration / MILLISECONDS_TO_SECONDS) /
                              NODES_TO_KILONODES));
    }

    // Reset performance metrics.
    nodes_visited = 0;
    leaf_nodes_visited = 0;
  }

  transposition_table.clear();
  sort_moves(move_scores);

  // Check if move leaves king in check.
  for (std::pair<Move, int> move_score : move_scores)
  {
    if (!game_board_state.move_leaves_king_in_check(move_score.first))
    {
      game_board_state.apply_move(move_score.first);
      return true;
    }
  }
  // No valid moves found.
  return false;
}

// PRIVATE FUNCTIONS

auto SearchEngine::minimax_alpha_beta_search(BoardState &board_state, int alpha,
                                             int beta, int depth,
                                             bool previous_move_is_null) -> int
{
  nodes_visited.fetch_add(1, std::memory_order_relaxed);

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
    if (depth <= entry_search_depth &&
        ((tt_flag == -1 && tt_value <= alpha) ||
         (tt_flag == 1 && tt_value >= beta) || tt_flag == 0))
    {
      return tt_value;
    }
  }

  // Evaluate leaf nodes.
  if (depth <= 0)
  {
    int eval =
        engine::parts::position_evaluator::evaluate_position(board_state);
    leaf_nodes_visited.fetch_add(1, std::memory_order_relaxed);
    if (board_state.color_to_move == PieceColor::BLACK)
    {
      return -eval;
    }
    return eval;
  }

  // Try a null move.
  if (!previous_move_is_null && (max_iterative_search_depth - depth) >= 3)
  {
    // If previous move is a null move, skip this to prevent double null
    // moves. This will prevent the search from being too shallow.
    board_state.apply_null_move();
    int eval =
        -minimax_alpha_beta_search(board_state, -beta, -alpha, depth - 3, true);
    board_state.undo_null_move();
    if (eval >= beta)
    {
      return eval;
    }
  }

  std::vector<Move> possible_moves =
      move_generator::calculate_possible_moves(board_state);
  int eval;
  // Start minimax search.
  int max_eval = -INF;
  int best_move_index = 0;
  if (entry_best_move >= 0 && entry_best_move < possible_moves.size())
  {
    max_search(board_state, alpha, beta, max_eval, eval, depth, best_move_index,
               entry_best_move, possible_moves);
  }
  for (int index = 0; index < possible_moves.size(); ++index)
  {
    max_search(board_state, alpha, beta, max_eval, eval, depth, best_move_index,
               index, possible_moves);
    if (alpha >= beta)
    {
      break;
    }
  }

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
                              std::vector<Move> &possible_moves)
{
  board_state.apply_move(possible_moves[move_index]);
  eval = -minimax_alpha_beta_search(board_state, -beta, -alpha, depth - 1);
  if (eval > max_eval)
  {
    max_eval = eval;
    best_move_index = move_index;
  }
  max_eval = std::max(max_eval, eval);
  alpha = std::max(eval, alpha);
  board_state.undo_move();
}
} // namespace engine::parts