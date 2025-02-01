#include "move_generator.h"

Move::Move(int from_x, int from_y, int to_x, int to_y, Piece *moving_piece,
           bool first_move, bool pawn_moved_two, int pmt_x, int pmt_y)
    : from_x(from_x), from_y(from_y), to_x(to_x), to_y(to_y),
      moving_piece(moving_piece), captured_piece(nullptr),
      promotion_piece_type(PieceType::EMPTY), is_en_passant(false),
      first_move(first_move), pawn_moved_two(pawn_moved_two), pmt_x(pmt_x),
      pmt_y(pmt_y) {}

Move::Move(int from_x, int from_y, int to_x, int to_y, Piece *moving_piece,
           PieceType promotion_piece_type)
    : from_x(from_x), from_y(from_y), to_x(to_x), to_y(to_y),
      moving_piece(moving_piece), captured_piece(nullptr),
      promotion_piece_type(promotion_piece_type), is_en_passant(false),
      first_move(false), pawn_moved_two(false) {}

Move::Move(int from_x, int from_y, int to_x, int to_y, Piece *moving_piece,
           Piece *captured_piece, bool first_move, bool is_en_passant)
    : from_x(from_x), from_y(from_y), to_x(to_x), to_y(to_y),
      moving_piece(moving_piece), captured_piece(captured_piece),
      promotion_piece_type(PieceType::EMPTY), is_en_passant(is_en_passant),
      first_move(first_move), pawn_moved_two(false) {}

Move::Move(int from_x, int from_y, int to_x, int to_y, Piece *moving_piece,
           Piece *captured_piece, PieceType promotion_piece_type)
    : from_x(from_x), from_y(from_y), to_x(to_x), to_y(to_y),
      moving_piece(moving_piece), captured_piece(captured_piece),
      promotion_piece_type(promotion_piece_type), is_en_passant(false),
      first_move(false), pawn_moved_two(false) {}

