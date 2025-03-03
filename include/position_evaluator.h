#ifndef POSITION_EVALUATOR_H
#define POSITION_EVALUATOR_H

#include "board_state.h"

namespace engine::parts
{
/**
 * @brief Class to evaluate the current position of the chess board.
 */
class PositionEvaluator
{
public:
  /**
   * @brief Evaluates current position using chess heuristics.
   * @param board_state BoardState object to evaluate.
   * @return Score of the given position.
   */
  static auto evaluate_position(BoardState &board_state) -> int;

private:
  /**
   * @brief Evaluates pawn at given position.
   *
   * @param x_coordinate, y_coordinate Position of the pawn.
   * @param eval Evaluation score.
   */
  static void evaluate_pawn(int x_coordinate, int y_coordinate, Piece &piece,
                            int &eval);

  /**
   * @brief Evaluates knight at given position.
   *
   * @param x_coordinate, y_coordinate Position of the knight.
   * @param eval Evaluation score.
   */
  static void evaluate_knight(int x_coordinate, int y_coordinate, Piece &piece,
                              int &eval);

  /**
   * @brief Evaluates bishop at given position.
   *
   * @param x_coordinate, y_coordinate Position of the bishop.
   * @param eval Evaluation score.
   */
  static void evaluate_bishop(int x_coordinate, int y_coordinate, Piece &piece,
                              int &eval);

  /**
   * @brief Evaluates rook at given position.
   *
   * @param x_coordinate, y_coordinate Position of the rook.
   * @param eval Evaluation score.
   */
  static void evaluate_rook(int x_coordinate, int y_coordinate, Piece &piece,
                            int &eval);

  /**
   * @brief Evaluates queen at given position.
   *
   * @param x_coordinate, y_coordinate Position of the queen.
   * @param eval Evaluation score.
   */
  static void evaluate_queen(int x_coordinate, int y_coordinate, Piece &piece,
                             int &eval);

  /**
   * @brief Evaluates king at given position.
   *
   * @param x_coordinate, y_coordinate Position of the king.
   * @param eval Evaluation score.
   */
  static void evaluate_king(int x_coordinate, int y_coordinate, Piece &piece,
                            int &eval);
};
} // namespace engine::parts

#endif