#include "move_interface.h"

namespace engine::parts
{
// CONSTRUCTORS
MoveInterface::MoveInterface(BoardState &board_state)
    : game_board_state(board_state)
{
}

// PUBLIC FUNCTIONS
auto MoveInterface::input_to_move(const std::vector<Move> &possible_moves,
                                  std::string move_string) -> Move
{
  std::unique_ptr<Move> move;
  char piece_type;
  while (true)
  {
    // Reset variables.
    move =
        std::make_unique<Move>(-1, -1, -1, -1, nullptr, nullptr,
                               PieceType::EMPTY, false, false, false, -1, -1);

    // Get move from user.
    std::cout << "Enter move: ";
    std::cin >> move_string;
    std::cout << '\n';

    // Parse input string move.
    if (!create_move_from_string(move, move_string, piece_type))
    {
      printf("Invalid Move - Regex Match Failure\n");
      continue;
    }

    // Validate move.
    if (!validate_move(possible_moves, move.get(), piece_type))
    {
      continue;
    }

    // Move is valid, exit loop.
    break;
  }
  return *move;
}

// PRIVATE FUNCTIONS
auto MoveInterface::create_move_from_string(std::unique_ptr<Move> &move,
                                            const std::string &move_string,
                                            char &piece_type) -> bool
{
  // Check if move is valid.
  std::smatch matches;
  std::regex moveRegex(
      R"(^(O-O(?:-O)?)|([kqrbnp])([a-h][1-8])(x)?([a-h][1-8])=?([qrbns])?([+#])?$)");
  if (std::regex_match(move_string, matches, moveRegex))
  {
    // Get initial and final coordinates.
    if (matches[CASTLE_MOVE_INDEX].matched)
    {
      // Castle Move.
      piece_type = 'k';
      // Initial x coordinate for both white and black king.
      move->from_x = XE_FILE;
      // King-side : Queen-side.
      move->to_x = matches[1] == "O-O" ? XG_FILE : XB_FILE;
      move->from_y = move->to_y =
          game_board_state.color_to_move == PieceColor::WHITE ? Y_MIN : Y_MAX;
    }
    else
    {
      piece_type = matches[PIECE_TYPE_INDEX].str().at(0);

      // Get initial coordinates.
      std::string from_position = matches[FROM_POSITION_INDEX].str();
      move->from_x = ALGEBRAIC_TO_INT.at(from_position.at(0));
      move->from_y = from_position.at(1) - '0' - 1;

      // Get final coordinates.
      std::string to_position = matches[TO_POSITION_INDEX].str();
      move->to_x = ALGEBRAIC_TO_INT.at(to_position.at(0));
      move->to_y = to_position.at(1) - '0' - 1;
    }

    // Get moving piece.
    move->moving_piece =
        game_board_state.chess_board[move->from_x][move->from_y];

    // Check if it is the first move of the moving piece.
    move->first_move_of_moving_piece =
        !move->moving_piece->piece_has_moved; // Get moving piece.
    move->moving_piece =
        game_board_state.chess_board[move->from_x][move->from_y];

    // Check if it is the first move of the moving piece.
    move->first_move_of_moving_piece = !move->moving_piece->piece_has_moved;

    // Capture move.
    if (matches[4].matched)
    {
      // En-passant capture if pawn moves diagonally to empty square.
      if (move->moving_piece->piece_type == PieceType::PAWN &&
          game_board_state.chess_board[move->to_x][move->to_y]->piece_type ==
              PieceType::EMPTY)
      {
        move->capture_is_en_passant = true;
        move->captured_piece =
            game_board_state.chess_board[move->to_x][move->from_y];
      }
      // Normal capture.
      else
      {
        move->captured_piece =
            game_board_state.chess_board[move->to_x][move->to_y];
      }
    }

    // Pawn moved two squares.
    if (move->moving_piece->piece_type == PieceType::PAWN &&
        (std::abs(move->to_y - move->from_y) == 2))
    {
      move->pawn_moved_two_squares = true;
      move->pawn_moved_two_squares_to_x = move->to_x;
      move->pawn_moved_two_squares_to_y = move->to_y;
    }

    // Pawn promotion.
    if (matches[PROMOTION_INDEX].matched)
    {
      move->promotion_piece_type =
          STRING_TO_PIECE_TYPE.at(matches[PROMOTION_INDEX].str().at(0));
    }
  }
  else
  { // Input did not match regex.
    return false;
  }

  return true;
}

auto MoveInterface::validate_move(const std::vector<Move> &possible_moves,
                                  Move *move, char &piece_type) -> bool
{
  // Check if moving piece is empty.
  if (move->moving_piece->piece_type == PieceType::EMPTY)
  {
    printf("Invalid Move - Moving Piece is Empty Square\n");
    return false;
  }

  // Check if input piece type matches square piece type.
  if (STRING_TO_PIECE_TYPE.at(piece_type) != move->moving_piece->piece_type)
  {
    printf("Invalid Move - Given piece type: %c does not match square piece "
           "type: %c\n",
           piece_type, PIECE_TYPE_TO_STRING.at(move->moving_piece->piece_type));
    return false;
  }

  // Check captured piece.
  if (move->captured_piece != nullptr)
  {
    if (move->captured_piece->piece_type == PieceType::EMPTY)
    {
      printf("Invalid Move - Empty Square\n");
      return false;
    }
    if (move->captured_piece->piece_color == move->moving_piece->piece_color)
    {
      printf("Invalid Move - Cannot capture own piece\n");
      return false;
    }
  }
  // Check if move is in generated possible moves.
  bool found_move = false;
  for (const auto &possible_move : possible_moves)
  {
    if (possible_move == *move)
    {
      found_move = true;
      break;
    }
  }
  if (!found_move)
  {
    printf("Invalid Move - Move not found in possible moves\n");
    return false;
  }

  // Check if move puts king in check.
  if (game_board_state.move_leaves_king_in_check(*move))
  {
    printf("Invalid Move - King is checked\n");
    return false;
  }
  return true;
}
} // namespace engine::parts