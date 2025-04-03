#ifndef POSITION_EVALUATOR_H
#define POSITION_EVALUATOR_H

#include "board_state.h"

/**
 * @brief Namespace for position evaluator functions.
 */
namespace engine::parts::position_evaluator
{

/**
 * @brief Evaluates the current position using chess heuristics.
 *
 * @note Positive score is good for white, negative score is good for black, and
 * 0 means the position is equal.
 *
 * @param board_state BoardState object to evaluate.
 *
 * @return Score of the given position.
 */
auto evaluate_position(BoardState &board_state) -> int;

/**
 * @brief Evaluates a pawn at the given position.
 *
 * @param x_file The x-coordinate (file) of the pawn.
 * @param y_rank The y-coordinate (rank) of the pawn.
 * @param pawn_piece The pawn piece object to evaluate.
 * @param eval Reference to the evaluation score to update.
 * @param board_state BoardState object to evaluate.
 */
static void evaluate_pawn(int x_file,
                          int y_rank,
                          Piece &pawn_piece,
                          int &eval,
                          BoardState &board_state);

/**
 * @brief Evaluates a knight at the given position.
 *
 * @param x_file The x-coordinate (file) of the knight.
 * @param y_rank The y-coordinate (rank) of the knight.
 * @param knight_piece The knight piece object to evaluate.
 * @param eval Reference to the evaluation score to update.
 * @param board_state BoardState object to evaluate.
 */
static void evaluate_knight(int x_file,
                            int y_rank,
                            Piece &knight_piece,
                            int &eval,
                            BoardState &board_state);

/**
 * @brief Evaluates a bishop at the given position.
 *
 * @param x_file The x-coordinate (file) of the bishop.
 * @param y_rank The y-coordinate (rank) of the bishop.
 * @param bishop_piece The bishop piece object to evaluate.
 * @param eval Reference to the evaluation score to update.
 * @param board_state BoardState object to evaluate.
 */
static void evaluate_bishop(int x_file,
                            int y_rank,
                            Piece &bishop_piece,
                            int &eval,
                            BoardState &board_state);

/**
 * @brief Evaluates a rook at the given position.
 *
 * @param x_file The x-coordinate (file) of the rook.
 * @param y_rank The y-coordinate (rank) of the rook.
 * @param rook_piece The rook piece object to evaluate.
 * @param eval Reference to the evaluation score to update.
 * @param board_state BoardState object to evaluate.
 */
static void evaluate_rook(int x_file,
                          int y_rank,
                          Piece &rook_piece,
                          int &eval,
                          BoardState &board_state);

/**
 * @brief Evaluates a queen at the given position.
 *
 * @param x_file The x-coordinate (file) of the queen.
 * @param y_rank The y-coordinate (rank) of the queen.
 * @param queen_piece The queen piece object to evaluate.
 * @param eval Reference to the evaluation score to update.
 * @param board_state BoardState object to evaluate.
 */
static void evaluate_queen(int x_file,
                           int y_rank,
                           Piece &queen_piece,
                           int &eval,
                           BoardState &board_state);

/**
 * @brief Evaluates a king at the given position.
 *
 * @param x_file The x-coordinate (file) of the king.
 * @param y_rank The y-coordinate (rank) of the king.
 * @param king_piece The king piece object to evaluate.
 * @param eval Reference to the evaluation score to update.
 * @param board_state BoardState object to evaluate.
 */
static void evaluate_king(int x_file,
                          int y_rank,
                          Piece &king_piece,
                          int &eval,
                          BoardState &board_state);

/**
 * @brief Evaluates the safety of a king at the given position.
 *
 * @param x_file The x-coordinate (file) of the king.
 * @param y_rank The y-coordinate (rank) of the king.
 * @param king_piece The king piece object to evaluate.
 * @param eval Reference to the evaluation score to update.
 * @param board_state BoardState object to evaluate.
 */
static void evaluate_king_safety(int x_file,
                                 int y_rank,
                                 Piece &king_piece,
                                 int &eval,
                                 BoardState &board_state);
} // namespace engine::parts::position_evaluator

#endif