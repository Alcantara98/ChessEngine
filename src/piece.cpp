#include "piece.h"

#include <array>
#include <iostream>
#include <string>
#include <unordered_set>
#include <vector>

Move::Move(int from_x, int from_y, int to_x, int to_y, Piece moving_piece, bool first_move, bool is_castling,
           bool is_castling_king_side, bool pawn_moved_two)
    : from_x(from_x)
    , from_y(from_y)
    , to_x(to_x)
    , to_y(to_y)
    , moving_piece(moving_piece)
    , captured_piece(Piece())
    , promotion_piece_type(PieceType::EMPTY)
    , is_castling(is_castling)
    , is_castling_king_side(is_castling_king_side)
    , is_en_passant(false)
    , first_move(first_move)
    , pawn_moved_two(pawn_moved_two)
{
}

Move::Move(int from_x, int from_y, int to_x, int to_y, Piece moving_piece, PieceType promotion_piece_type)
    : from_x(from_x)
    , from_y(from_y)
    , to_x(to_x)
    , to_y(to_y)
    , moving_piece(moving_piece)
    , captured_piece(Piece())
    , promotion_piece_type(promotion_piece_type)
    , is_castling(false)
    , is_castling_king_side(true)
    , is_en_passant(false)
    , first_move(false)
    , pawn_moved_two(false)
{
}

Move::Move(int from_x, int from_y, int to_x, int to_y, Piece moving_piece, Piece captured_piece,
           bool first_move, bool is_en_passant)
    : from_x(from_x)
    , from_y(from_y)
    , to_x(to_x)
    , to_y(to_y)
    , moving_piece(moving_piece)
    , captured_piece(captured_piece)
    , promotion_piece_type(PieceType::EMPTY)
    , is_castling(false)
    , is_castling_king_side(false)
    , is_en_passant(is_en_passant)
    , first_move(first_move)
    , pawn_moved_two(false)
{
}

Move::Move(int from_x, int from_y, int to_x, int to_y, Piece moving_piece, Piece captured_piece,
           PieceType promotion_piece_type)
    : from_x(from_x)
    , from_y(from_y)
    , to_x(to_x)
    , to_y(to_y)
    , moving_piece(moving_piece)
    , captured_piece(captured_piece)
    , promotion_piece_type(promotion_piece_type)
    , is_castling(false)
    , is_castling_king_side(false)
    , is_en_passant(is_en_passant)
    , first_move(false)
    , pawn_moved_two(false)
{
}

Piece::Piece(PieceType type, PieceColor color, int value, bool moved, bool pawn_moved_two)
    : type(type)
    , color(color)
    , value(value)
    , pawn_moved_two(pawn_moved_two)
{
}
