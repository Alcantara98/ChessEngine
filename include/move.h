#ifndef MOVE_H
#define MOVE_H

#include "piece.h"

/**
 * @brief Class to represent a move in chess.
 */
struct Move
{
  // The piece that is moving.
  Piece *moving_piece;

  // The piece that is captured by moving_piece.
  Piece *captured_piece;

  // If pawn is promoted, it will be promoted to this piece type.
  PieceType promotion_piece_type;

  // moving_piece coordinates before applying this move.
  int from_x, from_y;

  // moving_piece coordinates after applying this move.
  int to_x, to_y;

  // True if moveing_piece (has to be a pawn) captures En Passant.
  bool capture_is_en_passant;

  // Specifies whether this is the moving_piece's first move.
  bool first_move_of_moving_piece;

  // Specifies whether a pawn moved two squares forward on this move.
  bool pawn_moved_two_squares;

  // If a pawn moved two squares, these are the coordinates the pawn moved to.
  int pawn_moved_two_squares_to_x, pawn_moved_two_squares_to_y;

  /**
   * @brief Constructor with all parameters.
   *
   * @param from_x, from_y moving_piece coordinates before applying this move.
   * @param to_x, to_y moving_piece coordinates after applying this move.
   * @param moving_piece The piece that is moving.
   * @param captured_piece The piece that is captured by moving_piece.
   * @param promotion_piece_type If pawn is promoted, it will be promoted to
   * this piece type.
   * @param capture_is_en_passant True if moveing_piece (has to be a pawn)
   * captures En Passant.
   * @param first_move_of_moving_piece True if this is the moving_piece's first
   * move.
   * @param pawn_moved_two_squares Specifies whether a pawn moved two squares
   * forward on this move.
   * @param pawn_moved_two_squares_to_x, pawn_moved_two_squares_to_y If a pawn
   * moved two squares, these are the coordinates the pawn moved to.
   */
  Move(int from_x, int from_y, int to_x, int to_y, Piece *moving_piece,
       Piece *captured_piece, PieceType promotion_piece_type,
       bool capture_is_en_passant, bool moving_piece_first_move,
       bool pawn_moved_two, int pawn_moved_two_squares_to_x,
       int pawn_moved_two_squares_to_y);

  /**
   * @brief Constructor for normal move without captures.
   *
   * @param from_x, from_y moving_piece coordinates before applying this move.
   * @param to_x, to_y moving_piece coordinates after applying this move.
   * @param moving_piece The piece that is moving.
   * @param first_move_of_moving_piece True if this is the moving_piece's first
   * @param pawn_moved_two_squares Specifies whether a pawn moved two squares
   * forward on this move.
   * @param pawn_moved_two_squares_to_x, pawn_moved_two_squares_to_y If a pawn
   * moved two squares, these are the coordinates the pawn moved to.
   */
  Move(int from_x, int from_y, int to_x, int to_y, Piece *moving_piece,
       bool first_move = false, bool pawn_moved_two = false,
       int pawn_moved_two_squares_to_x = -1,
       int pawn_moved_two_squares_to_y = -1);

  /**
   * @brief Constructor for captures.
   *
   * @param from_x, from_y moving_piece coordinates before applying this move.
   * @param to_x, to_y moving_piece coordinates after applying this move.
   * @param moving_piece The piece that is moving.
   * @param captured_piece The piece that is captured by moving_piece.
   * @param capture_is_en_passant True if moveing_piece (has to be a pawn)
   * captures En Passant.
   * @param first_move_of_moving_piece True if this is the moving_piece's first
   */
  Move(int from_x, int from_y, int to_x, int to_y, Piece *moving_piece,
       Piece *captured_piece, bool first_move_of_moving_piece = false,
       bool capture_is_en_passant = false);

  /**
   * @brief Constructor for pawn promotion through normal move.
   *
   * @param from_x, from_y moving_piece coordinates before applying this move.
   * @param to_x, to_y moving_piece coordinates after applying this move.
   * @param moving_piece The piece that is moving.
   * @param promotion_piece_type If pawn is promoted, it will be promoted to
   * this piece type.
   */
  Move(int from_x, int from_y, int to_x, int to_y, Piece *moving_piece,
       PieceType promotion_piece_type);

  /**
   * @brief Constructor for pawn promotion through captures.
   *
   * @param from_x, from_y moving_piece coordinates before applying this move.
   * @param to_x, to_y moving_piece coordinates after applying this move.
   * @param moving_piece The piece that is moving.
   * @param captured_piece The piece that is captured by moving_piece.
   * @param promotion_piece_type If pawn is promoted, it will be promoted to
   * this piece type.
   */
  Move(int from_x, int from_y, int to_x, int to_y, Piece *moving_piece,
       Piece *captured_piece, PieceType promotion_piece_type);

  /**
   * @brief Overloaded operator to compare two moves.
   *
   * @param other The other move to compare with.
   *
   * @return True if the two moves are the same.
   */
  auto operator==(const Move &other) const -> bool;
};

#endif
