#ifndef POSITION_EVALUATOR_H
#define POSITION_EVALUATOR_H

#include "board_state.h"

class PositionEvaluator {
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

  // See BoardState.
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
};

#endif