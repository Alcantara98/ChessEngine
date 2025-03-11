#ifndef SEARCH_ENGINE_H
#define SEARCH_ENGINE_H

#include "board_state.h"
#include "move_generator.h"
#include "move_interface.h"
#include "position_evaluator.h"
#include "transposition_table.h"

#include <algorithm>
#include <atomic>
#include <future>
#include <limits>
#include <thread>

namespace engine::parts
{
/**
 * @brief Class to find the best move for the current board state.
 */
class SearchEngine
{

public:
  // PROPERTIES
  // Determines which color to maximise for.
  PieceColor engine_color = PieceColor::WHITE;

  // Max depth to search.
  int max_search_depth;

  // Show performance matrix of the search.
  bool show_performance;

  // CONSTRUCTORS
  /**
   * @brief Default Constructor - takes a chess board state.
   *
   * @param board_state BoardState object.
   */
  SearchEngine(BoardState &board_state);

  // FUNCTIONS
  /**
   * @brief Finds the best move for the engine and applies it to the board.
   *
   * @param max_search_depth Max depth to search.
   * @param show_performance Show performance matrix of the search.
   *
   * @return True if a move is found where the king is not checked.
   */
  auto execute_best_move() -> bool;

private:
  // PROPERTIES

  // Use for starting values of alpha and beta;
  const int INF = std::numeric_limits<int>::max();

  // Number of leaf nodes visited.
  std::atomic<int> leaf_nodes_visited = 0;

  // Number of nodes visited.
  std::atomic<int> nodes_visited = 0;

  // See BoardState.
  BoardState &game_board_state;

  // Transposition Table object.
  TranspositionTable transposition_table;

  // The max depth the current iterative search will reach.
  int max_iterative_search_depth;

  // FUNCTIONS

  /**
   * @brief Evaluates all possible moves and scores them.
   *
   * @param move_scores Vector of moves and their scores.
   */
  void evaluate_possible_moves(std::vector<std::pair<Move, int>> &move_scores);

  /**
   * @brief Recursive function to find the best move using minimax algorithm
   * with alpha beta pruning.
   *
   * @param board_state BoardState object to search.
   * @param alpha Highest score to be picked by maximizing node.
   * @param beta Lowest score to be picked by minimizing node.
   * @param depth Current depth of search.
   * @param maximise Determines if the current turn will try to maximise
   * score.
   *
   * @return Evaluation score from search branch.
   */
  auto minimax_alpha_beta_search(BoardState &board_state, int alpha, int beta,
                                 int depth,
                                 bool previous_move_is_null = false) -> int;

  /**
   * @brief Sorts the moves based on their scores.
   *
   * @param move_scores Vector of moves and their scores.
   */
  static void sort_moves(std::vector<std::pair<Move, int>> &move_scores);

  /**
   * @brief Max search procedure for each possible move.
   *
   * @param board_state BoardState object to search.
   * @param alpha Highest score to be picked by maximizing node.
   * @param beta Lowest score to be picked by minimizing node.
   * @param max_eval Current maximum evaluation score.
   * @param eval Evaluation score from search branch.
   * @param depth Current depth of search.
   * @param best_move_index Index of best move.
   * @param move_index Index of current move.
   * @param possible_moves Vector of possible moves.
   */
  void max_search(BoardState &board_state, int &alpha, int &beta, int &max_eval,
                  int &eval, int &depth, int &best_move_index, int &move_index,
                  std::vector<Move> &possible_moves);

  /**
   * @brief Min search procedure for each possible move.
   *
   * @param board_state BoardState object to search.
   * @param alpha Highest score to be picked by maximizing node.
   * @param beta Lowest score to be picked by minimizing node.
   * @param depth Current depth of search.
   * @param eval Evaluation score to be updated.
   */
  void do_null_move_search(BoardState &board_state, int &alpha, int &beta,
                           int &depth, int &eval);

  /**
   * @brief Stores the state in the transposition table.
   *
   * @param hash Hash of the board state.
   * @param depth Depth of the search.
   * @param max_eval Maximum evaluation score.
   * @param alpha Highest score to be picked by maximizing node.
   * @param beta Lowest score to be picked by minimizing node.
   * @param best_move_index Index of best move.
   */
  void store_state_in_transposition_table(uint64_t &hash, int &depth,
                                          int &max_eval, int &alpha, int &beta,
                                          int &best_move_index);

  /**
   * @brief Resets and prints the performance matrix.
   *
   * @param iterative_depth Current iterative depth.
   * @param search_start_time Start time of search.
   * @param search_end_time End time of search.
   */
  void reset_and_print_performance_matrix(
      int iterative_depth,
      std::chrono::system_clock::time_point search_start_time,
      std::chrono::system_clock::time_point search_end_time);
};
} // namespace engine::parts

#endif // SEARCH_ENGINE_H