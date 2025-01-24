#ifndef BOARD_STATE_H
#define BOARD_STATE_H

#include "piece.h"

#include <array>

class BoardState {
public:
  // 8 x 8 array to represent a chess board.
  std::array<std::array<Piece, 8>, 8> chess_board;
  // Indicates whether the white or black king is under check.
  bool w_king_in_check, b_king_in_check;
  // Indicates the position off both kings; required to check if a move exposes
  // king, making the move invalid.
  int w_king_x, w_king_y, b_king_x, b_king_y;

  /**
   * @brief Default Constructor - sets chess_board using resetBoard.
   */
  BoardState();

  /**
   * @brief Parameterized constructor.
   * @param input_chess_board Initialises chess_baord to equal
   * input_chess_board.
   */
  BoardState(std::array<std::array<Piece, 8>, 8> &input_chess_board);

  /**
   * @brief Resets chess board to default starting piece positions.
   */
  void resetBoard();
};

#endif
