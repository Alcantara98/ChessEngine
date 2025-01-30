#include "best_move_finder.h"

BestMoveFinder::BestMoveFinder(BoardState chess_board)
    : board_state(chess_board) {}

void BestMoveFinder::calculate_possible_moves() {
  std::array<std::array<Piece, 8>, 8> &chess_board = board_state.chess_board;

  for (int y = 0; y < 8; ++y) {
    for (int x = 0; x < 8; ++x) {
      Piece &current_piece = chess_board[x][y];
      PieceType &piece_type = current_piece.type;

      if (current_piece.color == board_state.move_color) {
        switch (piece_type) {
        case PieceType::PAWN:
          generatePawnMove(chess_board, x, y, possible_moves);
          break;
        case PieceType::ROOK:
          generateRookMove(chess_board, x, y, possible_moves);
          break;
        case PieceType::KNIGHT:
          generateKnightMove(chess_board, x, y, possible_moves);
          break;
        case PieceType::BISHOP:
          generateBishopMove(chess_board, x, y, possible_moves);
          break;
        case PieceType::QUEEN:
          generateQueenMove(chess_board, x, y, possible_moves);
          break;
        case PieceType::KING:
          generateKingMove(chess_board, x, y, possible_moves);
          break;
        default:
          // Empty square.
          break;
        }
      }
    }
  }
}

void BestMoveFinder::apply_move(Move &move) {
  std::array<std::array<Piece, 8>, 8> &chess_board = board_state.chess_board;

  if (move.captured_piece.type != PieceType::EMPTY) {
    // Implement
  } else if (move.is_en_passant) {
    // Implement
  } else if (move.pawn_moved_two) {
    // Implement
  } else if (move.promotion_piece_type != PieceType::EMPTY) {
    // Implement
  } else if (move.moving_piece.type == PieceType::KING) {
    switch (move.to_x - move.from_x) {
    case 2:
      // King Side Castle.
      move.moving_piece.moved = true;
      chess_board[7][move.to_y].moved = true;
      chess_board[move.from_x][move.from_y] = board_state.empty_square;
      chess_board[move.to_x][move.to_y] = move.moving_piece;
      chess_board[5][move.to_y] = chess_board[7][move.to_y];
      chess_board[7][move.to_y] = board_state.empty_square;
      break;
    case -2:
      // Queen Side Castle.
      move.moving_piece.moved = true;
      chess_board[0][move.to_y].moved = true;
      chess_board[move.from_x][move.from_y] = board_state.empty_square;
      chess_board[move.to_x][move.to_y] = move.moving_piece;
      chess_board[3][move.to_y] = chess_board[0][move.to_y];
      chess_board[0][move.to_y] = board_state.empty_square;
      break;
    default:
      break;
    }
  }
}