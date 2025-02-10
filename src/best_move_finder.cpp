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
          MoveGenerator::generate_pawn_move(board_state, x, y, possible_moves);
          break;
        case PieceType::ROOK:
          MoveGenerator::generate_rook_move(board_state, x, y, possible_moves);
          break;
        case PieceType::KNIGHT:
          MoveGenerator::generate_knight_move(board_state, x, y,
                                              possible_moves);
          break;
        case PieceType::BISHOP:
          MoveGenerator::generate_bishop_move(board_state, x, y,
                                              possible_moves);
          break;
        case PieceType::QUEEN:
          MoveGenerator::generate_queen_move(board_state, x, y, possible_moves);
          break;
        case PieceType::KING:
          MoveGenerator::generate_king_move(board_state, x, y, possible_moves);
          break;
        default:
          // Empty square.
          break;
        }
      }
    }
  }
}

int BestMoveFinder::evaluate_position() {
  int eval = 0;
  bool white_king_alive = false;
  bool black_king_alive = false;

  for (int y = 0; y < 8; ++y) {
    for (int x = 0; x < 8; ++x) {
      Piece *current_piece = board_state.chess_board[x][y];
      PieceType &piece_type = current_piece->type;

      if (current_piece->color == board_state.move_color) {
        switch (piece_type) {
        case PieceType::PAWN:
          break;
        case PieceType::ROOK:
          break;
        case PieceType::KNIGHT:
          break;
        case PieceType::BISHOP:
          break;
        case PieceType::QUEEN:
          break;
        case PieceType::KING:
          if (current_piece->color == PieceColor::WHITE) {
            white_king_alive = true;
          } else {
            black_king_alive = true;
          }
          break;
        default:
          // Empty square.
          break;
        }
      }
    }
  }

  if(!white_king_alive) {
    eval = -INF;
  } else if(!black_king_alive) {
    eval = INF;
  }
}