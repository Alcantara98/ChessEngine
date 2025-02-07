#include "move_interface.h"

Move MoveInterface::input_to_move() {
  Piece *moving_piece;
  Piece *captured_piece = nullptr;
  PieceType promotion_piece_type = PieceType::EMPTY;
  int from_x, from_y;
  int to_x, to_y;
  bool is_en_passant;
  bool first_move;
  bool pawn_moved_two;
  int pmt_x, pmt_y;

  char piece_type;

  while (true) {
    std::string move;
    std::cin >> move;
    std::regex moveRegex(
        R"(^((O-O(?:-O)?)|
        ([KQRBN])
        ([a-h][1-8])
        (x?)
        ([a-h][1-8])
        =?([QRBN])?
        ([+#]?)
        )$)",
        std::regex::extended);

    std::smatch matches;

    if (std::regex_match(move, matches, moveRegex)) {
      if (matches[1].matched) {
        // Castle Move.
        from_x = 4;
        if (matches[1] == "O-O") {
          to_x = 6;
        } else {
          to_x = 2;
        }
        if (board_state.move_color == PieceColor::WHITE) {
          from_y = 0;
          to_y = 0;
        } else {
          from_y = 7;
          to_y = 7;
        }
        piece_type = 'K';
      } else {
        piece_type = matches[2].str().at(0);

        from_x = algebraic_to_int.at(matches[3].str().at(0));
        from_y = static_cast<int>(matches[3].str().at(1));

        to_x = algebraic_to_int.at(matches[5].str().at(0));
        to_y = static_cast<int>(matches[5].str().at(1));
        if (matches[4].str() == "x") {
          if (piece_type == 'P' &&
              board_state.chess_board[to_x][to_y]->type == PieceType::EMPTY) {
            is_en_passant = true;
            captured_piece = board_state.chess_board[to_x][from_y];
          } else {
            captured_piece = board_state.chess_board[to_x][to_y];
          }
        }
        if (piece_type == 'P' && (to_y - from_y == 2 || from_y - to_y == 2)) {
          pawn_moved_two = true;
          pmt_x = to_x;
          pmt_y = to_y;
        }
        if (matches[6].matched) {
          promotion_piece_type =
              string_to_piece_type.at(matches[6].str().at(0));
        }
      }
    } else {
      continue;
    }

    moving_piece = board_state.chess_board[from_x][from_y];
    if (string_to_piece_type.at(piece_type) != moving_piece->type) {
      printf("Given piece type: %c does not match square piece type: %c\n",
             piece_type, piece_type_to_string.at(moving_piece->type));
      continue;
    }

    if (moving_piece->moved) {
      first_move = false;
    } else {
      first_move = true;
    }
  }
  return Move(from_x, from_y, to_x, to_y, moving_piece, captured_piece,
              promotion_piece_type, is_en_passant, first_move, pawn_moved_two,
              pmt_x, pmt_y);
}