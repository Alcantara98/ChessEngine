#include "piece.h"

#include <array>
#include <iostream>
#include <string>
#include <unordered_set>
#include <vector>

Move::Move(Piece moving_piece = Piece(), Piece captured_piece = Piece(), Piece promotion_piece = Piece(), int from_x,
           int from_y, int to_x, int to_y, bool is_castling = false, bool is_castling_king_side = true,
           bool is_en_passant = false)
    : moving_piece(moving_piece)
    , captured_piece(captured_piece)
    , promotion_piece(promotion_piece)
    , from_x(from_x)
    , from_y(from_y)
    , to_x(to_x)
    , to_y(to_y)
    , is_castling(is_castling)
    , is_castling_king_side(is_castling_king_side)
    , is_en_passant(is_en_passant)
{
}

Piece::Piece()
    : type(PieceType::EMPTY)
    , color(PieceColor::NONE)
    , value(0)
    , pawn_moved_two(false)
{
}

Piece::Piece(PieceType type, PieceColor color, int value, bool moved = false, bool pawn_moved_two = false)
    : type(type)
    , color(color)
    , value(value)
    , pawn_moved_two(pawn_moved_two)
{
}
