#include "best_move_finder.h"

BestMoveFinder::BestMoveFinder(BoardState &chess_board)
    : board_state(chess_board) {}

void BestMoveFinder::calculate_possible_moves() {
  for (int y = 0; y < 8; ++y) {
    for (int x = 0; x < 8; ++x) {
      Piece *current_piece = board_state.chess_board[x][y];
      PieceType &piece_type = current_piece->type;

      if (current_piece->color == board_state.move_color) {
        switch (piece_type) {
        case PieceType::PAWN:
          generatePawnMove(board_state, x, y, possible_moves);
          break;
        case PieceType::ROOK:
          generateRookMove(board_state, x, y, possible_moves);
          break;
        case PieceType::KNIGHT:
          generateKnightMove(board_state, x, y, possible_moves);
          break;
        case PieceType::BISHOP:
          generateBishopMove(board_state, x, y, possible_moves);
          break;
        case PieceType::QUEEN:
          generateQueenMove(board_state, x, y, possible_moves);
          break;
        case PieceType::KING:
          generateKingMove(board_state, x, y, possible_moves);
          break;
        default:
          // Empty square.
          break;
        }
      }
    }
  }
}