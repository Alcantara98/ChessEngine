#ifndef ENGINE_CONSTANTS_H
#define ENGINE_CONSTANTS_H

#include "piece.h"
#include <array>
#include <limits>
#include <map>
#include <string>

namespace engine::parts
{
// BOARD DIMENSIONS
const int BOARD_WIDTH = 8;
const int BOARD_HEIGHT = 8;
const int NUM_OF_SQUARES = 64;
const int NUM_OF_PIECE_TYPES = 6;
const int NUM_OF_COLORS = 2;

// BOARD POSITIONS
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

// BOARD DIRECTIONS
const int POSITIVE_DIRECTION = 1;
const int NEGATIVE_DIRECTION = -1;

// USER INPUT REGEX MATCH INDEXES
const int CASTLE_MOVE_INDEX = 1;
const int PIECE_TYPE_INDEX = 2;
const int FROM_POSITION_INDEX = 3;
const int CAPTURE_MOVE_INDEX = 4;
const int TO_POSITION_INDEX = 5;
const int PROMOTION_INDEX = 6;

// MAX TRANSPOSITION TABLE CONSTANTS
const int MAX_TRANSPOSITION_TABLE_SIZE = 100000000;
const int FAILED_LOW = -1;
const int FAILED_HIGH = 1;
const int EXACT = 0;

// PERFORMANCE MATRIX CONVERSIONS
const int PERCENTAGE = 100;

// SEARCH ENGINE CONSTANTS
const int MAX_SEARCH_DEPTH = 100;
const int DEFAULT_SEARCH_TIME_MS = 2000;
const int MAX_SEARCH_TIME_MS = 600000;
const int INF = std::numeric_limits<int>::max();
const int INF_MINUS_1000 = INF - 1000;
const int MIN_ROOT_MOVE_PRUNING_DEPTH = 8;
const int ROOT_MOVE_PRUNING_INTERVAL = 2;
const int MIN_SEARCH_THREADS = 12;
const int POSSIBLE_MOVE_RESERVE_SIZE = 100;
const int POSSIBLE_CAPTURE_MOVE_RESERVE_SIZE = 50;
const int TT_FUTILITY_PRUNING_MIN_DEPTH = 8;
const int PROB_CUT_DEPTH_THRESHOLD = 3;
const int MIN_PROB_CUT_DEPTH = 3;

// PIECE VALUES
const int PAWN_VALUE = 80;
const int KNIGHT_VALUE = 320;
const int BISHOP_VALUE = 340;
const int ROOK_VALUE = 500;
const int QUEEN_VALUE = 900;
const int KING_VALUE = 20000;
// For getting MVV_LVA_VALUES.
const std::array<int, 6> PIECE_VALUES = {PAWN_VALUE, KNIGHT_VALUE, BISHOP_VALUE,
                                         ROOK_VALUE, QUEEN_VALUE,  KING_VALUE};

// POSITION EVALUATION CONSTANTS
const int EXTREMELY_SMALL_EVAL_VALUE = 2;
const int VERY_SMALL_EVAL_VALUE = 5;
const int SMALL_EVAL_VALUE = 10;
const int MEDIUM_EVAL_VALUE = 20;
const int LARGE_EVAL_VALUE = 40;
const int VERY_LARGE_EVAL_VALUE = 80;
const int MAX_DOUBLE_PAWN_SQUARES_TO_CHECK = 3;
const int BISHOP_PAIR_COUNT = 2;

// NULL MOVE CONSTANTS
const int NULL_MOVE_REDUCTION = 2;
const int MIN_NULL_MOVE_DEPTH = 2;
const int MIN_NULL_MOVE_ITERATION_DEPTH = 6;
const int NULL_MOVE_ADDITIONAL_DEPTH_DIVISOR = 3;

// LATE MOVE REDUCTION CONSTANTS
const int MIN_LMR_DEPTH = 2;
const int MIN_LMR_ITERATION_DEPTH = 6;
const int LMR_THRESHOLD = 3;
const int EXTREME_LMR_THRESHOLD = 10;
const int LATE_MOVE_REDUCTION = 2;
const int LMR_EXTREME_REDUCTION_INDEX_DIVISOR = 10;
const int LMR_PLY_REDUCTION_DIVISOR = 5;
const int LMR_REDUCTION_DEPTH_DIVISOR = 8;

// FUTILITY PRUNING CONSTANTS
const int MIN_FUTILITY_PRUNING_PLY = 6;

// RAZOR PRUNING CONSTANTS
const int MIN_RAZOR_PRUNING_PLY = 3;
const int RAZOR_BASE_MARGIN = 400;
const int RAZOR_MARGIN_MULTIPLIER = 300;
const int RAZOR_MAX_MARGIN = 3000;

// GAME STATE CONSTANTS
const int INITIAL_MAIN_PIECES_COUNT = 12;
const int INITIAL_QUEENS_COUNT = 2;
const int END_GAME_CONDITION_TWO_QUEENS = 2;
const int END_GAME_CONDITION_ONE_QUEEN = 5;
const int END_GAME_CONDITION_NO_QUEENS = 8;

// HISTORY HEURISTIC CONSTANTS
const int DECAY_RATE_NUMERATOR = 9;
const int DECAY_RATE_DENOMINATOR = 10;

// MVV-LVA CONSTANTS
// First index represents the victim piece, second index represents the attacker
// piece.
const std::array<std::array<int, 6>, 6> MVV_LVA_VALUES = {
    {// Victim:  P   N   B   R   Q   K
     /* Pawn */ {15, 14, 13, 12, 11, 10},
     /* Knight */ {25, 24, 23, 22, 21, 20},
     /* Bishop */ {35, 34, 33, 32, 31, 30},
     /* Rook */ {45, 44, 43, 42, 41, 40},
     /* Queen */ {55, 54, 53, 52, 51, 50},
     /* King */ {65, 64, 63, 62, 61, 60}}};

// CHECKSUM CONSTANTS
const uint32_t CHECKSUM_SEED = 0x811C9DC5;
const std::array<int, 5> CHECKSUM_PRIMES = {31, 37, 41, 43, 47};

// ASPIRATION WINDOW CONSTANTS
const std::array<int, 4> ASPIRATION_WINDOWS = {
    {PAWN_VALUE / 2, PAWN_VALUE * 2, INF}};

// POSITION EVALUATION MAP FOR PIECES
const std::array<int, 8> PAWN_POSITION_EVAL_MAP = {{0, 4, 8, 10, 10, 8, 4, 0}};
const std::array<int, 8> KNIGHT_POSITION_EVAL_MAP = {{1, 3, 4, 5, 5, 4, 3, 1}};

const std::array<int, 8> KING_POSITION_EVAL_MAP = {{5, 20, 0, 0, 0, 0, 20, 5}};

// DIRECTION MAPS FOR PIECES
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

// PIECE TYPE MAPPINGS
const std::map<char, parts::PieceType> CHAR_TO_PIECE_TYPE = {
    {'k', parts::PieceType::KING},   {'q', parts::PieceType::QUEEN},
    {'r', parts::PieceType::ROOK},   {'b', parts::PieceType::BISHOP},
    {'n', parts::PieceType::KNIGHT}, {'p', parts::PieceType::PAWN},
    {'-', parts::PieceType::EMPTY}};

const std::map<parts::PieceType, char> PIECE_TYPE_TO_CHAR = {
    {parts::PieceType::KING, 'k'},   {parts::PieceType::QUEEN, 'q'},
    {parts::PieceType::ROOK, 'r'},   {parts::PieceType::BISHOP, 'b'},
    {parts::PieceType::KNIGHT, 'n'}, {parts::PieceType::PAWN, 'p'},
    {parts::PieceType::EMPTY, '-'}};

const std::map<char, int> ALGEBRAIC_TO_INT = {{'a', 0}, {'b', 1}, {'c', 2},
                                              {'d', 3}, {'e', 4}, {'f', 5},
                                              {'g', 6}, {'h', 7}};

const std::map<int, char> INT_TO_ALGEBRAIC = {{0, 'a'}, {1, 'b'}, {2, 'c'},
                                              {3, 'd'}, {4, 'e'}, {5, 'f'},
                                              {6, 'g'}, {7, 'h'}};

// STATE NAMES
const std::string MAIN_MENU_STATE = "Main Menu";
const std::string PLAYER_VS_PLAYER_STATE = "Player vs Player";
const std::string ENGINE_VS_PLAYER_STATE = "Engine vs Player";

// CHESS ENGINE CONSTANTS
const int INPUT_DELAY_TIME = 100;
const char WHITE_PIECE_CHAR = 'w';
const char BLACK_PIECE_CHAR = 'b';

// INTERFACE MESSAGES
const std::string GAME_OVER_HELP_MESSAGE =
    "\n-- Game Over-- \n\nCommand Options :\n  - menu\n  - exit\n  - undo\n  - "
    "reset\n  - play-engine\n  - play-player\n  - print-moves\n  - help\n\n "
    "Update Engine Parameters:\n  - update-depth\n  - update-timelimit\n  - "
    "update-window\n  - update-info\n  - update-pondering\n\nEnter one of the "
    "commands above: ";

const std::string HELP_MESSAGE =
    "\nCommands:\n\n ALL States:\n  - menu\n  - exit\n  - play-engine\n  - "
    "play-player\n  - help\n\n All Playing States:\n  - undo\n  - reset\n  - "
    "redo\n\n Player's Turn\n  - print-moves\n  - enter a move\n  - Update "
    "Engine Parameters:\n    ~ update-depth\n    ~ update-timelimit\n    ~ "
    "update-window\n    ~ update-info\n    ~ update-pondering\n\n "
    "Engine's turn:\n  - stop-search\n\n";

} // namespace engine::parts

#endif // ENGINE_CONSTANTS_H