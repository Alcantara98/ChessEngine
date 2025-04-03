#ifndef MOVE_H
#define MOVE_H

#include "piece.h"

namespace engine::parts
{
/**
 * @brief A structure to represent a move in chess.
 */
struct Move
{
  // PROPERTIES

  /// @brief The piece that is moving.
  Piece *moving_piece;

  /// @brief The piece that is captured by moving_piece.
  /// @note If no piece is captured, this will be nullptr.
  Piece *captured_piece;

  /// @brief If a pawn is promoted, it will be promoted to this piece type.
  PieceType promotion_piece_type;

  /// @brief Coordinates of moving_piece before applying this move.
  int from_x, from_y;

  /// @brief Coordinates of moving_piece after applying this move.
  int to_x, to_y;

  /// @brief True if moving_piece (must be a pawn) captures En Passant.
  bool capture_is_en_passant;

  /// @brief Specifies whether this is the moving_piece's first move.
  bool first_move_of_moving_piece;

  /// @brief Specifies whether a pawn moved two squares forward on this move.
  bool pawn_moved_two_squares;

  /// @brief If a pawn moved two squares, these are the coordinates the pawn
  /// moved to.
  int pawn_moved_two_squares_to_x, pawn_moved_two_squares_to_y;

  /// @brief Index of the move in the move list.
  /// @note This is used to identify the best move in the possible moves list.
  int list_index = 0;

  // CONSTRUCTORS

  /**
   * @brief Constructor with all parameters.
   *
   * @param from_x, from_y Coordinates of moving_piece before applying this move.
   * @param to_x, to_y Coordinates of moving_piece after applying this move.
   * @param moving_piece The piece that is moving.
   * @param captured_piece The piece that is captured by moving_piece.
   * @param promotion_piece_type If a pawn is promoted, it will be promoted to
   * this piece type.
   * @param capture_is_en_passant True if moving_piece (must be a pawn)
   * captures En Passant.
   * @param first_move_of_moving_piece True if this is the moving_piece's first
   * move.
   * @param pawn_moved_two_squares Specifies whether a pawn moved two squares
   * forward on this move.
   * @param pawn_moved_two_squares_to_x, pawn_moved_two_squares_to_y If a pawn
   * moved two squares, these are the coordinates the pawn moved to.
   */
  Move(int from_x,
       int from_y,
       int to_x,
       int to_y,
       Piece *moving_piece,
       Piece *captured_piece,
       PieceType promotion_piece_type,
       bool capture_is_en_passant,
       bool first_move_of_moving_piece,
       bool pawn_moved_two_squares,
       int pawn_moved_two_squares_to_x,
       int pawn_moved_two_squares_to_y);

  /**
   * @brief Constructor for normal moves without captures.
   *
   * @param from_x, from_y Coordinates of moving_piece before applying this move.
   * @param to_x, to_y Coordinates of moving_piece after applying this move.
   * @param moving_piece The piece that is moving.
   * @param first_move_of_moving_piece True if this is the moving_piece's first
   * move.
   * @param pawn_moved_two_squares Specifies whether a pawn moved two squares
   * forward on this move.
   * @param pawn_moved_two_squares_to_x, pawn_moved_two_squares_to_y If a pawn
   * moved two squares, these are the coordinates the pawn moved to.
   */
  Move(int from_x,
       int from_y,
       int to_x,
       int to_y,
       Piece *moving_piece,
       bool first_move_of_moving_piece = false,
       bool pawn_moved_two_squares = false,
       int pawn_moved_two_squares_to_x = -1,
       int pawn_moved_two_squares_to_y = -1);

  /**
   * @brief Constructor for captures.
   *
   * @param from_x, from_y Coordinates of moving_piece before applying this move.
   * @param to_x, to_y Coordinates of moving_piece after applying this move.
   * @param moving_piece The piece that is moving.
   * @param captured_piece The piece that is captured by moving_piece.
   * @param first_move_of_moving_piece True if this is the moving_piece's first
   * move.
   * @param capture_is_en_passant True if moving_piece (must be a pawn)
   * captures En Passant.
   */
  Move(int from_x,
       int from_y,
       int to_x,
       int to_y,
       Piece *moving_piece,
       Piece *captured_piece,
       bool first_move_of_moving_piece = false,
       bool capture_is_en_passant = false);

  /**
   * @brief Constructor for pawn promotion through normal moves.
   *
   * @param from_x, from_y Coordinates of moving_piece before applying this move.
   * @param to_x, to_y Coordinates of moving_piece after applying this move.
   * @param moving_piece The piece that is moving.
   * @param promotion_piece_type If a pawn is promoted, it will be promoted to
   * this piece type.
   */
  Move(int from_x,
       int from_y,
       int to_x,
       int to_y,
       Piece *moving_piece,
       PieceType promotion_piece_type);

  /**
   * @brief Constructor for pawn promotion through captures.
   *
   * @param from_x, from_y Coordinates of moving_piece before applying this move.
   * @param to_x, to_y Coordinates of moving_piece after applying this move.
   * @param moving_piece The piece that is moving.
   * @param captured_piece The piece that is captured by moving_piece.
   * @param promotion_piece_type If a pawn is promoted, it will be promoted to
   * this piece type.
   */
  Move(int from_x,
       int from_y,
       int to_x,
       int to_y,
       Piece *moving_piece,
       Piece *captured_piece,
       PieceType promotion_piece_type);

  // FUNCTIONS

  /**
   * @brief Overloaded operator to compare two moves.
   *
   * @param other The other move to compare with.
   *
   * @return True if the two moves are the same.
   */
  auto operator==(const Move &other) const -> bool;
};
} // namespace engine::parts

#endif
