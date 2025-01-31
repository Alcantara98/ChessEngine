#include "piece.h"

Piece::Piece(PieceType type, PieceColor color, bool moved, bool pawn_moved_two)
    : type(type), color(color), moved(moved), pawn_moved_two(pawn_moved_two) {}
