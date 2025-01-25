#ifndef BOARD_STATE_H
#define BOARD_STATE_H

#include "piece.h"

#include <array>

class BoardState {
public:
  // 8 x 8 array to represent a chess board.
  std::array<std::array<Piece, 8>, 8> chess_board;
  // Represents who is to move.
  PieceColor move_color = PieceColor::WHITE;
  // Color to maximise for.
  PieceColor engine_color;

  /**
   * @brief Default Constructor - sets chess_board using resetBoard.
   * @param move_color Color to move with current state. White by default starts
   * the game.
   * @param engine_color Determines which color to maximise for.
   */
  BoardState(PieceColor move_color = PieceColor::WHITE,
             PieceColor engine_color = PieceColor::BLACK);

  /**
   * @brief Use if boardstate is not default chess starting state.
   * @param input_chess_board Initialises chess_board to equal
   * input_chess_board.
   * @param move_color Color to move with current state. White by default starts
   * the game.
   * @param engine_color Determines which color to maximise for.
   */
  BoardState(std::array<std::array<Piece, 8>, 8> &input_chess_board,
             PieceColor move_color = PieceColor::WHITE,
             PieceColor engine_color = PieceColor::BLACK);

  /**
   * @brief Resets chess board to default starting piece positions.
   */
  void resetBoard();
};

#endif
