#ifndef MOVE_INTERFACE_H
#define MOVE_INTERFACE_H

#include "board_state.h"
#include "move_generator.h"
#include <cmath>
#include <iostream>
#include <map>
#include <regex>
#include <string>

namespace engine::parts
{
/**
 * @brief Class to convert user input into a Move and vice versa.
 */
class MoveInterface
{
public:
  /**
   * @brief User to Engine move interface.
   */
  MoveInterface(BoardState &board_state);

  /**
   * @brief Get user input and convert it into a Move.
   *
   * @return Returns the converted Move.
   */
  auto input_to_move(const std::vector<Move> &possible_moves,
                     std::string string_move = "") -> Move;

  /**
   * @brief Converts a Move into a string(modern chess move notation).
   *
   * @param move Move to convert into a string.
   * @param board_state Chess board to help conversion.
   * @return Returns a string reprsenting the move.
   */
  auto move_to_string(Move &move) -> std::string;

  /**
   * @brief Retrieves information from move string and updates a Move object.
   *
   * @param move_from_input Updated Move object based on user input.
   * @param move_string String to convert into a Move.
   */
  auto parse_string_move(std::unique_ptr<Move> &move_from_input,
                         const std::string &move_string,
                         char &piece_type) -> bool;

private:
  // Reference of the board state;
  BoardState &game_board_state;
};
} // namespace engine::parts

#endif