#include "position_evaluator.h"

PositionEvaluator::PositionEvaluator(BoardState &board_state)
    : board_state(board_state) {}

void PositionEvaluator::evaluate_pawn(int x, int y, Piece &piece, int &eval) {
  // Piece value.

  // Position value - y coordinate.
  if (piece.color == PieceColor::WHITE) {
    eval += 50;
    eval += (y * y);
  } else {
    eval -= 50;
    eval -= ((7 - y) * (7 - y));
  }

  // Position value - x coordinate.
  switch (x) {
  case 0:
    if (piece.color == PieceColor::WHITE) {
      eval += 5;
    } else {
      eval -= 5;
    }
    break;
  case 1:
    if (piece.color == PieceColor::WHITE) {
      eval += 10;
    } else {
      eval -= 10;
    }
    break;
  case 2:
    if (piece.color == PieceColor::WHITE) {
      eval += 20;
    } else {
      eval -= 20;
    }
    break;
  case 3:
    if (piece.color == PieceColor::WHITE) {
      eval += 35;
    } else {
      eval -= 35;
    }
    break;
  case 4:
    if (piece.color == PieceColor::WHITE) {
      eval += 35;
    } else {
      eval -= 35;
    }
    break;
  case 5:
    if (piece.color == PieceColor::WHITE) {
      eval += 20;
    } else {
      eval -= 20;
    }
    break;
  case 6:
    if (piece.color == PieceColor::WHITE) {
      eval += 10;
    } else {
      eval -= 10;
    }
    break;
  case 7:
    if (piece.color == PieceColor::WHITE) {
      eval += 5;
    } else {
      eval -= 5;
    }
    break;

  default:
    break;
  }
}

void PositionEvaluator::evaluate_knight(int x, int y, Piece &piece, int &eval) {
  // Piece value.
  if (piece.color == PieceColor::WHITE) {
    eval += 150;
  } else {
    eval -= 150;
  }
}

void PositionEvaluator::evaluate_bishop(int x, int y, Piece &piece, int &eval) {
  // Piece value.
  if (piece.color == PieceColor::WHITE) {
    eval += 170;
  } else {
    eval -= 170;
  }
}

void PositionEvaluator::evaluate_rook(int x, int y, Piece &piece, int &eval) {
  // Piece value.
  if (piece.color == PieceColor::WHITE) {
    eval += 500;
  } else {
    eval -= 500;
  }
}

void PositionEvaluator::evaluate_queen(int x, int y, Piece &piece, int &eval) {
  // Piece value.
  if (piece.color == PieceColor::WHITE) {
    eval += 1000;
  } else {
    eval -= 1000;
  }
}

void PositionEvaluator::evaluate_king(int x, int y, Piece &piece, int &eval) {
  // Piece value.
  if (piece.color == PieceColor::WHITE) {
    eval += 20000;
  } else {
    eval -= 20000;
  }
}

int PositionEvaluator::evaluate_position() {
  int eval = 0;

  for (int y = 0; y < 8; ++y) {
    for (int x = 0; x < 8; ++x) {
      Piece &piece = *board_state.chess_board[x][y];
      PieceType &piece_type = piece.type;

      switch (piece_type) {
      case PieceType::PAWN:
        evaluate_pawn(x, y, piece, eval);
        break;
      case PieceType::ROOK:
        evaluate_rook(x, y, piece, eval);
        break;
      case PieceType::KNIGHT:
        evaluate_knight(x, y, piece, eval);
        break;
      case PieceType::BISHOP:
        evaluate_bishop(x, y, piece, eval);
        break;
      case PieceType::QUEEN:
        evaluate_queen(x, y, piece, eval);
        break;
      case PieceType::KING:
        evaluate_king(x, y, piece, eval);
        break;
      default:
        // Empty square.
        break;
      }
    }
  }
  return eval;
}
