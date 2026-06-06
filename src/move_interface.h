#ifndef MOVE_INTERFACE_H
#define MOVE_INTERFACE_H

#include "board_state.h"

#include <string>

namespace engine::parts::move_interface
{

/**
 * @brief Converts a Move into a string in UCI format.
 *
 * @param move Move to convert into a string.
 *
 * @return Returns a string representing the move in modern chess notation.
 */
auto move_to_string(const Move &move) -> std::string;

/**
 * @brief Creates a Move object from a string in UCI format.
 *
 * @param move Move object to update based on user input.
 * @param move_string String to convert into a Move.
 * @param board_state The board state to validate the move on.
 *
 * @return Returns true if the move is valid, false otherwise.
 */
auto string_to_move(Move &move,
                    const std::string &move_string,
                    BoardState &board_state) -> bool;

/**
 * @brief Validates a move with the given board state.
 *
 * @param move Move to validate.
 * @param board_state The board state to validate the move on.
 *
 * @return Returns true if the move is valid, false otherwise.
 */
auto validate_move(Move &move, BoardState &board_state) -> bool;
} // namespace engine::parts::move_interface

#endif // MOVE_INTERFACE_H
