#include "board_state.h"

BoardState::BoardState(PieceColor move_color, PieceColor engine_color)
    : move_color(move_color), engine_color(engine_color) {
  reset_board();
}

BoardState::BoardState(std::array<std::array<Piece *, 8>, 8> &input_chess_board,
                       PieceColor move_color, PieceColor engine_color)
    : chess_board(input_chess_board), move_color(move_color),
      engine_color(engine_color) {}

void BoardState::reset_board() {
  // Set empty squares.
  for (int y = 2; y < 6; ++y) {
    for (int x = 0; x < 8; ++x) {
      chess_board[x][y] = &empty_piece;
    }
  }
  // Set Pawns.
  for (int x = 0; x < 8; ++x) {
    chess_board[x][1] = new Piece(PieceType::PAWN, PieceColor::WHITE);
  }
  for (int x = 0; x < 8; ++x) {
    chess_board[x][6] = new Piece(PieceType::PAWN, PieceColor::BLACK);
  }
  // Set Rooks.
  chess_board[0][0] = new Piece(PieceType::ROOK, PieceColor::WHITE);
  chess_board[7][0] = new Piece(PieceType::ROOK, PieceColor::WHITE);
  chess_board[0][7] = new Piece(PieceType::ROOK, PieceColor::BLACK);
  chess_board[7][7] = new Piece(PieceType::ROOK, PieceColor::BLACK);
  // Set Knights.
  chess_board[1][0] = new Piece(PieceType::KNIGHT, PieceColor::WHITE);
  chess_board[6][0] = new Piece(PieceType::KNIGHT, PieceColor::WHITE);
  chess_board[1][7] = new Piece(PieceType::KNIGHT, PieceColor::BLACK);
  chess_board[6][7] = new Piece(PieceType::KNIGHT, PieceColor::BLACK);
  // Set Bishops.
  chess_board[2][0] = new Piece(PieceType::BISHOP, PieceColor::WHITE);
  chess_board[5][0] = new Piece(PieceType::BISHOP, PieceColor::WHITE);
  chess_board[2][7] = new Piece(PieceType::BISHOP, PieceColor::BLACK);
  chess_board[5][7] = new Piece(PieceType::BISHOP, PieceColor::BLACK);
  // Set Queens.
  chess_board[3][0] = new Piece(PieceType::QUEEN, PieceColor::WHITE);
  chess_board[3][7] = new Piece(PieceType::QUEEN, PieceColor::BLACK);
  // Set Kings.
  chess_board[4][0] = new Piece(PieceType::KING, PieceColor::WHITE);
  chess_board[4][7] = new Piece(PieceType::KING, PieceColor::BLACK);
}

void BoardState::print_board() {
  for (int y = 7; y >= 0; --y) {
    for (int x = 0; x < 8; ++x) {
      Piece *piece = chess_board[x][y];
      char piece_char = (piece->color == PieceColor::WHITE)
                            ? w_piece_to_char.at(piece->type)
                            : b_piece_to_char.at(piece->type);
      printf("%c ", piece_char);
    }
    printf("\n");
  }
}

void BoardState::apply_move(Move &move) {
  if (move.pawn_moved_two) {
    move.moving_piece->pawn_moved_two = true;
  }

  if (move.is_en_passant) {
    // Remove captured pawn.
    int captured_y_pos = (move.moving_piece->color == PieceColor::WHITE)
                             ? move.to_y - 1
                             : move.to_y + 1;
    chess_board[move.to_x][captured_y_pos] = &empty_piece;
  } else if (move.moving_piece->type == PieceType::KING) {
    // Move Rook.
    switch (move.to_x - move.from_x) {
    case 2:
      // King Side Castle.
      chess_board[7][move.to_y]->moved = true;
      std::swap(chess_board[5][move.to_y], chess_board[7][move.to_y]);
      break;
    case -2:
      // Queen Side Castle.
      chess_board[0][move.to_y]->moved = true;
      std::swap(chess_board[0][move.to_y], chess_board[3][move.to_y]);
      break;
    default:
      break;
    }
  }

  std::swap(chess_board[move.from_x][move.from_y],
            chess_board[move.to_x][move.to_y]);

  if (move.promotion_piece_type != PieceType::EMPTY) {
    // If pawn is promoting, moving piece will become promotion piece.
    chess_board[move.to_x][move.to_y] =
        new Piece(move.promotion_piece_type, move.moving_piece->color);
  }

  if (move.captured_piece != nullptr && move.is_en_passant == false) {
    // If capturing, original square will not be empty after swap.
    // Empty here.
    chess_board[move.from_x][move.from_y] = &empty_piece;
  }

  if (move.first_move) {
    move.moving_piece->moved = true;
  }

  move_color =
      (move_color == PieceColor::WHITE) ? PieceColor::BLACK : PieceColor::WHITE;

  previous_moves.push(move);
}

void BoardState::undo_move() {
  Move &move = previous_moves.top();
  if (move.pawn_moved_two) {
    move.moving_piece->pawn_moved_two = false;
  }

  if (move.is_en_passant) {
    // Add captured pawn.
    int captured_y_pos = (move.moving_piece->color == PieceColor::WHITE)
                             ? move.to_y - 1
                             : move.to_y + 1;
    chess_board[move.to_x][captured_y_pos] = move.captured_piece;
  } else if (move.moving_piece->type == PieceType::KING) {
    // Move Rook.
    switch (move.to_x - move.from_x) {
    case 2:
      // King Side Castle.
      std::swap(chess_board[5][move.to_y], chess_board[7][move.to_y]);
      chess_board[7][move.to_y]->moved = false;
      break;
    case -2:
      // Queen Side Castle.
      std::swap(chess_board[0][move.to_y], chess_board[3][move.to_y]);
      chess_board[0][move.to_y]->moved = false;
      break;
    default:
      break;
    }
  }

  if (move.promotion_piece_type != PieceType::EMPTY) {
    // If pawn was promoted, pawn piece is replaced.
    // Add back the pawn piece, and delete the promotion piece.
    delete chess_board[move.to_x][move.to_y];
    chess_board[move.to_x][move.to_y] = move.moving_piece;
  }

  if (move.captured_piece != nullptr && move.is_en_passant == false) {
    // If a piece was captured, add the piece back.
    chess_board[move.from_x][move.from_y] = move.captured_piece;
  }

  std::swap(chess_board[move.from_x][move.from_y],
            chess_board[move.to_x][move.to_y]);

  if (move.first_move) {
    move.moving_piece->moved = false;
  }

  move_color =
      (move_color == PieceColor::WHITE) ? PieceColor::BLACK : PieceColor::WHITE;
  previous_moves.pop();
}
