#ifndef MOVE_INTERFACE_H
#define MOVE_INTERFACE_H

#include "board_state.h"
#include "move_generator.h"
#include <iostream>
#include <map>
#include <regex>
#include <string>


class MoveInterface {
private:
  // Map to convert algebraic coordinates to int.
  const std::map<char, int> algebraic_to_int = {{'a', 1}, {'b', 1}, {'c', 1},
                                                {'d', 1}, {'e', 1}, {'f', 1},
                                                {'g', 1}, {'h', 1}};
  // Reference of all possible moves;
  std::vector<Move> &possible_moves;

  // Reference of the baord state;
  BoardState &board_state;

public:
  /**
   * @brief User to Engine move interface.
   */
  MoveInterface(BoardState &board_state, std::vector<Move> &possible_moves);

  /**
   * @brief Use to convert command line input into a Move.
   * @return Returns the converted Move.
   */
  Move input_to_move();

  /**
   * @brief Converts a Move into a string(modern chess move notation).
   * @param move Move to convert into a string.
   * @param board_state Chess board to help conversion.
   * @return Returns a string reprsenting the move.
   */
  std::string move_to_string(Move &move);
};

#endif