void generatePawnMove(BoardState &board_state, int x, int y,
                      std::vector<Move> &possible_moves) {
  std::array<std::array<Piece *, 8>, 8> &board = board_state.chess_board;
  Piece *pawn_piece = board[x][y];
  bool first_move = !pawn_piece->moved;

  // Create pawn moves.
  int pawn_direction, promotion_tile;
  if (pawn_piece->color == PieceColor::WHITE) {
    pawn_direction = 1;
    promotion_tile = 7;
  } else {
    pawn_direction = -1;
    promotion_tile = 1;
  }
  if (y != promotion_tile || y != promotion_tile + pawn_direction) {
    // Normal one square move forward.
    if (board[x][y + pawn_direction]->type == PieceType::EMPTY) {
      possible_moves.push_back(
          Move(x, y, x, y + pawn_direction, pawn_piece, first_move));
    }
    // Normal two square move forward.
    if (board[x][y + pawn_direction]->type == PieceType::EMPTY &&
        board[x][y + (2 * pawn_direction)]->type == PieceType::EMPTY &&
        first_move) {
      possible_moves.push_back(Move(x, y, x, y + (2 * pawn_direction),
                                    pawn_piece, true, true, x,
                                    y + (2 * pawn_direction)));
    }
    // Normal capture.
    if (x > 0) {
      Piece *capture_left = board[x - 1][y + pawn_direction];
      if (capture_left->type != PieceType::EMPTY &&
          capture_left->color != pawn_piece->color) {
        possible_moves.push_back(Move(x, y, x - 1, y + pawn_direction,
                                      pawn_piece, capture_left, first_move));
      }
    }
    if (x < 7) {
      Piece *capture_right = board[x + 1][y + pawn_direction];
      if (capture_right->type != PieceType::EMPTY &&
          capture_right->color != pawn_piece->color) {
        possible_moves.push_back(Move(x, y, x + 1, y + pawn_direction,
                                      pawn_piece, capture_right, first_move));
      }
    }
    // En-passant captures.
    if ((y == 4 && pawn_piece->color == PieceColor::WHITE) ||
        (y == 3 && pawn_piece->color == PieceColor::BLACK)) {
      Move &previous_move = board_state.previous_moves.top();
      if (x > 0) {
        Piece *left_piece = board[x - 1][y];
        if (left_piece->type == PieceType::PAWN &&
            previous_move.pmt_x == (x - 1) && previous_move.pmt_y == y) {
          if (left_piece->color != pawn_piece->color &&
              left_piece->pawn_moved_two) {
            if (board[x - 1][y + pawn_direction]->type == PieceType::EMPTY) {
              possible_moves.push_back(Move(x, y, x - 1, y + pawn_direction,
                                            pawn_piece, left_piece, first_move,
                                            true));
            }
          }
        }
      }
      if (x < 7) {
        Piece *right_piece = board[x + 1][y];
        if (right_piece->type == PieceType::PAWN &&
            previous_move.pmt_x == (x + 1) && previous_move.pmt_y == y) {
          if (right_piece->color != pawn_piece->color &&
              right_piece->pawn_moved_two) {
            if (board[x + 1][y + pawn_direction]->type == PieceType::EMPTY) {
              possible_moves.push_back(Move(x, y, x - 1, y + pawn_direction,
                                            pawn_piece, right_piece, first_move,
                                            true));
            }
          }
        }
      }
    }
  }
  // Promotion moves.
  else if (y == promotion_tile) {
    int new_x, new_y;
    // Promotion through normal one square move forward.
    if (board[x][y + pawn_direction]->type == PieceType::EMPTY) {
      possible_moves.push_back(
          Move(x, y, x, y + pawn_direction, pawn_piece, PieceType::QUEEN));
      possible_moves.push_back(
          Move(x, y, x, y + pawn_direction, pawn_piece, PieceType::BISHOP));
      possible_moves.push_back(
          Move(x, y, x, y + pawn_direction, pawn_piece, PieceType::KNIGHT));
      possible_moves.push_back(
          Move(x, y, x, y + pawn_direction, pawn_piece, PieceType::ROOK));
    }
    // Promotion through capture.
    Piece *capture_left = board[x - 1][y + pawn_direction];
    Piece *capture_right = board[x + 1][y + pawn_direction];
    if (capture_left->type != PieceType::EMPTY &&
        capture_left->color != pawn_piece->color) {
      possible_moves.push_back(Move(x, y, x - 1, y + pawn_direction, pawn_piece,
                                    capture_left, PieceType::QUEEN));
      possible_moves.push_back(Move(x, y, x - 1, y + pawn_direction, pawn_piece,
                                    capture_left, PieceType::BISHOP));
      possible_moves.push_back(Move(x, y, x - 1, y + pawn_direction, pawn_piece,
                                    capture_left, PieceType::KNIGHT));
      possible_moves.push_back(Move(x, y, x - 1, y + pawn_direction, pawn_piece,
                                    capture_left, PieceType::ROOK));
    }
    if (capture_right->type != PieceType::EMPTY &&
        capture_right->color != pawn_piece->color) {
      possible_moves.push_back(Move(x, y, x + 1, y + pawn_direction, pawn_piece,
                                    capture_right, PieceType::QUEEN));
      possible_moves.push_back(Move(x, y, x + 1, y + pawn_direction, pawn_piece,
                                    capture_right, PieceType::BISHOP));
      possible_moves.push_back(Move(x, y, x + 1, y + pawn_direction, pawn_piece,
                                    capture_right, PieceType::KNIGHT));
      possible_moves.push_back(Move(x, y, x + 1, y + pawn_direction, pawn_piece,
                                    capture_right, PieceType::ROOK));
    }
  }
}

