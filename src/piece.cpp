#include "piece.h"

#include <array>
#include <iostream>
#include <string>
#include <unordered_set>
#include <vector>

Piece::Piece(PieceType type, PieceColor color, int value, bool moved,
             bool pawn_moved_two)
    : type(type), color(color), value(value), moved(moved),
      pawn_moved_two(pawn_moved_two) {}
