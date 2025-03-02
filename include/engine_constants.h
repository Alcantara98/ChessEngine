#ifndef ENGINE_CONSTANTS_H
#define ENGINE_CONSTANTS_H

#include "piece.h"
#include <array>
#include <map>

namespace engine::constants
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

// Directions for the queen.
const std::array<std::array<int, 2>, 8> QUEEN_DIRECTIONS = {
    {{1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1}}};

// Directions for the bishop.
const std::array<std::array<int, 2>, 4> BISHOP_DIRECTIONS = {
    {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}}};

// Directions for the knight.
const std::array<std::array<int, 2>, 8> KNIGHT_MOVES = {
    {{1, 2}, {1, -2}, {-1, 2}, {-1, -2}, {2, 1}, {2, -1}, {-2, 1}, {-2, -1}}};

// Directions for the rook.
const std::array<std::array<int, 2>, 4> ROOK_DIRECTIONS = {
    {{1, 0}, {-1, 0}, {0, 1}, {0, -1}}};

// Map to convert string piece type to PieceType.
const std::map<char, parts::PieceType> STRING_TO_PIECE_TYPE = {
    {'k', parts::PieceType::KING},   {'q', parts::PieceType::QUEEN},
    {'r', parts::PieceType::ROOK},   {'b', parts::PieceType::BISHOP},
    {'n', parts::PieceType::KNIGHT}, {'p', parts::PieceType::PAWN}};

// Map to convert PieceType to string piece type.
const std::map<parts::PieceType, char> PIECE_TYPE_TO_STRING = {
    {parts::PieceType::KING, 'k'},   {parts::PieceType::QUEEN, 'q'},
    {parts::PieceType::ROOK, 'r'},   {parts::PieceType::BISHOP, 'b'},
    {parts::PieceType::KNIGHT, 'n'}, {parts::PieceType::PAWN, 'p'}};

// Map to convert algebraic coordinates to int.
const std::map<char, int> ALGEBRAIC_TO_INT = {{'a', 0}, {'b', 1}, {'c', 2},
                                              {'d', 3}, {'e', 4}, {'f', 5},
                                              {'g', 6}, {'h', 7}};

// Map to convert int to algebraic coordinates.
const std::map<int, char> INT_TO_ALGEBRAIC = {{0, 'a'}, {1, 'b'}, {2, 'c'},
                                              {3, 'd'}, {4, 'e'}, {5, 'f'},
                                              {6, 'g'}, {7, 'h'}};
} // namespace engine::constants

#endif // ENGINE_CONSTANTS_H