void generateKingMove(BoardState &board_state, int x, int y,
                      std::vector<Move> &possible_moves) {
  std::array<std::array<Piece *, 8>, 8> &board = board_state.chess_board;
  Piece *king_piece = board[x][y];
  bool first_move = !king_piece->moved;

  for (int new_x = x - 1; new_x <= x + 1; ++new_x) {
    for (int new_y = y - 1; new_y <= y + 1; ++new_y) {
      // Continue if coordinate is out of the chess baord.
      if (new_x < 0 || new_x > 7 || new_y < 0 || new_y > 7) {
        continue;
      }
      // Continue if the position is the kings current position.
      if (new_x == x && new_y == y) {
        continue;
      }
      Piece *target_piece = board[new_x][new_y];
      // Normal move.
      if (target_piece->type == PieceType::EMPTY) {
        possible_moves.push_back(
            Move(x, y, new_x, new_y, king_piece, first_move));
      }
      // Capture move.
      else if (target_piece->color != king_piece->color) {
        possible_moves.push_back(
            Move(x, y, new_x, new_y, king_piece, target_piece, first_move));
      }
    }
  }
  // Castle Moves
  if (first_move) {
    // Castle king side.
    if (board[x + 1][y]->type == PieceType::EMPTY &&
        board[x + 2][y]->type == PieceType::EMPTY) {
      Piece *rook = board[7][y];
      if (rook->type == PieceType::ROOK && rook->moved == false) {
        possible_moves.push_back(
            Move(x, y, x + 2, y, king_piece, first_move, true));
      }
    }
    // Castle queen side.
    if (board[x - 1][y]->type == PieceType::EMPTY &&
        board[x - 2][y]->type == PieceType::EMPTY &&
        board[x - 3][y]->type == PieceType::EMPTY) {
      Piece *rook = board[0][y];
      if (rook->type == PieceType::ROOK && rook->moved == false) {
        possible_moves.push_back(
            Move(x, y, x - 2, y, king_piece, first_move, true));
      }
    }
  }
}

void generateKnightMove(BoardState &board_state, int x, int y,
                        std::vector<Move> &possible_moves) {
  std::array<std::array<Piece *, 8>, 8> &board = board_state.chess_board;
  Piece *knight_piece = board[x][y];
  bool first_move = !knight_piece->moved;

  std::vector<int> x_pos_list = {x - 2, x - 2, x - 1, x - 1,
                                 x + 1, x + 1, x + 2, x + 2};
  std::vector<int> y_pos_list = {y - 1, y + 1, y - 2, y + 2,
                                 y - 2, y + 2, y - 1, y + 1};
  for (int index = 0; index < 8; ++index) {
    int new_x = x_pos_list[index];
    int new_y = y_pos_list[index];
    // Continue if coordinate is out of the chess baord.
    if (new_x < 0 || new_x > 7 || new_y < 0 || new_y > 7) {
      continue;
    }
    Piece *target_piece = board[new_x][new_y];
    // Normal move.
    if (target_piece->type == PieceType::EMPTY) {
      possible_moves.push_back(
          Move(x, y, new_x, new_y, knight_piece, first_move));
    }
    // Capture move.
    else if (target_piece->color != knight_piece->color) {
      possible_moves.push_back(
          Move(x, y, new_x, new_y, knight_piece, target_piece, first_move));
    }
  }
}

