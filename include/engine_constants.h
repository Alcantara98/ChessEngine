#ifndef ENGINE_CONSTANTS_H
#define ENGINE_CONSTANTS_H

#include "piece.h"
#include <array>
#include <limits>
#include <map>
#include <string>

namespace engine::parts
{
// Board dimensions.
const int BOARD_WIDTH = 8;
const int BOARD_HEIGHT = 8;
const int NUM_OF_SQUARES = 64;
const int NUM_OF_PIECE_TYPES = 6;
const int NUM_OF_COLORS = 2;

// Board positions.
const int XA_FILE = 0;
const int XB_FILE = 1;
const int XC_FILE = 2;
const int XD_FILE = 3;
const int XE_FILE = 4;
const int XF_FILE = 5;
const int XG_FILE = 6;
const int XH_FILE = 7;
const int Y1_RANK = 0;
const int Y2_RANK = 1;
const int Y3_RANK = 2;
const int Y4_RANK = 3;
const int Y5_RANK = 4;
const int Y6_RANK = 5;
const int Y7_RANK = 6;
const int Y8_RANK = 7;
const int Y_MIN = 0;
const int Y_MAX = 7;
const int X_MIN = 0;
const int X_MAX = 7;

// Board directions.
const int POSITIVE_DIRECTION = 1;
const int NEGATIVE_DIRECTION = -1;

// User input regex match index.
const int CASTLE_MOVE_INDEX = 1;
const int PIECE_TYPE_INDEX = 2;
const int FROM_POSITION_INDEX = 3;
const int CAPTURE_MOVE_INDEX = 4;
const int TO_POSITION_INDEX = 5;
const int PROMOTION_INDEX = 6;

// Max transposition table size.
const int MAX_TRANSPOSITION_TABLE_SIZE = 10000000;

// Performance matrix conversions.
const int NODES_TO_KILONODES = 1000;
const double MILLISECONDS_TO_SECONDS = 1000.0;

// Search Engine constants.
const int MAX_SEARCH_DEPTH = 30;
// Use for starting values of alpha and beta;
const int INF = std::numeric_limits<int>::max();

// Piece values.
const int PAWN_VALUE = 100;
const int KNIGHT_VALUE = 320;
const int BISHOP_VALUE = 330;
const int ROOK_VALUE = 500;
const int QUEEN_VALUE = 900;
const int KING_VALUE = 20000;

// Max number of moves for a piece.
const int MAX_MOVES_KNIGHT = 8;

// Evaluation points.
const int VERY_SMALL_EVAL_VALUE = 5;
const int SMALL_EVAL_VALUE = 10;
const int MEDIUM_EVAL_VALUE = 20;
const int LARGE_EVAL_VALUE = 40;
const int VERY_LARGE_EVAL_VALUE = 80;

// Null Move Constants.
const int NULL_MOVE_REDUCTION = 3;
const int MIN_NULL_MOVE_DEPTH = 5;

// Game state constants.
const int START_MAIN_PIECES_COUNT = 12;
const int START_QUEENS_COUNT = 2;
const int END_GAME_CONDITION_TWO_QUEENS = 2;
const int END_GAME_CONDITION_ONE_QUEEN = 5;
const int END_GAME_CONDITION_NO_QUEENS = 8;

// Aspiration Window Constants.
const std::array<int, 3> ASPIRATION_WINDOWS = {
    {PAWN_VALUE + 1, (PAWN_VALUE * 2) + 1, INF}};

// Position evaluation map for pieces.
const std::array<int, 8> PAWN_POSITION_EVAL_MAP = {
    {5, 10, 20, 30, 30, 20, 10, 5}};

const std::array<int, 8> KING_POSITION_EVAL_MAP = {{5, 20, 0, 0, 0, 0, 20, 5}};

// Direction Maps for Pieces
const std::array<std::array<int, 2>, 8> QUEEN_DIRECTIONS = {
    {{1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1}}};

const std::array<std::array<int, 2>, 4> BISHOP_DIRECTIONS = {
    {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}}};

const std::array<std::array<int, 2>, 8> KNIGHT_MOVES = {
    {{1, 2}, {1, -2}, {-1, 2}, {-1, -2}, {2, 1}, {2, -1}, {-2, 1}, {-2, -1}}};

const std::array<std::array<int, 2>, 4> ROOK_DIRECTIONS = {
    {{1, 0}, {-1, 0}, {0, 1}, {0, -1}}};

const std::array<std::array<int, 2>, 8> KING_MOVES = {{{-1, -1},
                                                       {-1, 0},
                                                       {-1, +1},
                                                       {0, -1},
                                                       {0, +1},
                                                       {+1, -1},
                                                       {+1, 0},
                                                       {+1, +1}}};

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

// State names.
const std::string MAIN_MENU_STATE = "Main Menu";
const std::string PLAYER_VS_PLAYER_STATE = "Player vs Player";
const std::string ENGINE_VS_PLAYER_STATE = "Engine vs Player";

// Interface messages.
const std::string GAME_OVER_HELP_MESSAGE =
    "\n-- Game Over-- \n\nCommand Options :\n  - menu\n  - exit\n  - undo\n  - "
    "reset\n  - play-engine\n  - play-player\n  - print-moves\n  - "
    "help\n\nEnter one of the commands above: ";

const std::string HELP_MESSAGE =
    "\nCommands:\n\n ALL States:\n  - menu\n  - exit\n  - play-engine\n  - "
    "play-player\n  - help\n\n Playing States:\n  - undo\n  - reset\n -  "
    "print-moves\n -  enter a move\n\n";
} // namespace engine::parts

#endif // ENGINE_CONSTANTS_H