#ifndef BEST_MOVE_FINDER_H
#define BEST_MOVE_FINDER_H

#include "board_state.h"
#include "move_generator.h"

#include <limits>

class BestMoveFinder {
private:
  // Use for starting values of alpha and beta;
  const int INF = std::numeric_limits<int>::max();

  // See BoardState.
  BoardState &board_state;

  /**
   * @brief Recursive function to find the best move using minimax algorithm
   * with alpha beta pruning.
   * @param move Move to explore.
   * @param alpha Highest score to be picked by maximizing node.
   * @param beta Lowest score to be picked by minimizing node.
   * @param depth Current depth of search.
   * @param maximise Determines if the current turn will try to maximise
   * score.
   */
  void minimax_alpha_beta_search(Move move, int alpha, int beta, int depth,
                                 bool maximise);

public:
  // Store possible moves.
  std::vector<Move> possible_moves;

  /**
   * @brief Default Constructor - takes a chess board state.
   * @param board_state BoardState object.
   */
  BestMoveFinder(BoardState &board_state);

  /**
   * @brief Finds the best move with the given current state of the board.
   * @param max_search_depth Max depth to search.
   */
  Move find_best_move(int max_search_depth);

  /**
   * @brief Calculates all possible moves of current board state.
   */
  void calculate_possible_moves();
};

#endif