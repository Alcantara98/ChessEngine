#ifndef BEST_MOVE_FINDER_H
#define BEST_MOVE_FINDER_H

#include "board_state.h"
#include "move_generator.h"
#include "position_evaluator.h"
#include "transposition_table.h"

#include <algorithm>
#include <limits>

class BestMoveFinder {

public:
  // Determines which color to maximise for.
  PieceColor engine_color = PieceColor::WHITE;

  /**
   * @brief Default Constructor - takes a chess board state.
   * @param board_state BoardState object.
   */
  BestMoveFinder(BoardState &board_state);

  /**
   * @brief Calculates all possible moves of current board state.
   */
  std::vector<Move> calculate_possible_moves();

  /**
   * @brief Finds the best move with the given current state of the board.
   * @param max_search_depth Max depth to search.
   */
  Move find_best_move(int max_search_depth, bool show_performance);

private:
  // Use for starting values of alpha and beta;
  const int INF = std::numeric_limits<int>::max();

  // Max depth to search at current iteration.
  int iterative_depth_search = 0;

  // Number of leaf nodes visited.
  int leaf_nodes_visited = 0;

  // Number of nodes visited.
  int nodes_visited = 0;

  // See BoardState.
  BoardState &board_state;

  // Position Evaluator object.
  PositionEvaluator position_evaluator;

  // Transposition Table object.
  TranspositionTable transposition_table;

  /**
   * @brief Recursive function to find the best move using minimax algorithm
   * with alpha beta pruning.
   * @param move Move to explore.
   * @param alpha Highest score to be picked by maximizing node.
   * @param beta Lowest score to be picked by minimizing node.
   * @param depth Current depth of search.
   * @param maximise Determines if the current turn will try to maximise
   * score.
   * @return Evaluation score from search branch.
   */
  int minimax_alpha_beta_search(int alpha, int beta, int depth, bool maximise);

  /**
   * @brief Sorts the moves based on their scores.
   * @param move_scores Vector of moves and their scores.
   */
  void sort_moves(std::vector<std::pair<Move, int>> &move_scores);

  /**
   * @brief Max search procedure for each possible move.
   * @param alpha Highest score to be picked by maximizing node.
   * @param beta Lowest score to be picked by minimizing node.
   * @param max_eval Current maximum evaluation score.
   * @param eval Evaluation score from search branch.
   * @param depth Current depth of search.
   * @param best_move_index Index of best move.
   * @param move_index Index of current move.
   * @param possible_moves Vector of possible moves.
   */
  void max_search(int &alpha, int &beta, int &max_eval, int &eval, int &depth,
                  int &best_move_index, int &move_index,
                  std::vector<Move> &possible_moves);

  /**
   * @brief Min search procedure for each possible move.
   * @param alpha Highest score to be picked by maximizing node.
   * @param beta Lowest score to be picked by minimizing node.
   * @param min_eval Current minimum evaluation score.
   * @param eval Evaluation score from search branch.
   * @param depth Current depth of search.
   * @param best_move_index Index of best move.
   * @param move_index Index of current move.
   * @param possible_moves Vector of possible moves.
   */
  void min_search(int &alpha, int &beta, int &min_eval, int &eval, int &depth,
                  int &best_move_index, int &move_index,
                  std::vector<Move> &possible_moves);
};

#endif