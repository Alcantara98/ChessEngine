#ifndef POSITION_EVALUATOR_H
#define POSITION_EVALUATOR_H

#include "board_state.h"

/**
 * @brief Namespace for position evaluator functions.
 */
namespace engine::parts::position_evaluator
{

/**
 * @brief Evaluates current position using chess heuristics.
 *
 * @param board_state BoardState object to evaluate.
 *
 * @return Score of the given position.
 */
auto evaluate_position(BoardState &board_state) -> int;

/**
 * @brief Evaluates pawn at given position.
 *
 * @param x_position, y_position Position of the pawn.
 * @param pawn_piece Pawn piece object to evaluate.
 * @param eval Evaluation score to update.
 * @param board_state BoardState object to evaluate.
 */
static void evaluate_pawn(int x_position, int y_position, Piece &pawn_piece,
                          int &eval, BoardState &board_state);

/**
 * @brief Evaluates knight at given position.
 *
 * @param x_position, y_position Position of the knight.
 * @param knight_piece Knight piece object to evaluate.
 * @param eval Evaluation score to update.
 * @param board_state BoardState object to evaluate.
 */
static void evaluate_knight(int x_position, int y_position, Piece &knight_piece,
                            int &eval, BoardState &board_state);

/**
 * @brief Evaluates bishop at given position.
 *
 * @param x_position, y_position Position of the bishop.
 * @param bishop_piece Bishop piece object to evaluate.
 * @param eval Evaluation score to update.
 * @param board_state BoardState object to evaluate.
 */
static void evaluate_bishop(int x_position, int y_position, Piece &bishop_piece,
                            int &eval, BoardState &board_state);

/**
 * @brief Evaluates rook at given position.
 *
 * @param x_position, y_position Position of the rook.
 * @param rook_piece Rook piece object to evaluate.
 * @param eval Evaluation score to update.
 * @param board_state BoardState object to evaluate.
 */
static void evaluate_rook(int x_position, int y_position, Piece &rook_piece,
                          int &eval, BoardState &board_state);

/**
 * @brief Evaluates queen at given position.
 *
 * @param x_position, y_position Position of the queen.
 * @param queen_piece Queen piece object to evaluate.
 * @param eval Evaluation score to update.
 * @param board_state BoardState object to evaluate.
 */
static void evaluate_queen(int x_position, int y_position, Piece &queen_piece,
                           int &eval, BoardState &board_state);

/**
 * @brief Evaluates king at given position.
 *
 * @param x_position, y_position Position of the king.
 * @param king_piece King piece object to evaluate.
 * @param eval Evaluation score to update.
 * @param board_state BoardState object to evaluate.
 */
static void evaluate_king(int x_position, int y_position, Piece &king_piece,
                          int &eval, BoardState &board_state);

/**
 * @brief Evaluates king safety at given position.
 *
 * @param x_position, y_position Position of the king.
 * @param king_piece King piece object to evaluate.
 * @param eval Evaluation score to update.
 * @param board_state BoardState object to evaluate.
 */
static void evaluate_king_safety(int x_position, int y_position,
                                 Piece &king_piece, int &eval,
                                 BoardState &board_state);
} // namespace engine::parts::position_evaluator

#endif