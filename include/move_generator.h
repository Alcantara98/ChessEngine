#ifndef MOVE_GENERATOR_H
#define MOVE_GENERATOR_H

#include "board_state.h"
#include "move.h"
#include "piece.h"

#include <array>
#include <vector>

namespace engine::parts
{
/**
 * @brief Class to generate all possible moves for a given piece given the
 * current board state.
 */
class MoveGenerator
{
public:
  /**
   * @brief Generates all possible moves for a given pawn.
   *
   * @note Generated moves are pushed back into the possible_moves vector.
   *
   * @param board_state Reference of the current board state.
   * @param x_coordinate, y_coordinate The coordinate of the pawn.
   * @param possible_moves Reference to the list of possible moves of current
   * board_state.
   */
  static void generate_pawn_move(BoardState &board_state, int x_coordinate,
                                 int y_coordinate,
                                 std::vector<Move> &possible_moves);

  /**
   * @brief Generates all possible moves for a given king.
   *
   * @note Generated moves are pushed back into the possible_moves vector.
   *
   * @param board_state Reference of the current board state.
   * @param x_coordinate, y_coordinate The coordinate of the king.
   * @param possible_moves Reference to the list of possible moves of current
   * board_state.
   */
  static void generate_king_move(BoardState &board_state, int x_coordinate,
                                 int y_coordinate,
                                 std::vector<Move> &possible_moves);

  /**
   * @brief Generates all possible moves for a given knight.
   *
   * @note Generated moves are pushed back into the possible_moves vector.
   *
   * @param board_state Reference of the current board state.
   * @param x_coordinate, y_coordinate The coordinate of the knight.
   * @param possible_moves Reference to the list of possible moves of current
   * board_state.
   */
  static void generate_knight_move(BoardState &board_state, int x_coordinate,
                                   int y_coordinate,
                                   std::vector<Move> &possible_moves);

  /**
   * @brief Generates all possible moves for a given bishop.
   *
   * @note Generated moves are pushed back into the possible_moves vector.
   *
   * @param board_state Reference of the current board state.
   * @param x_coordinate, y_coordinate The coordinate of the bishop.
   * @param possible_moves Reference to the list of possible moves of current
   * board_state.
   */
  static void generate_bishop_move(BoardState &board_state, int x_coordinate,
                                   int y_coordinate,
                                   std::vector<Move> &possible_moves);

  /**
   * @brief Generates all possible moves for a given rook.
   *
   * @note Generated moves are pushed back into the possible_moves vector.
   *
   * @param board_state Reference of the current board state.
   * @param x_coordinate, y_coordinate The coordinate of the rook.
   * @param possible_moves Reference to the list of possible moves of current
   * board_state.
   */
  static void generate_rook_move(BoardState &board_state, int x_coordinate,
                                 int y_coordinate,
                                 std::vector<Move> &possible_moves);

  /**
   * @brief Generates all possible moves for a given queen.
   *
   * @note Generated moves are pushed back into the possible_moves vector.
   *
   * @param board_state Reference of the current board state.
   * @param x_coordinate, y_coordinate The coordinate of the queen.
   * @param possible_moves Reference to the list of possible moves of current
   * board_state.
   */
  static void generate_queen_move(BoardState &board_state, int x_coordinate,
                                  int y_coordinate,
                                  std::vector<Move> &possible_moves);

private:
  /**
   * @brief Generates bishop and rook moves in one given direction.
   *
   * @param board_state Reference of the current board state.
   * @param x_coordinate, y_coordinate The coordinate of the bishop or bishop.
   * @param possible_moves Reference to the list of possible moves of current
   * board_state.
   */
  static void rook_bishop_move_helper(BoardState &board_state, int x_coordinate,
                                      int y_coordinate, int x_direction,
                                      int y_direction,
                                      std::vector<Move> &possible_moves);
};
} // namespace engine::parts

#endif // MOVE_GENERATOR_H