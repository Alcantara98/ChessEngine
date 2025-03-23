#ifndef MOVE_GENERATOR_H
#define MOVE_GENERATOR_H

#include "board_state.h"
#include "move.h"
#include "piece.h"

#include <array>
#include <vector>

/**
 * @brief Namespace for move generator functions.
 */
namespace engine::parts::move_generator
{

/**
 * @brief Calculates all possible moves of current board state.
 *
 * @param board_state BoardState object to calculate moves from.
 * @param capture_only Flag to calculate only capture moves.
 *
 * @return Vector of possible moves.
 */
auto calculate_possible_moves(BoardState &board_state,
                              bool capture_only = false) -> std::vector<Move>;

/**
 * @brief Generates all possible moves for a given pawn.
 *
 * @note Generated moves are pushed back into the possible_moves vector.
 *
 * @param board_state Reference of the current board state.
 * @param x_position, y_position The coordinate of the pawn.
 * @param possible_moves Reference to the list of possible moves of current
 * board_state.
 */
static void generate_pawn_moves(BoardState &board_state,
                                int x_position,
                                int y_position,
                                std::vector<Move> &possible_moves,
                                bool capture_only = false);

/**
 * @brief Generates one square forward and two squares forward moves for a pawn.
 *
 * @note Generated moves are pushed back into the possible_moves vector.
 *
 * @param chess_board Reference of the current chess board.
 * @param x_position, y_position The coordinate of the pawn.
 * @param possible_moves Reference to the list of possible moves of current
 * board_state.
 * @param pawn_piece The pawn piece.
 * @param pawn_direction The direction of the pawn.
 * @param first_move True if the pawn has not moved yet.
 * @param promotion_rank The rank to promote the pawn.
 */
static void generate_normal_pawn_moves(chess_board_type &chess_board,
                                       int x_position,
                                       int y_position,
                                       std::vector<Move> &possible_moves,
                                       Piece *pawn_piece,
                                       int pawn_direction,
                                       bool first_move,
                                       int promotion_rank);

/**
 * @brief Generates normal pawn capture moves.
 *
 * @note Generated moves are pushed back into the possible_moves vector.
 *
 * @param chess_board Reference of the current chess board.
 * @param x_position, y_position The coordinate of the pawn.
 * @param possible_moves Reference to the list of possible moves of current
 * board_state.
 * @param pawn_piece The pawn piece.
 * @param pawn_direction The direction of the pawn.
 * @param first_move True if the pawn has not moved yet.
 * @param promotion_rank The rank to promote the pawn.
 */
static void generate_pawn_capture_moves(chess_board_type &chess_board,
                                        int x_position,
                                        int y_position,
                                        std::vector<Move> &possible_moves,
                                        Piece *pawn_piece,
                                        int pawn_direction,
                                        bool first_move,
                                        int promotion_rank);

/**
 * @brief Generates en passant pawn capture moves.
 *
 * @note Generated moves are pushed back into the possible_moves vector.
 *
 * @param chess_board Reference of the current chess board.
 * @param x_position, y_position The coordinate of the pawn.
 * @param possible_moves Reference to the list of possible moves of current
 * board_state.
 * @param pawn_piece The pawn piece.
 * @param pawn_direction The direction of the pawn.
 * @param first_move True if the pawn has not moved yet.
 * @param previous_move The previous move applied on the board.
 */
static void
generate_en_passant_pawn_capture_moves(chess_board_type &chess_board,
                                       int x_position,
                                       int y_position,
                                       std::vector<Move> &possible_moves,
                                       Piece *pawn_piece,
                                       int pawn_direction,
                                       bool first_move,
                                       Move &previous_move);

/**
 * @brief Generates all possible moves for a given king.
 *
 * @note Generated moves are pushed back into the possible_moves vector.
 *
 * @param board_state Reference of the current board state.
 * @param x_position, y_position The coordinate of the king.
 * @param possible_moves Reference to the list of possible moves of current
 * board_state.
 */
static void generate_king_moves(BoardState &board_state,
                                int x_position,
                                int y_position,
                                std::vector<Move> &possible_moves,
                                bool capture_only = false);

/**
 * @brief Generates all possible moves for a given queen.
 *
 * @note Generated moves are pushed back into the possible_moves vector.
 *
 * @param board_state Reference of the current board state.
 * @param x_position, y_position The coordinate of the queen.
 * @param possible_moves Reference to the list of possible moves of current
 * board_state.
 */
static void generate_castle_king_moves(BoardState &board_state,
                                       int x_position,
                                       int y_position,
                                       std::vector<Move> &possible_moves);

/**
 * @brief Helper function to check if the king can castle.
 *
 * @param board_state Reference of the current board state.
 * @param king_piece The king piece.
 * @param x_position, y_position The coordinate of the rook.
 * @param potential_rook_piece The potential rook piece.
 * @param castle_path Squares the king must pass through to castle.
 *
 * @return bool True if the rook can castle.
 */
static auto can_castle(BoardState &board_state,
                       Piece *king_piece,
                       int y_position,
                       Piece *potential_rook_piece,
                       const std::vector<int> &castle_path) -> bool;

/**
 * @brief Generates all possible moves for a given knight.
 *
 * @note Generated moves are pushed back into the possible_moves vector.
 *
 * @param board_state Reference of the current board state.
 * @param x_position, y_position The coordinate of the knight.
 * @param possible_moves Reference to the list of possible moves of current
 * board_state.
 */
static void generate_knight_moves(BoardState &board_state,
                                  int x_position,
                                  int y_position,
                                  std::vector<Move> &possible_moves,
                                  bool capture_only = false);

/**
 * @brief Generates all possible moves for a given bishop.
 *
 * @note Generated moves are pushed back into the possible_moves vector.
 *
 * @param board_state Reference of the current board state.
 * @param x_position, y_position The coordinate of the bishop.
 * @param possible_moves Reference to the list of possible moves of current
 * board_state.
 */
static void generate_bishop_moves(BoardState &board_state,
                                  int x_position,
                                  int y_position,
                                  std::vector<Move> &possible_moves,
                                  bool capture_only = false);

/**
 * @brief Generates all possible moves for a given rook.
 *
 * @note Generated moves are pushed back into the possible_moves vector.
 *
 * @param board_state Reference of the current board state.
 * @param x_position, y_position The coordinate of the rook.
 * @param possible_moves Reference to the list of possible moves of current
 * board_state.
 */
static void generate_rook_moves(BoardState &board_state,
                                int x_position,
                                int y_position,
                                std::vector<Move> &possible_moves,
                                bool capture_only = false);

/**
 * @brief Generates all possible moves for a given queen.
 *
 * @note Generated moves are pushed back into the possible_moves vector.
 *
 * @param board_state Reference of the current board state.
 * @param x_position, y_position The coordinate of the queen.
 * @param possible_moves Reference to the list of possible moves of current
 * board_state.
 */
static void generate_queen_moves(BoardState &board_state,
                                 int x_position,
                                 int y_position,
                                 std::vector<Move> &possible_moves,
                                 bool capture_only = false);

/**
 * @brief Generates bishop and rook moves in one given direction.
 *
 * @param board_state Reference of the current board state.
 * @param x_position, y_position The coordinate of the bishop or bishop.
 * @param x_direction, y_direction The direction to move.
 * @param possible_moves Reference to the list of possible moves of current
 * board_state.
 */
static void rook_bishop_move_helper(BoardState &board_state,
                                    int x_position,
                                    int y_position,
                                    int x_direction,
                                    int y_direction,
                                    std::vector<Move> &possible_moves,
                                    bool capture_only = false);
} // namespace engine::parts::move_generator

#endif // MOVE_GENERATOR_H