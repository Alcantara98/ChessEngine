#include "board_state.h"

#include "piece.h"

BoardState::BoardState(PieceColor move_color, PieceColor engine_color)
    : move_color(move_color), engine_color(engine_color) {
  resetBoard();
}

BoardState::BoardState(std::array<std::array<Piece, 8>, 8> &input_chess_board,
                       PieceColor move_color, PieceColor engine_color)
    : chess_board(input_chess_board), move_color(move_color),
      engine_color(engine_color) {}

void BoardState::resetBoard() {
  // Set empty squares.
  for (int y = 2; y < 6; ++y) {
    for (int x = 0; x < 8; ++x) {
      chess_board[x][y] = Piece();
    }
  }
  // Set Pawns.
  for (int x = 0; x < 8; ++x) {
    chess_board[x][1] = Piece(PieceType::PAWN, PieceColor::WHITE);
  }
  for (int x = 0; x < 8; ++x) {
    chess_board[x][6] = Piece(PieceType::PAWN, PieceColor::BLACK);
  }
  // Set Rooks.
  chess_board[0][0] = Piece(PieceType::ROOK, PieceColor::WHITE);
  chess_board[7][0] = Piece(PieceType::ROOK, PieceColor::WHITE);
  chess_board[0][7] = Piece(PieceType::ROOK, PieceColor::BLACK);
  chess_board[7][7] = Piece(PieceType::ROOK, PieceColor::BLACK);
  // Set Knights.
  chess_board[1][0] = Piece(PieceType::KNIGHT, PieceColor::WHITE);
  chess_board[6][0] = Piece(PieceType::KNIGHT, PieceColor::WHITE);
  chess_board[1][7] = Piece(PieceType::KNIGHT, PieceColor::BLACK);
  chess_board[6][7] = Piece(PieceType::KNIGHT, PieceColor::BLACK);
  // Set Bishops.
  chess_board[2][0] = Piece(PieceType::BISHOP, PieceColor::WHITE);
  chess_board[5][0] = Piece(PieceType::BISHOP, PieceColor::WHITE);
  chess_board[2][7] = Piece(PieceType::BISHOP, PieceColor::BLACK);
  chess_board[5][7] = Piece(PieceType::BISHOP, PieceColor::BLACK);
  // Set Queens.
  chess_board[3][0] = Piece(PieceType::QUEEN, PieceColor::WHITE);
  chess_board[3][7] = Piece(PieceType::QUEEN, PieceColor::BLACK);
  // Set Kings.
  chess_board[4][0] = Piece(PieceType::KING, PieceColor::WHITE);
  chess_board[4][7] = Piece(PieceType::KING, PieceColor::BLACK);
}
