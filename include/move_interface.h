#ifndef MOVE_INTERFACE_H
#define MOVE_INTERFACE_H

#include "board_state.h"
#include "move_generator.h"
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
private:
  // Map to convert algebraic coordinates to int.
  const std::map<char, int> algebraic_to_int = {{'a', 0}, {'b', 1}, {'c', 2},
                                                {'d', 3}, {'e', 4}, {'f', 5},
                                                {'g', 6}, {'h', 7}};

  // Map to convert int to algebraic coordinates.
  const std::map<int, char> int_to_algebraic = {{0, 'a'}, {1, 'b'}, {2, 'c'},
                                                {3, 'd'}, {4, 'e'}, {5, 'f'},
                                                {6, 'g'}, {7, 'h'}};

  // Reference of the board state;
  BoardState &game_board_state;

public:
  /**
   * @brief User to Engine move interface.
   */
  MoveInterface(BoardState &board_state);

  /**
   * @brief Use to convert command line input into a Move.
   *
   * @return Returns the converted Move.
   */
  auto input_to_move(const std::vector<Move> &possible_moves,
                     std::string string_move = "") -> Move;

  /**
   * @brief Converts a Move into a string(modern chess move notation).
   * @param move Move to convert into a string.
   * @param board_state Chess board to help conversion.
   * @return Returns a string reprsenting the move.
   */
  auto move_to_string(Move &move) -> std::string;
};
} // namespace engine::parts

#endif