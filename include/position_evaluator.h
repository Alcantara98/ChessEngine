#ifndef POSITION_EVALUATOR_H
#define POSITION_EVALUATOR_H

#include "board_state.h"

class PositionEvaluator {
private:
  BoardState &board_state;
  /**
   * @brief Evaluates pawn at given position.
   * @param x, y Position of the pawn.
   * @param eval Evaluation score.
   */
  void evaluate_pawn(int x, int y, Piece &piece, int &eval);

  /**
   * @brief Evaluates knight at given position.
   * @param x, y Position of the knight.
   * @param eval Evaluation score.
   */
  void evaluate_knight(int x, int y, Piece &piece, int &eval);

  /**
   * @brief Evaluates bishop at given position.
   * @param x, y Position of the bishop.
   * @param eval Evaluation score.
   */
  void evaluate_bishop(int x, int y, Piece &piece, int &eval);

  /**
   * @brief Evaluates rook at given position.
   * @param x, y Position of the rook.
   * @param eval Evaluation score.
   */
  void evaluate_rook(int x, int y, Piece &piece, int &eval);

  /**
   * @brief Evaluates queen at given position.
   * @param x, y Position of the queen.
   * @param eval Evaluation score.
   */
  void evaluate_queen(int x, int y, Piece &piece, int &eval);

  /**
   * @brief Evaluates king at given position.
   * @param x, y Position of the king.
   * @param eval Evaluation score.
   */
  void evaluate_king(int x, int y, Piece &piece, int &eval);

public:
  /**
   * @brief Default Constructor - takes a chess board state.
   * @param board_state BoardState object.
   */
  PositionEvaluator(BoardState &board_state);

  /**
   * @brief Evaluates current position using chess heuristics.
   * @return Score of the given position.
   */
  int evaluate_position();
};

#endif