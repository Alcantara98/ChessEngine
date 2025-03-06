#ifndef MOVE_INTERFACE_H
#define MOVE_INTERFACE_H

#include "board_state.h"
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
  // CONSTRUCTORS
  /**
   * @brief User to Engine move interface.
   *
   * @param board_state Reference of the board state.
   */
  MoveInterface(BoardState &board_state);

  // FUNCTIONS
  /**
   * @brief Get user input and convert it into a Move.
   *
   * @param possible_moves List of possible moves.
   * @param move_string User input.
   *
   * @return Returns true if move is valid, false otherwise.
   */
  auto input_to_move(const std::vector<Move> &possible_moves,
                     const std::string &move_string) -> bool;

  /**
   * @brief Converts a Move into a string(modern chess move notation).
   *
   * @param move Move to convert into a string.
   *
   * @return Returns a string reprsenting the move.
   */
  auto move_to_string(Move &move) -> std::string;

private:
  // PROPERTIES
  // Reference of the board state;
  BoardState &game_board_state;

  // FUNCTIONS
  /**
   * @brief Retrieves information from move string and updates a Move object.
   *
   * @param move_from_input Updated Move object based on user input.
   * @param move_string String to convert into a Move.
   * @param piece_type The type of piece that is moving.
   *
   * @return Returns true if move is valid, false otherwise.
   */
  auto create_move_from_string(std::unique_ptr<Move> &move_from_input,
                               const std::string &move_string,
                               char &piece_type) -> bool;

  /**
   * @brief Validates a move.
   *
   * @param possible_moves List of possible moves.
   * @param move Move to validate.
   * @param piece_type The type of piece that is moving.
   *
   * @return Returns true if move is valid, false otherwise.
   */
  auto validate_move(const std::vector<Move> &possible_moves, Move *move,
                     char &piece_type) -> bool;
};
} // namespace engine::parts

#endif // MOVE_INTERFACE_H