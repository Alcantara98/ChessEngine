#include "move_generator.h"

//  PUBLIC FUNCTIONS
void MoveGenerator::generate_pawn_move(BoardState &board_state, int x, int y,
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
    promotion_tile = 0;
  }
  // Helper variables.
  int y_plus_pd = y + pawn_direction;
  int x_minus_1 = x - 1;
  int x_plus_1 = x + 1;

  if (y_plus_pd != promotion_tile) {
    // Normal one square move forward.
    if (board[x][y_plus_pd]->type == PieceType::EMPTY) {
      possible_moves.push_back(
          Move(x, y, x, y_plus_pd, pawn_piece, first_move));
    }
    // Normal two square move forward.
    if (first_move && board[x][y_plus_pd]->type == PieceType::EMPTY &&
        board[x][y + (2 * pawn_direction)]->type == PieceType::EMPTY) {
      possible_moves.push_back(Move(x, y, x, y + (2 * pawn_direction),
                                    pawn_piece, true, true, x,
                                    y + (2 * pawn_direction)));
    }
    // Normal capture.
    if (x > 0) {
      Piece *capture_left = board[x_minus_1][y_plus_pd];
      if (capture_left->type != PieceType::EMPTY &&
          capture_left->color != pawn_piece->color) {
        possible_moves.push_back(Move(x, y, x_minus_1, y_plus_pd, pawn_piece,
                                      capture_left, first_move));
      }
    }
    if (x < 7) {
      Piece *capture_right = board[x_plus_1][y_plus_pd];
      if (capture_right->type != PieceType::EMPTY &&
          capture_right->color != pawn_piece->color) {
        possible_moves.push_back(Move(x, y, x_plus_1, y_plus_pd, pawn_piece,
                                      capture_right, first_move));
      }
    }
    // En-passant captures.
    if ((y == 4 && pawn_piece->color == PieceColor::WHITE) ||
        (y == 3 && pawn_piece->color == PieceColor::BLACK)) {
      Move &previous_move = board_state.previous_moves.top();
      if (x > 0) {
        Piece *left_piece = board[x_minus_1][y];
        if (left_piece->type == PieceType::PAWN &&
            previous_move.pmt_x == (x_minus_1) && previous_move.pmt_y == y) {
          if (left_piece->color != pawn_piece->color &&
              left_piece->pawn_moved_two) {
            if (board[x_minus_1][y_plus_pd]->type == PieceType::EMPTY) {
              possible_moves.push_back(Move(x, y, x_minus_1, y_plus_pd,
                                            pawn_piece, left_piece, first_move,
                                            true));
            }
          }
        }
      }
      if (x < 7) {
        Piece *right_piece = board[x_plus_1][y];
        if (right_piece->type == PieceType::PAWN &&
            previous_move.pmt_x == (x_plus_1) && previous_move.pmt_y == y) {
          if (right_piece->color != pawn_piece->color &&
              right_piece->pawn_moved_two) {
            if (board[x_plus_1][y_plus_pd]->type == PieceType::EMPTY) {
              possible_moves.push_back(Move(x, y, x_plus_1, y_plus_pd,
                                            pawn_piece, right_piece, first_move,
                                            true));
            }
          }
        }
      }
    }
  }
  // Promotion moves.
  else {
    int new_x, new_y;
    // Promotion through normal one square move forward.
    if (board[x][y_plus_pd]->type == PieceType::EMPTY) {
      possible_moves.push_back(
          Move(x, y, x, y_plus_pd, pawn_piece, PieceType::QUEEN));
      possible_moves.push_back(
          Move(x, y, x, y_plus_pd, pawn_piece, PieceType::BISHOP));
      possible_moves.push_back(
          Move(x, y, x, y_plus_pd, pawn_piece, PieceType::KNIGHT));
      possible_moves.push_back(
          Move(x, y, x, y_plus_pd, pawn_piece, PieceType::ROOK));
    }
    // Promotion through capture.
    if (x > 0) { // Check if x_minus_1 is within bounds
      Piece *capture_left = board[x_minus_1][y_plus_pd];
      if (capture_left->type != PieceType::EMPTY &&
          capture_left->color != pawn_piece->color) {
        possible_moves.push_back(Move(x, y, x_minus_1, y_plus_pd, pawn_piece,
                                      capture_left, PieceType::QUEEN));
        possible_moves.push_back(Move(x, y, x_minus_1, y_plus_pd, pawn_piece,
                                      capture_left, PieceType::BISHOP));
        possible_moves.push_back(Move(x, y, x_minus_1, y_plus_pd, pawn_piece,
                                      capture_left, PieceType::KNIGHT));
        possible_moves.push_back(Move(x, y, x_minus_1, y_plus_pd, pawn_piece,
                                      capture_left, PieceType::ROOK));
      }
    }
    if (x < 7) { // Check if x_plus_1 is within bounds
      Piece *capture_right = board[x_plus_1][y_plus_pd];
      if (capture_right->type != PieceType::EMPTY &&
          capture_right->color != pawn_piece->color) {
        possible_moves.push_back(Move(x, y, x_plus_1, y_plus_pd, pawn_piece,
                                      capture_right, PieceType::QUEEN));
        possible_moves.push_back(Move(x, y, x_plus_1, y_plus_pd, pawn_piece,
                                      capture_right, PieceType::BISHOP));
        possible_moves.push_back(Move(x, y, x_plus_1, y_plus_pd, pawn_piece,
                                      capture_right, PieceType::KNIGHT));
        possible_moves.push_back(Move(x, y, x_plus_1, y_plus_pd, pawn_piece,
                                      capture_right, PieceType::ROOK));
      }
    }
  }
}

