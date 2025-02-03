#include "best_move_finder.h"

BestMoveFinder::BestMoveFinder(BoardState &board_state)
    : board_state(board_state) {}

void BestMoveFinder::calculate_possible_moves() {
  for (int y = 0; y < 8; ++y) {
    for (int x = 0; x < 8; ++x) {
      Piece *current_piece = board_state.chess_board[x][y];
      PieceType &piece_type = current_piece->type;

      if (current_piece->color == board_state.move_color) {
        switch (piece_type) {
        case PieceType::PAWN:
          MoveGenerator::generatePawnMove(board_state, x, y, possible_moves);
          break;
        case PieceType::ROOK:
          MoveGenerator::generateRookMove(board_state, x, y, possible_moves);
          break;
        case PieceType::KNIGHT:
          MoveGenerator::generateKnightMove(board_state, x, y, possible_moves);
          break;
        case PieceType::BISHOP:
          MoveGenerator::generateBishopMove(board_state, x, y, possible_moves);
          break;
        case PieceType::QUEEN:
          MoveGenerator::generateQueenMove(board_state, x, y, possible_moves);
          break;
        case PieceType::KING:
          MoveGenerator::generateKingMove(board_state, x, y, possible_moves);
          break;
        default:
          // Empty square.
          break;
        }
      }
    }
  }
}