#ifndef MOVE_H
#define MOVE_H

#include "piece.h"

struct Move {
  Piece *moving_piece;
  Piece *captured_piece;
  PieceType promotion_piece_type;
  int from_x, from_y;
  int to_x, to_y;
  bool is_en_passant;
  bool first_move;
  bool pawn_moved_two;
  // Coordinates of the pawn that moved two.
  int pmt_x, pmt_y;

  /**
   * @brief Constructor with all parameters.
   *
   * @param from_x, from_y Initial coordinates of moving piece.
   * @param to_x, to_y Final coordinates of moving piece.
   * @param moving_piece The piece to move.
   * @param captured_piece True if move captures another piece.
   * @param promotion_piece_type Promote piece to this piece type.
   * @param is_en_passant True if Pawn will capture En Passant.
   * @param first_move Specifies whether this is the piece's first move.
   * @param pawn_moved_two Specifies whether a pawn moved two squares forward.
   * @param pmt_x, pmt_Y Coordinates of the pawn that moved two squares.
   */
  Move(int from_x, int from_y, int to_x, int to_y, Piece *moving_piece,
       Piece *captured_piece, PieceType promotion_piece_type,
       bool is_en_passant, bool first_move, bool pawn_moved_two, int pmt_x,
       int pmt_y);

  /**
   * @brief Constructor for normal move without captures.
   *
   * @param from_x, from_y Initial coordinates of moving piece.
   * @param to_x, to_y Final coordinates of moving piece.
   * @param moving_piece The piece to move.
   * @param first_move Specifies whether this is the piece's first move.
   * @param pawn_moved_two Specifies whether a pawn moved two squares forward.
   * @param pmt_x, pmt_Y Coordinates of the pawn that moved two squares.
   */
  Move(int from_x, int from_y, int to_x, int to_y, Piece *moving_piece,
       bool first_move = false, bool pawn_moved_two = false, int pmt_x = -1,
       int pmt_y = -1);

  /**
   * @brief Constructor for pawn promotion through normal move.
   *
   * @param from_x, from_y Initial coordinates of moving piece.
   * @param to_x, to_y Final coordinates of moving piece.
   * @param moving_piece The piece to move.
   * @param promotion_piece_type Promote piece to this piece type.
   */
  Move(int from_x, int from_y, int to_x, int to_y, Piece *moving_piece,
       PieceType promotion_piece_type);

  /**
   * @brief Constructor for captures.
   *
   * @param from_x, from_y Initial coordinates of moving piece.
   * @param to_x, to_y Final coordinates of moving piece.
   * @param moving_piece The piece to move.
   * @param captured_piece True if move captures another piece.
   * @param is_en_passant True if Pawn will capture En Passant.
   * @param first_move Specifies whether this is the piece's first move.
   */
  Move(int from_x, int from_y, int to_x, int to_y, Piece *moving_piece,
       Piece *captured_piece, bool first_move = false,
       bool is_en_passant = false);

  /**
   * @brief Constructor for pawn promotion through captures.
   *
   * @param from_x, from_y Initial coordinates of moving piece.
   * @param to_x, to_y Final coordinates of moving piece.
   * @param moving_piece The piece to move.
   * @param captured_piece True if move captures another piece.
   * @param promotion_piece_type Promote piece to this piece type.
   */
  Move(int from_x, int from_y, int to_x, int to_y, Piece *moving_piece,
       Piece *captured_piece, PieceType promotion_piece_type);
};

#endif