void MoveGenerator::generate_king_move(BoardState &board_state, int x, int y,
                                       std::vector<Move> &possible_moves) {
  std::array<std::array<Piece *, 8>, 8> &board = board_state.chess_board;
  Piece *king_piece = board[x][y];
  bool first_move = !king_piece->moved;

  // Helper variables.
  int x_minus_1 = x - 1;
  int x_plus_1 = x + 1;

  for (int new_x = x - 1; new_x <= x + 1; ++new_x) {
    for (int new_y = y - 1; new_y <= y + 1; ++new_y) {
      // Continue if coordinate is out of the chess board.
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
  if (first_move && !board_state.square_is_attacked(x, y, king_piece->color)) {
    // Castle king side.
    Piece *rook = board[7][y];
    if (rook->type == PieceType::ROOK && rook->moved == false) {
      if (board[x_plus_1][y]->type == PieceType::EMPTY &&
          board[x + 2][y]->type == PieceType::EMPTY) {
        if (!board_state.square_is_attacked(x_plus_1, y, king_piece->color) &&
            !board_state.square_is_attacked(x + 2, y, king_piece->color)) {
          possible_moves.push_back(
              Move(x, y, x + 2, y, king_piece, first_move, false));
        }
      }
    }
    // Castle queen side.
    rook = board[0][y];
    if (rook->type == PieceType::ROOK && rook->moved == false) {
      if (board[x_minus_1][y]->type == PieceType::EMPTY &&
          board[x - 2][y]->type == PieceType::EMPTY &&
          board[x - 3][y]->type == PieceType::EMPTY) {
        if (!board_state.square_is_attacked(x_minus_1, y, king_piece->color) &&
            !board_state.square_is_attacked(x - 2, y, king_piece->color) &&
            !board_state.square_is_attacked(x - 3, y, king_piece->color)) {
          possible_moves.push_back(
              Move(x, y, x - 2, y, king_piece, first_move, true));
        }
      }
    }
  }
}

void MoveGenerator::generate_knight_move(BoardState &board_state, int x, int y,
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
    // Continue if coordinate is out of the chess board.
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

void MoveGenerator::generate_bishop_move(BoardState &board_state, int x, int y,
                                         std::vector<Move> &possible_moves) {
  std::array<std::array<Piece *, 8>, 8> &board = board_state.chess_board;
  Piece *bishop_piece = board[x][y];
  bool first_move = !bishop_piece->moved;

  // Each respective pair of x and y directions represent a diagonal.
  rook_bishop_move_helper(board_state, x, y, 1, 1, possible_moves);
  rook_bishop_move_helper(board_state, x, y, 1, -1, possible_moves);
  rook_bishop_move_helper(board_state, x, y, -1, 1, possible_moves);
  rook_bishop_move_helper(board_state, x, y, -1, -1, possible_moves);
}

void MoveGenerator::generate_rook_move(BoardState &board_state, int x, int y,
                                       std::vector<Move> &possible_moves) {
  std::array<std::array<Piece *, 8>, 8> &board = board_state.chess_board;
  Piece *rook_piece = board[x][y];
  bool first_move = !rook_piece->moved;

  // Each respective pair of x and y directions represent horizontal or
  // vertical moves.
  rook_bishop_move_helper(board_state, x, y, 1, 0, possible_moves);
  rook_bishop_move_helper(board_state, x, y, -1, 0, possible_moves);
  rook_bishop_move_helper(board_state, x, y, 0, 1, possible_moves);
  rook_bishop_move_helper(board_state, x, y, 0, -1, possible_moves);
}

void MoveGenerator::generate_queen_move(BoardState &board_state, int x, int y,
                                        std::vector<Move> &possible_moves) {

  // Queen moves are a combination of rook and bishop moves.
  generate_rook_move(board_state, x, y, possible_moves);
  generate_bishop_move(board_state, x, y, possible_moves);
}

// PRIVATE FUNCTIONS
void MoveGenerator::rook_bishop_move_helper(BoardState &board_state, int x,
                                            int y, int x_direction,
                                            int y_direction,
                                            std::vector<Move> &possible_moves) {
  std::array<std::array<Piece *, 8>, 8> &board = board_state.chess_board;
  Piece *moving_piece = board[x][y];
  bool first_move = !moving_piece->moved;

  int new_x = x + x_direction;
  int new_y = y + y_direction;
  for (; new_x >= 0 && new_x < 8 && new_y >= 0 && new_y < 8;
       new_x += x_direction, new_y += y_direction) {
    Piece *target_piece = board[new_x][new_y];
    if (target_piece->type == PieceType::EMPTY) {
      possible_moves.push_back(
          Move(x, y, new_x, new_y, moving_piece, first_move));
    } else if (target_piece->color != moving_piece->color) {
      possible_moves.push_back(
          Move(x, y, new_x, new_y, moving_piece, target_piece, first_move));
      break;
    } else {
      break;
    }
  }
}