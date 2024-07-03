#ifndef MOVE_GENERATOR_H
#define MOVE_GENERATOR_H
#include "board_state.h"
#include "piece.h"

#include <vector>

/**
 * @brief Generates all possible moves for a given pawn.
 *
 * @param board Reference of the current board state.
 * @param x The x-coordinate of the pawn.
 * @param y The y-coordinate of the pawn.
 * @param possible_moves Reference to the list of possible moves of current board_state.
 * @return A vector of possible moves.
 */
void generatePawnMove(const std::array<std::array<Piece, 8>, 8>& board, int x, int y, std::vector<Move>& possible_moves);

/**
 * @brief Generates all possible moves for a given knight.
 *
 * @param board Reference of the current board state.
 * @param x The x-coordinate of the knight.
 * @param y The y-coordinate of the knight.
 * @param possible_moves Reference to the list of possible moves of current board_state.
 * @return A vector of possible moves.
 */
void generateKnightMove(const std::array<std::array<Piece, 8>, 8>& board, int x, int y, std::vector<Move>& possible_moves);

/**
 * @brief Generates all possible moves for a given bishop.
 *
 * @param board Reference of the current board state.
 * @param x The x-coordinate of the bishop.
 * @param y The y-coordinate of the bishop.
 * @param possible_moves Reference to the list of possible moves of current board_state.
 * @return A vector of possible moves.
 */
void generateBishopMove(const std::array<std::array<Piece, 8>, 8>& board, int x, int y, std::vector<Move>& possible_moves);

/**
 * @brief Generates all possible moves for a given rook.
 *
 * @param board Reference of the current board state.
 * @param x The x-coordinate of the rook.
 * @param y The y-coordinate of the rook.
 * @param possible_moves Reference to the list of possible moves of current board_state.
 * @return A vector of possible moves.
 */
void generateRookMove(const std::array<std::array<Piece, 8>, 8>& board, int x, int y, std::vector<Move>& possible_moves);

/**
 * @brief Generates all possible moves for a given queen.
 *
 * @param board Reference of the current board state.
 * @param x The x-coordinate of the queen.
 * @param y The y-coordinate of the queen.
 * @param possible_moves Reference to the list of possible moves of current board_state.
 * @return A vector of possible moves.
 */
void generateQueenMove(const std::array<std::array<Piece, 8>, 8>& board, int x, int y, std::vector<Move>& possible_moves);

/**
 * @brief Generates all possible moves for a given king.
 *
 * @param board Reference of the current board state.
 * @param x The x-coordinate of the king.
 * @param y The y-coordinate of the king.
 * @param possible_moves Reference to the list of possible moves of current board_state.
 * @return A vector of possible moves.
 */
void generateKingMove(const std::array<std::array<Piece, 8>, 8>& board, int x, int y, std::vector<Move>& possible_moves);

#endif // MOVE_GENERATOR_H