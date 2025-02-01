#ifndef MOVE_INTERFACE_H
#define MOVE_INTERFACE_H

#include "board_state.h"
#include "move_generator.h"
#include <string>

/**
 * @brief Use to convert command line input into a Move.
 * @param move_string Converts this string into a Move.alignas
 * @param chess_board Chess board to help conversion.
 * @return Returns the converted Move.
 */
Move string_to_move(std::string &move_string, BoardState &chess_board);

/**
 * @brief Converts a Move into a string(modern chess move notation).
 * @param move Move to convert into a string.
 * @param chess_board Chess board to help conversion.
 * @return Returns a string reprsenting the move.
 */
std::string move_to_string(Move &move, BoardState &chess_board);

#endif