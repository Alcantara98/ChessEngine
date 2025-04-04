#ifndef ATTACK_CHECK_H
#define ATTACK_CHECK_H

#include "board_state.h"
#include "engine_constants.h"
#include "move.h"

namespace engine::parts::attack_check
{
/**
 * @brief Checks if the given square is attacked.
 *
 * @param board_state The current state of the chess board.
 * @param x_file The x coordinate of the square (file).
 * @param y_rank The y coordinate of the square (rank).
 * @param color_being_attacked The color of the pieces being attacked.
 *
 * @return True if the square is attacked, false otherwise.
 */
auto square_is_attacked(BoardState &board_state,
                        int x_file,
                        int y_rank,
                        PieceColor color_being_attacked) -> bool;

/**
 * @brief Checks if the king of the given color is in check.
 *
 * @param board_state The current state of the chess board.
 * @param color_of_king The color of the king to check (WHITE or BLACK).
 *
 * @return True if the king is in check, false otherwise.
 */
auto king_is_checked(BoardState &board_state, PieceColor color_of_king) -> bool;

/**
 * @brief Checks if the given move leaves the king in check.
 *
 * @param board_state The current state of the chess board.
 * @param move The move to check.
 *
 * @return True if the move leaves the king in check, false otherwise.
 */
auto move_leaves_king_in_check(BoardState &board_state, Move &move) -> bool;

/**
 * @brief Helper function to check if a square is attacked by a pawn.
 *
 * @param board_state The current state of the chess board.
 * @param x_file The x coordinate of the pawn.
 * @param y_rank The y coordinate of the pawn.
 * @param color_being_attacked The color of the pieces being attacked.
 *
 * @return True if the square is attacked, false otherwise.
 */
static auto
square_is_attacked_by_pawn(BoardState &board_state,
                           int &x_file,
                           int &y_rank,
                           PieceColor &color_being_attacked) -> bool;

/**
 * @brief Helper function to check if a square is attacked by a knight.
 *
 * @param board_state The current state of the chess board.
 * @param x_file The x coordinate of the knight.
 * @param y_rank The y coordinate of the knight.
 * @param color_being_attacked The color of the pieces being attacked.
 *
 * @return True if the square is attacked, false otherwise.
 */
static auto
square_is_attacked_by_knight(BoardState &board_state,
                             int &x_file,
                             int &y_rank,
                             PieceColor &color_being_attacked) -> bool;

/**
 * @brief Helper function to check if a square is attacked by a rook or queen.
 *
 * @param board_state The current state of the chess board.
 * @param x_file The x coordinate of the rook or queen.
 * @param y_rank The y coordinate of the rook or queen.
 * @param color_being_attacked The color of the pieces being attacked.
 *
 * @return True if the square is attacked, false otherwise.
 */
static auto
square_is_attacked_by_rook_or_queen(BoardState &board_state,
                                    int &x_file,
                                    int &y_rank,
                                    PieceColor &color_being_attacked) -> bool;

/**
 * @brief Helper function to check if a square is attacked by a bishop or
 * queen.
 *
 * @param board_state The current state of the chess board.
 * @param x_file The x coordinate of the bishop or queen.
 * @param y_rank The y coordinate of the bishop or queen.
 * @param color_being_attacked The color of the pieces being attacked.
 *
 * @return True if the square is attacked, false otherwise.
 */
static auto
square_is_attacked_by_bishop_or_queen(BoardState &board_state,
                                      int &x_file,
                                      int &y_rank,
                                      PieceColor &color_being_attacked) -> bool;

/**
 * @brief Helper function to check if a square is attacked by a king.
 *
 * @param board_state The current state of the chess board.
 * @param x_file The x coordinate of the king.
 * @param y_rank The y coordinate of the king.
 * @param color_being_attacked The color of the pieces being attacked.
 *
 * @return True if the square is attacked, false otherwise.
 */
static auto
square_is_attacked_by_king(BoardState &board_state,
                           int &x_file,
                           int &y_rank,
                           PieceColor &color_being_attacked) -> bool;
} // namespace engine::parts::attack_check

#endif // ATTACK_CHECK_H