#include "move_interface.h"
#include "attack_check.h"
#include "move_generator.h"

#include <cmath>
#include <map>
#include <regex>
#include <string>

namespace engine::parts::move_interface
{
// PUBLIC FUNCTIONS

auto move_to_string(const Move &move) -> std::string
{
  std::string move_string;

  // Get piece type.
  move_string += PIECE_TYPE_TO_CHAR.at(move.moving_piece->piece_type);

  // Get initial coordinates.
  move_string += INT_TO_ALGEBRAIC.at(move.from_x);
  move_string += std::to_string(move.from_y + 1);

  // Get final coordinates.
  move_string += INT_TO_ALGEBRAIC.at(move.to_x);
  move_string += std::to_string(move.to_y + 1);

  // Pawn promotion.
  if (move.promotion_piece_type != PieceType::EMPTY)
  {
    move_string += PIECE_TYPE_TO_CHAR.at(move.promotion_piece_type);
  }

  return move_string;
}

// PRIVATE FUNCTIONS

auto string_to_move(Move &move,
                    const std::string &move_string,
                    char &piece_type,
                    BoardState &board_state) -> bool
{
  // Check if move is valid.
  std::smatch matches;
  std::regex move_pattern(R"(^([kqrbnp])([a-h][1-8])([a-h][1-8])([qrbns])?$)");
  if (std::regex_match(move_string, matches, move_pattern))
  {

    piece_type = matches[PIECE_TYPE_INDEX].str().at(0);

    // Get initial coordinates.
    std::string from_position = matches[FROM_POSITION_INDEX].str();
    move.from_x = ALGEBRAIC_TO_INT.at(from_position.at(0));
    move.from_y = from_position.at(1) - '0' - 1;

    // Get final coordinates.
    std::string to_position = matches[TO_POSITION_INDEX].str();
    move.to_x = ALGEBRAIC_TO_INT.at(to_position.at(0));
    move.to_y = to_position.at(1) - '0' - 1;

    // Get moving piece.
    move.moving_piece = board_state.chess_board[move.from_x][move.from_y];

    // Check if it is the first move of the moving piece.
    move.first_move_of_moving_piece =
        !move.moving_piece->piece_has_moved; // Get moving piece.
    move.moving_piece = board_state.chess_board[move.from_x][move.from_y];

    // Check if it is the first move of the moving piece.
    move.first_move_of_moving_piece = !move.moving_piece->piece_has_moved;

    // Capture moves.
    // En-passant capture if pawn moves diagonally to empty square.
    if (move.moving_piece->piece_type == PieceType::PAWN &&
        move.from_x != move.to_x &&
        board_state.chess_board[move.to_x][move.to_y]->piece_type ==
            PieceType::EMPTY &&
        board_state.chess_board[move.to_x][move.from_y]->piece_type ==
            PieceType::PAWN)
    {
      move.capture_is_en_passant = true;
      move.captured_piece = board_state.chess_board[move.to_x][move.from_y];
    }
    // Normal capture.
    if (board_state.chess_board[move.to_x][move.to_y]->piece_type !=
        PieceType::EMPTY)
    {
      move.captured_piece = board_state.chess_board[move.to_x][move.to_y];
    }

    // Pawn moved two squares.
    if (move.moving_piece->piece_type == PieceType::PAWN &&
        (std::abs(move.to_y - move.from_y) == 2))
    {
      move.pawn_moved_two_squares = true;
      move.pawn_moved_two_squares_to_x = move.to_x;
      move.pawn_moved_two_squares_to_y = move.to_y;
    }

    // Pawn promotion.
    if (matches[PROMOTION_INDEX].matched)
    {
      move.promotion_piece_type =
          CHAR_TO_PIECE_TYPE.at(matches[PROMOTION_INDEX].str().at(0));
    }
  }
  else
  { // Input did not match regex.
    return false;
  }

  return true;
}

auto validate_move(Move &move,
                   char &piece_type,
                   BoardState &board_state) -> bool
{
  // Check if moving piece is empty.
  if (move.moving_piece->piece_type == PieceType::EMPTY)
  {
    printf("Invalid Move - Moving Piece is Empty Square\n");
    return false;
  }

  // Check if input piece type matches square piece type.
  if (CHAR_TO_PIECE_TYPE.at(piece_type) != move.moving_piece->piece_type)
  {
    printf("Invalid Move - Given piece type: %c does not match square piece "
           "type: %c\n",
           piece_type, PIECE_TYPE_TO_CHAR.at(move.moving_piece->piece_type));
    return false;
  }

  // Check captured piece.
  if (move.captured_piece != nullptr)
  {
    if (move.captured_piece->piece_type == PieceType::EMPTY)
    {
      printf("Invalid Move - Empty Square\n");
      return false;
    }
    if (move.captured_piece->piece_color == move.moving_piece->piece_color)
    {
      printf("Invalid Move - Cannot capture own piece\n");
      return false;
    }
  }
  // Check if move is in generated possible moves.
  const std::vector<Move> &possible_moves =
      move_generator::calculate_possible_moves(board_state);
  bool found_move = false;
  for (const auto &possible_move : possible_moves)
  {
    if (possible_move == move)
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
  if (attack_check::move_leaves_king_in_check(board_state, move))
  {
    printf("Invalid Move - King is checked\n");
    return false;
  }
  return true;
}
} // namespace engine::parts::move_interface