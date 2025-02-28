#ifndef POSITION_EVALUATOR_H
#define POSITION_EVALUATOR_H

#include "board_state.h"

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
  auto evaluate_position(BoardState &board_state) -> int;

private:
  // Directions for the queen.
  const std::array<std::array<int, 2>, 8> queen_directions = {
      {{1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1}}};

  // Directions for the bishop.
  const std::array<std::array<int, 2>, 4> bishop_directions = {
      {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}}};

  // Directions for the knight.
  const std::array<std::array<int, 2>, 8> knight_moves = {
      {{1, 2}, {1, -2}, {-1, 2}, {-1, -2}, {2, 1}, {2, -1}, {-2, 1}, {-2, -1}}};

  // Directions for the rook.
  const std::array<std::array<int, 2>, 4> rook_directions = {
      {{1, 0}, {-1, 0}, {0, 1}, {0, -1}}};

  /**
   * @brief Evaluates pawn at given position.
   *
   * @param x_coordinate, y_coordinate Position of the pawn.
   * @param eval Evaluation score.
   */
  void evaluate_pawn(int x_coordinate, int y_coordinate, Piece &piece,
                     int &eval);

  /**
   * @brief Evaluates knight at given position.
   *
   * @param x_coordinate, y_coordinate Position of the knight.
   * @param eval Evaluation score.
   */
  void evaluate_knight(int x_coordinate, int y_coordinate, Piece &piece,
                       int &eval);

  /**
   * @brief Evaluates bishop at given position.
   *
   * @param x_coordinate, y_coordinate Position of the bishop.
   * @param eval Evaluation score.
   */
  void evaluate_bishop(int x_coordinate, int y_coordinate, Piece &piece,
                       int &eval);

  /**
   * @brief Evaluates rook at given position.
   *
   * @param x_coordinate, y_coordinate Position of the rook.
   * @param eval Evaluation score.
   */
  void evaluate_rook(int x_coordinate, int y_coordinate, Piece &piece,
                     int &eval);

  /**
   * @brief Evaluates queen at given position.
   *
   * @param x_coordinate, y_coordinate Position of the queen.
   * @param eval Evaluation score.
   */
  void evaluate_queen(int x_coordinate, int y_coordinate, Piece &piece,
                      int &eval);

  /**
   * @brief Evaluates king at given position.
   *
   * @param x_coordinate, y_coordinate Position of the king.
   * @param eval Evaluation score.
   */
  void evaluate_king(int x_coordinate, int y_coordinate, Piece &piece,
                     int &eval);
};

#endif