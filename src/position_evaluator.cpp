#include "position_evaluator.h"

// PUBLIC FUNCTIONS
auto PositionEvaluator::evaluate_position(BoardState &board_state) -> int {
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

// PRIVATE FUNCTIONS
void PositionEvaluator::evaluate_pawn(int x, int y, Piece &piece, int &eval) {
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
    if (!piece.moved) {
      eval -= 30;
    }
  } else {
    eval -= 150;
    if (!piece.moved) {
      eval += 30;
    }
  }

  // The more moves a knight has, the better.
  int new_x, new_y;
  for (auto &move : knight_moves) {
    new_x = x + move[0];
    new_y = y + move[1];
    if (new_x >= 0 && new_x < 8 && new_y >= 0 && new_y < 8) {
      // Increase evaluation based on the number of moves.
      if (piece.color == PieceColor::WHITE) {
        eval += 5;
      } else {
        eval -= 5;
      }
    }
  }
}

void PositionEvaluator::evaluate_bishop(int x, int y, Piece &piece, int &eval) {
  // Piece value.
  if (piece.color == PieceColor::WHITE) {
    eval += 170;
    if (!piece.moved) {
      eval -= 30;
    }
  } else {
    eval -= 170;
    if (!piece.moved) {
      eval += 30;
    }
  }

  // The more moves a bishop has, the better.
  int new_x, new_y;
  for (auto &direction : bishop_directions) {
    new_x = x + direction[0];
    new_y = y + direction[1];
    while (new_x >= 0 && new_x < 8 && new_y >= 0 && new_y < 8) {
      // Increase evaluation based on the number of moves.
      if (piece.color == PieceColor::WHITE) {
        eval += 5;
      } else {
        eval -= 5;
      }
      new_x += direction[0];
      new_y += direction[1];
    }
  }
}

void PositionEvaluator::evaluate_rook(int x, int y, Piece &piece, int &eval) {
  // Piece value.
  if (piece.color == PieceColor::WHITE) {
    eval += 400;
  } else {
    eval -= 400;
  }

  // The more moves a rook has, the better.
  int new_x, new_y;
  for (auto &direction : rook_directions) {
    new_x = x + direction[0];
    new_y = y + direction[1];
    while (new_x >= 0 && new_x < 8 && new_y >= 0 && new_y < 8) {
      // Increase evaluation based on the number of moves.
      if (piece.color == PieceColor::WHITE) {
        eval += 5;
      } else {
        eval -= 5;
      }
      new_x += direction[0];
      new_y += direction[1];
    }
  }
}

void PositionEvaluator::evaluate_queen(int x, int y, Piece &piece, int &eval) {
  // Piece value.
  if (piece.color == PieceColor::WHITE) {
    eval += 1000;
  } else {
    eval -= 1000;
  }

  // The more moves a queen has, the better.
  int new_x, new_y;
  for (auto &direction : queen_directions) {
    new_x = x + direction[0];
    new_y = y + direction[1];
    while (new_x >= 0 && new_x < 8 && new_y >= 0 && new_y < 8) {
      // Increase evaluation based on the number of moves.
      if (piece.color == PieceColor::WHITE) {
        eval += 5;
      } else {
        eval -= 5;
      }
      new_x += direction[0];
      new_y += direction[1];
    }
  }
}

void PositionEvaluator::evaluate_king(int x, int y, Piece &piece, int &eval) {
  // Piece value.
  if (piece.color == PieceColor::WHITE) {
    eval += 20000;
  } else {
    eval -= 20000;
  }
  // King is safer in the sides.
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
      eval += 8;
    } else {
      eval -= 8;
    }
    break;
  case 7:
    if (piece.color == PieceColor::WHITE) {
      eval += 5;
    } else {
      eval -= 5;
    }
    break;
  case 6:
    if (piece.color == PieceColor::WHITE) {
      eval += 10;
    } else {
      eval -= 10;
    }
    break;
  default:
    break;
  }
}
