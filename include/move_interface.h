#ifndef MOVE_INTERFACE_H
#define MOVE_INTERFACE_H

#include "board_state.h"
#include "move_generator.h"
#include <iostream>
#include <map>
#include <regex>
#include <string>

/**
 * @brief Class to convert user input into a Move and vice versa.
 */
class MoveInterface {
private:
  // Map to convert algebraic coordinates to int.
  const std::map<char, int> algebraic_to_int = {{'a', 0}, {'b', 1}, {'c', 2},
                                                {'d', 3}, {'e', 4}, {'f', 5},
                                                {'g', 6}, {'h', 7}};

  // Map to convert int to algebraic coordinates.
  const std::map<int, char> int_to_algebraic = {{0, 'a'}, {1, 'b'}, {2, 'c'},
                                                {3, 'd'}, {4, 'e'}, {5, 'f'},
                                                {6, 'g'}, {7, 'h'}};

  // Map to convert string piece type to PieceType.
  const std::map<char, PieceType> string_to_piece_type = {
      {'k', PieceType::KING},   {'q', PieceType::QUEEN},
      {'r', PieceType::ROOK},   {'b', PieceType::BISHOP},
      {'n', PieceType::KNIGHT}, {'p', PieceType::PAWN}};

  // Map to convert PieceType to string piece type.
  const std::map<PieceType, char> piece_type_to_string = {
      {PieceType::KING, 'k'},   {PieceType::QUEEN, 'q'},
      {PieceType::ROOK, 'r'},   {PieceType::BISHOP, 'b'},
      {PieceType::KNIGHT, 'n'}, {PieceType::PAWN, 'p'}};

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
  auto input_to_move(std::vector<Move> possible_moves,
                     std::string string_move = "") -> Move;

  /**
   * @brief Converts a Move into a string(modern chess move notation).
   * @param move Move to convert into a string.
   * @param board_state Chess board to help conversion.
   * @return Returns a string reprsenting the move.
   */
  auto move_to_string(Move &move) -> std::string;
};

#endif