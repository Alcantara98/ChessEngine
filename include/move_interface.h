#ifndef MOVE_INTERFACE_H
#define MOVE_INTERFACE_H

#include "board_state.h"
#include <cmath>
#include <iostream>
#include <map>
#include <regex>
#include <string>

/**
 * @brief Namespace for move interface functions.
 */
namespace engine::parts::move_interface
{
/**
 * @brief Get user input and convert it into a Move.
 *
 * @param possible_moves List of possible moves.
 * @param move_string User input.
 * @param board_state Reference to the current board state.
 *
 * @return Returns true if move is valid, false otherwise.
 */
auto input_to_move(const std::vector<Move> &possible_moves,
                   const std::string &move_string,
                   BoardState &board_state) -> bool;

/**
 * @brief Converts a Move into a string (modern chess move notation).
 *
 * @param move Move to convert into a string.
 * @param board_state Reference to the current board state.
 *
 * @return Returns a string representing the move.
 */
auto move_to_string(const Move &move, BoardState &board_state) -> std::string;

/**
 * @brief Retrieves information from move string and updates a Move object.
 *
 * @param move Updated this Move object based on user input.
 * @param move_string String to convert into a Move.
 * @param piece_type The type of piece that is moving.
 * @param board_state Reference to the current board state.
 *
 * @return Returns true if move is valid, false otherwise.
 */
static auto create_move_from_string(Move &move,
                                    const std::string &move_string,
                                    char &piece_type,
                                    BoardState &board_state) -> bool;

/**
 * @brief Validates a move.
 *
 * @param possible_moves List of possible moves.
 * @param move Move to validate.
 * @param piece_type The type of piece that is moving.
 * @param board_state Reference to the current board state.
 *
 * @return Returns true if move is valid, false otherwise.
 */
static auto validate_move(const std::vector<Move> &possible_moves,
                          Move &move,
                          char &piece_type,
                          BoardState &board_state) -> bool;
} // namespace engine::parts::move_interface

#endif // MOVE_INTERFACE_H