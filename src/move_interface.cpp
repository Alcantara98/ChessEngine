#include "move_interface.h"

MoveInterface::MoveInterface(BoardState &board_state,
                             std::vector<Move> &possible_moves)
    : board_state(board_state), possible_moves(possible_moves) {}

Move MoveInterface::input_to_move(std::string string_move) {
  Piece *moving_piece;
  Piece *captured_piece = nullptr;
  PieceType promotion_piece_type = PieceType::EMPTY;
  int from_x, from_y;
  int to_x, to_y;
  bool is_en_passant = false;
  bool first_move;
  bool pawn_moved_two = false;
  int pmt_x = -1, pmt_y = -1;

  char piece_type;
  int i = 0;
  while (i == 0) {
    ++i;
    // std::string string_move;
    // std::cin >> string_move;
    std::regex moveRegex(
        R"(^(O-O(?:-O)?)|([kqrbnp])?([a-h][1-8])(x)?([a-h][1-8])=?([qrbns])?([+#])?$)");

    std::smatch matches;

    if (std::regex_match(string_move, matches, moveRegex)) {
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
        piece_type = 'k';
      } else {
        piece_type = matches[2].str().at(0);

        from_x = algebraic_to_int.at(matches[3].str().at(0));
        from_y = matches[3].str().at(1) - '0' - 1;

        to_x = algebraic_to_int.at(matches[5].str().at(0));
        to_y = matches[5].str().at(1) - '0' - 1;
        if (matches[4].matched) {
          if (piece_type == 'p' &&
              board_state.chess_board[to_x][to_y]->type == PieceType::EMPTY) {
            is_en_passant = true;
            captured_piece = board_state.chess_board[to_x][from_y];
          } else {
            captured_piece = board_state.chess_board[to_x][to_y];
          }
        }
        if (piece_type == 'p' && (to_y - from_y == 2 || from_y - to_y == 2)) {
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
      printf("Invalid Move - Regex Match Failure\n");
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

    // Create Move.
    Move next_move = Move(from_x, from_y, to_x, to_y, moving_piece,
                          captured_piece, promotion_piece_type, is_en_passant,
                          first_move, pawn_moved_two, pmt_x, pmt_y);

    // Check if move is in generated possible moves.
    bool found_move = false;
    for (auto &possible_move : possible_moves) {
      if (possible_move == next_move) {
        found_move = true;
        break;
      }
    }
    if (!found_move) {
      printf("Invalid Move - Move not found in possible moves\n");
      continue;
    }

    // Check if move puts king in check.
    PieceColor current_color = board_state.move_color;
    board_state.apply_move(next_move);
    bool king_is_checked = board_state.king_is_checked(current_color);
    board_state.undo_move();
    if (king_is_checked) {
      printf("King is checked - Choose a different move\n");
      continue;
    }
    // Move is valid, exit loop.
    break;
  }
  return Move(from_x, from_y, to_x, to_y, moving_piece, captured_piece,
              promotion_piece_type, is_en_passant, first_move, pawn_moved_two,
              pmt_x, pmt_y);
}