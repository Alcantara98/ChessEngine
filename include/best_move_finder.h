#ifndef BEST_MOVE_FINDER_H
#define BEST_MOVE_FINDER_H

#include "move_generator.h"

class BestMoveFinder {
public:
  std::vector<Move> possible_moves;
  BoardState current_board_state;

  /**
   * @brief Default Constructor - takes a chess board state.
   * @param chess_board BoardState object.
   */
  BestMoveFinder(BoardState chess_board);

  /**
   * @brief Finds the best move with the given current state of the board.
   */
  Move find_best_move();

  /**
   * @brief Evaluates current position using chess heuristics.
   * @return Score of the given position.
   */
  int evaluate_position();
  /**
   * @brief Recursive function to find the best move using minimax algorithm
   * with alpha beta pruning.
   * @param move Move to explore.
   * @param alpha Highest score to be picked by maximizing node.
   * @param beta Lowest score to be picked by minimizing node.
   * @param depth Current depth of search.
   * @param maximise Determines if the current turn will try to maximise score.
   */
  void minimax_alpha_beta_search(Move move, int alpha, int beta, int depth,
                                 bool maximise);

  /**
   * @brief Calculates all possible moves of current board state.
   */
  void calculate_possible_moves();

  /**
   * @brief Will apply the given move unto current_board_state;
   * @param move Move to apply on current_board_state.
   */
  void apply_move(Move move);

  /**
   * @brief Will undo the given move unto current_board_state;
   * @param move Move to undo on current_board_state.
   */
  void undo_move(Move move);
};

#endif