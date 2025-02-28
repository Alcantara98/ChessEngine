#ifndef ENGINE_CONSTANTS_H
#define ENGINE_CONSTANTS_H

namespace engine_constants
{
// Board dimensions
const int BOARD_WIDTH = 8;
const int BOARD_HEIGHT = 8;

// Piece values
const int PAWN_VALUE = 100;
const int KNIGHT_VALUE = 320;
const int BISHOP_VALUE = 330;
const int ROOK_VALUE = 500;
const int QUEEN_VALUE = 900;
const int KING_VALUE = 20000;

} // namespace engine_constants

#endif // ENGINE_CONSTANTS_H