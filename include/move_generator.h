#ifndef MOVE_GENERATOR_H
#define MOVE_GENERATOR_H

#include "board_state.h"
#include "move.h"
#include "piece.h"

#include <array>
#include <vector>

class MoveGenerator {
public:
  /**
   * @brief Generates all possible moves for a given pawn.
   *
   * @param board_state Reference of the current board state.
   * @param x, y The coordinate of the pawn.
   * @param possible_moves Reference to the list of possible moves of current
   * board_state.
   * @return A vector of possible moves.
   */
  static void generatePawnMove(BoardState &board_state, int x, int y,
                               std::vector<Move> &possible_moves);

  /**
   * @brief Generates all possible moves for a given knight.
   *
   * @param board_state Reference of the current board state.
   * @param x, y The coordinate of the knight.
   * @param possible_moves Reference to the list of possible moves of current
   * board_state.
   */
  static void generateKnightMove(BoardState &board_state, int x, int y,
                                 std::vector<Move> &possible_moves);

  /**
   * @brief Generates all possible moves for a given bishop.
   *
   * @param board_state Reference of the current board state.
   * @param x, y The coordinate of the bishop.
   * @param possible_moves Reference to the list of possible moves of current
   * board_state.
   */
  static void generateBishopMove(BoardState &board_state, int x, int y,
                                 std::vector<Move> &possible_moves);

  /**
   * @brief Generates all possible moves for a given rook.
   *
   * @param board_state Reference of the current board state.
   * @param x, y The coordinate of the rook.
   * @param possible_moves Reference to the list of possible moves of current
   * board_state.
   */
  static void generateRookMove(BoardState &board_state, int x, int y,
                               std::vector<Move> &possible_moves);

  /**
   * @brief Generates all possible moves for a given queen.
   *
   * @param board_state Reference of the current board state.
   * @param x, y The coordinate of the queen.
   * @param possible_moves Reference to the list of possible moves of current
   * board_state.
   */
  static void generateQueenMove(BoardState &board_state, int x, int y,
                                std::vector<Move> &possible_moves);

  /**
   * @brief Generates all possible moves for a given king.
   *
   * @param board_state Reference of the current board state.
   * @param x, y The coordinate of the king.
   * @param possible_moves Reference to the list of possible moves of current
   * board_state.
   */
  static void generateKingMove(BoardState &board_state, int x, int y,
                               std::vector<Move> &possible_moves);

  /**
   * @brief Checks if the given square is attacked.
   * @param board_state Reference of the current board state.
   * @param x, y The coordinate of the square.
   * @param color The color of the attacking pieces.
   * @return True if the square is attacked, false otherwise.
   */
  static bool square_is_attacked(BoardState &board_state, int x, int y,
                                 PieceColor color);
};

#endif // MOVE_GENERATOR_H