#include "move.h"

Move::Move(int from_x, int from_y, int to_x, int to_y, Piece *moving_piece,
           Piece *captured_piece, PieceType promotion_piece_type,
           bool is_en_passant, bool first_move, bool pawn_moved_two, int pmt_x,
           int pmt_y)
    : from_x(from_x), from_y(from_y), to_x(to_x), to_y(to_y),
      moving_piece(moving_piece), captured_piece(captured_piece),
      promotion_piece_type(promotion_piece_type),
      capture_is_en_passant(is_en_passant),
      first_move_of_moving_piece(first_move),
      pawn_moved_two_squares(pawn_moved_two),
      pawn_moved_two_squares_to_x(pmt_x), pawn_moved_two_squares_to_y(pmt_y) {}

Move::Move(int from_x, int from_y, int to_x, int to_y, Piece *moving_piece,
           bool first_move, bool pawn_moved_two, int pmt_x, int pmt_y)
    : from_x(from_x), from_y(from_y), to_x(to_x), to_y(to_y),
      moving_piece(moving_piece), captured_piece(nullptr),
      promotion_piece_type(PieceType::EMPTY), capture_is_en_passant(false),
      first_move_of_moving_piece(first_move),
      pawn_moved_two_squares(pawn_moved_two),
      pawn_moved_two_squares_to_x(pmt_x), pawn_moved_two_squares_to_y(pmt_y) {}

Move::Move(int from_x, int from_y, int to_x, int to_y, Piece *moving_piece,
           Piece *captured_piece, bool first_move, bool is_en_passant)
    : from_x(from_x), from_y(from_y), to_x(to_x), to_y(to_y),
      moving_piece(moving_piece), captured_piece(captured_piece),
      promotion_piece_type(PieceType::EMPTY),
      capture_is_en_passant(is_en_passant),
      first_move_of_moving_piece(first_move), pawn_moved_two_squares(false),
      pawn_moved_two_squares_to_x(-1), pawn_moved_two_squares_to_y(-1) {}

Move::Move(int from_x, int from_y, int to_x, int to_y, Piece *moving_piece,
           PieceType promotion_piece_type)
    : from_x(from_x), from_y(from_y), to_x(to_x), to_y(to_y),
      moving_piece(moving_piece), captured_piece(nullptr),
      promotion_piece_type(promotion_piece_type), capture_is_en_passant(false),
      first_move_of_moving_piece(false), pawn_moved_two_squares(false),
      pawn_moved_two_squares_to_x(-1), pawn_moved_two_squares_to_y(-1) {}

Move::Move(int from_x, int from_y, int to_x, int to_y, Piece *moving_piece,
           Piece *captured_piece, PieceType promotion_piece_type)
    : from_x(from_x), from_y(from_y), to_x(to_x), to_y(to_y),
      moving_piece(moving_piece), captured_piece(captured_piece),
      promotion_piece_type(promotion_piece_type), capture_is_en_passant(false),
      first_move_of_moving_piece(false), pawn_moved_two_squares(false),
      pawn_moved_two_squares_to_x(-1), pawn_moved_two_squares_to_y(-1) {}

auto Move::operator==(const Move &other) const -> bool {
  return from_x == other.from_x && from_y == other.from_y &&
         to_x == other.to_x && to_y == other.to_y &&
         moving_piece == other.moving_piece &&
         captured_piece == other.captured_piece &&
         promotion_piece_type == other.promotion_piece_type &&
         capture_is_en_passant == other.capture_is_en_passant &&
         first_move_of_moving_piece == other.first_move_of_moving_piece &&
         pawn_moved_two_squares == other.pawn_moved_two_squares &&
         pawn_moved_two_squares_to_x == other.pawn_moved_two_squares_to_x &&
         pawn_moved_two_squares_to_y == other.pawn_moved_two_squares_to_y;
}
