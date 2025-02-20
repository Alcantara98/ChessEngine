#ifndef MOVE_GENERATOR_H
#define MOVE_GENERATOR_H

#include "board_state.h"
#include "move.h"
#include "piece.h"

#include <array>
#include <vector>

/**
 * @brief Class to generate all possible moves for a given piece given the
 * current board state.
 */
class MoveGenerator {
private:
  /**
   * @brief Generates bishop and rook moves in one given direction.
   *
   * @param board_state Reference of the current board state.
   * @param x, y The coordinate of the bishop or bishop.
   * @param possible_moves Reference to the list of possible moves of current
   * board_state.
   */
  static void rook_bishop_move_helper(BoardState &board_state, int x, int y,
                                      int x_direction, int y_direction,
                                      std::vector<Move> &possible_moves);

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
  static void generate_pawn_move(BoardState &board_state, int x, int y,
                                 std::vector<Move> &possible_moves);

  /**
   * @brief Generates all possible moves for a given king.
   *
   * @param board_state Reference of the current board state.
   * @param x, y The coordinate of the king.
   * @param possible_moves Reference to the list of possible moves of current
   * board_state.
   */
  static void generate_king_move(BoardState &board_state, int x, int y,
                                 std::vector<Move> &possible_moves);

  /**
   * @brief Generates all possible moves for a given knight.
   *
   * @param board_state Reference of the current board state.
   * @param x, y The coordinate of the knight.
   * @param possible_moves Reference to the list of possible moves of current
   * board_state.
   */
  static void generate_knight_move(BoardState &board_state, int x, int y,
                                   std::vector<Move> &possible_moves);

  /**
   * @brief Generates all possible moves for a given bishop.
   *
   * @param board_state Reference of the current board state.
   * @param x, y The coordinate of the bishop.
   * @param possible_moves Reference to the list of possible moves of current
   * board_state.
   */
  static void generate_bishop_move(BoardState &board_state, int x, int y,
                                   std::vector<Move> &possible_moves);

  /**
   * @brief Generates all possible moves for a given rook.
   *
   * @param board_state Reference of the current board state.
   * @param x, y The coordinate of the rook.
   * @param possible_moves Reference to the list of possible moves of current
   * board_state.
   */
  static void generate_rook_move(BoardState &board_state, int x, int y,
                                 std::vector<Move> &possible_moves);

  /**
   * @brief Generates all possible moves for a given queen.
   *
   * @param board_state Reference of the current board state.
   * @param x, y The coordinate of the queen.
   * @param possible_moves Reference to the list of possible moves of current
   * board_state.
   */
  static void generate_queen_move(BoardState &board_state, int x, int y,
                                  std::vector<Move> &possible_moves);
};

#endif // MOVE_GENERATOR_H