void generateBishopMove(BoardState &board_state, int x, int y,
                        std::vector<Move> &possible_moves) {
  std::array<std::array<Piece *, 8>, 8> &board = board_state.chess_board;
  Piece *bishop_piece = board[x][y];
  bool first_move = !bishop_piece->moved;

  // Bishop can go four directions diagonally from its current position.
  // Calculate using for loop for each direction.
  int new_x, new_y;
  new_x = x + 1;
  new_y = y + 1;
  for (; new_x < 8 && new_y < 8; ++new_x, ++new_y) {
    Piece *target_piece = board[new_x][new_y];
    if (target_piece->type == PieceType::EMPTY) {
      possible_moves.push_back(
          Move(x, y, new_x, new_y, bishop_piece, first_move));
    } else if (target_piece->color != bishop_piece->color) {
      possible_moves.push_back(
          Move(x, y, new_x, new_y, bishop_piece, target_piece, first_move));
      break;
    } else {
      break;
    }
  }
  new_x = x - 1;
  new_y = y - 1;
  for (; new_x >= 0 && new_y >= 0; --new_x, --new_y) {
    Piece *target_piece = board[new_x][new_y];
    if (target_piece->type == PieceType::EMPTY) {
      possible_moves.push_back(
          Move(x, y, new_x, new_y, bishop_piece, first_move));
    } else if (target_piece->color != bishop_piece->color) {
      possible_moves.push_back(
          Move(x, y, new_x, new_y, bishop_piece, target_piece, first_move));
      break;
    } else {
      break;
    }
  }
  new_x = x + 1;
  new_y = y - 1;
  for (; new_x < 8 && new_y >= 0; ++new_x, --new_y) {
    Piece *target_piece = board[new_x][new_y];
    if (target_piece->type == PieceType::EMPTY) {
      possible_moves.push_back(
          Move(x, y, new_x, new_y, bishop_piece, first_move));
    } else if (target_piece->color != bishop_piece->color) {
      possible_moves.push_back(
          Move(x, y, new_x, new_y, bishop_piece, target_piece, first_move));
      break;
    } else {
      break;
    }
  }
  new_x = x - 1;
  new_y = y + 1;
  for (; new_x >= 0 && new_y < 8; --new_x, ++new_y) {
    Piece *target_piece = board[new_x][new_y];
    if (target_piece->type == PieceType::EMPTY) {
      possible_moves.push_back(
          Move(x, y, new_x, new_y, bishop_piece, first_move));
    } else if (target_piece->color != bishop_piece->color) {
      possible_moves.push_back(
          Move(x, y, new_x, new_y, bishop_piece, target_piece, first_move));
      break;
    } else {
      break;
    }
  }
}

void generateRookMove(BoardState &board_state, int x, int y,
                      std::vector<Move> &possible_moves) {
  std::array<std::array<Piece *, 8>, 8> &board = board_state.chess_board;
  Piece *rook_piece = board[x][y];
  bool first_move = !rook_piece->moved;

  // Bishop can go four directions diagonally from its current position.
  // Calculate ussing forloop for each direction.
  int new_x, new_y;
  new_x = x + 1;
  for (; new_x < 8; ++new_x) {
    Piece *target_piece = board[new_x][y];
    if (target_piece->type == PieceType::EMPTY) {
      possible_moves.push_back(Move(x, y, new_x, y, rook_piece, first_move));
    } else if (target_piece->color != rook_piece->color) {
      possible_moves.push_back(
          Move(x, y, new_x, y, rook_piece, target_piece, first_move));
      break;
    } else {
      break;
    }
  }
  new_x = x - 1;
  for (; new_x >= 0; --new_x) {
    Piece *target_piece = board[new_x][y];
    if (target_piece->type == PieceType::EMPTY) {
      possible_moves.push_back(Move(x, y, new_x, y, rook_piece, first_move));
    } else if (target_piece->color != rook_piece->color) {
      possible_moves.push_back(
          Move(x, y, new_x, y, rook_piece, target_piece, first_move));
      break;
    } else {
      break;
    }
  }
  new_y = y - 1;
  for (; new_y >= 0; --new_y) {
    Piece *target_piece = board[x][new_y];
    if (target_piece->type == PieceType::EMPTY) {
      possible_moves.push_back(Move(x, y, x, new_y, rook_piece, first_move));
    } else if (target_piece->color != rook_piece->color) {
      possible_moves.push_back(
          Move(x, y, x, new_y, rook_piece, target_piece, first_move));
      break;
    } else {
      break;
    }
  }
  new_y = y + 1;
  for (; new_y < 8; ++new_y) {
    Piece *target_piece = board[x][new_y];
    if (target_piece->type == PieceType::EMPTY) {
      possible_moves.push_back(Move(x, y, x, new_y, rook_piece, first_move));
    } else if (target_piece->color != rook_piece->color) {
      possible_moves.push_back(
          Move(x, y, x, new_y, rook_piece, target_piece, first_move));
      break;
    } else {
      break;
    }
  }
}

void generateQueenMove(BoardState &board_state, int x, int y,
                       std::vector<Move> &possible_moves) {
  generateRookMove(board_state, x, y, possible_moves);
  generateBishopMove(board_state, x, y, possible_moves);
}