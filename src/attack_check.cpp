#include "attack_check.h"

namespace engine::parts::attack_check
{
auto square_is_attacked(BoardState &board_state,
                        int x_file,
                        int y_rank,
                        PieceColor color_being_attacked) -> bool
{
  return square_is_attacked_by_pawn(board_state, x_file, y_rank,
                                    color_being_attacked) ||
         square_is_attacked_by_knight(board_state, x_file, y_rank,
                                      color_being_attacked) ||
         square_is_attacked_by_rook_or_queen(board_state, x_file, y_rank,
                                             color_being_attacked) ||
         square_is_attacked_by_bishop_or_queen(board_state, x_file, y_rank,
                                               color_being_attacked) ||
         square_is_attacked_by_king(board_state, x_file, y_rank,
                                    color_being_attacked);
}

auto king_is_checked(BoardState &board_state, PieceColor color_of_king) -> bool
{
  if (color_of_king == PieceColor::WHITE)
  {
    return square_is_attacked(board_state, board_state.white_king_x_file,
                              board_state.white_king_y_rank, color_of_king);
  }

  return square_is_attacked(board_state, board_state.black_king_x_file,
                            board_state.black_king_y_rank, color_of_king);
}

auto move_leaves_king_in_check(BoardState &board_state, Move &move) -> bool
{
  board_state.apply_move(move);
  bool king_is_checked_after_move =
      king_is_checked(board_state, move.moving_piece->piece_color);
  board_state.undo_move();
  return king_is_checked_after_move;
}

auto square_is_attacked_by_pawn(BoardState &board_state,
                                int &x_file,
                                int &y_rank,
                                PieceColor &color_being_attacked) -> bool
{
  int pawn_direction = (color_being_attacked == PieceColor::WHITE)
                           ? POSITIVE_DIRECTION
                           : NEGATIVE_DIRECTION;

  // Check for pawn attacks in negative x direction.
  if (x_file > X_MIN && y_rank + pawn_direction >= Y_MIN &&
      y_rank + pawn_direction <= Y_MAX)
  {
    if (board_state.chess_board[x_file - 1][y_rank + pawn_direction]
                ->piece_type == PieceType::PAWN &&
        board_state.chess_board[x_file - 1][y_rank + pawn_direction]
                ->piece_color != color_being_attacked)
    {
      return true;
    }
  }

  // Check for pawn attacks in positive x direction.
  if (x_file < X_MAX && y_rank + pawn_direction >= Y_MIN &&
      y_rank + pawn_direction <= Y_MAX)
  {
    if (board_state.chess_board[x_file + 1][y_rank + pawn_direction]
                ->piece_type == PieceType::PAWN &&
        board_state.chess_board[x_file + 1][y_rank + pawn_direction]
                ->piece_color != color_being_attacked)
    {
      return true;
    }
  }
  return false;
}

auto square_is_attacked_by_knight(BoardState &board_state,
                                  int &x_file,
                                  int &y_rank,
                                  PieceColor &color_being_attacked) -> bool
{
  return std::any_of(
      KNIGHT_MOVES.begin(), KNIGHT_MOVES.end(),
      [&](const auto &direction)
      {
        int new_x = x_file + direction[0];
        int new_y = y_rank + direction[1];
        return new_x >= X_MIN && new_x <= X_MAX && new_y >= Y_MIN &&
               new_y <= Y_MAX &&
               board_state.chess_board[new_x][new_y]->piece_type ==
                   PieceType::KNIGHT &&
               board_state.chess_board[new_x][new_y]->piece_color !=
                   color_being_attacked;
      });
}

auto square_is_attacked_by_rook_or_queen(BoardState &board_state,
                                         int &x_file,
                                         int &y_rank,
                                         PieceColor &color_being_attacked)
    -> bool
{
  for (const auto &direction : ROOK_DIRECTIONS)
  {
    int new_x = x_file + direction[0];
    int new_y = y_rank + direction[1];
    while (new_x >= X_MIN && new_x <= X_MAX && new_y >= Y_MIN && new_y <= Y_MAX)
    {
      Piece *target_piece = board_state.chess_board[new_x][new_y];
      if (target_piece->piece_type != PieceType::EMPTY)
      {
        if ((target_piece->piece_type == PieceType::ROOK ||
             target_piece->piece_type == PieceType::QUEEN) &&
            target_piece->piece_color != color_being_attacked)
        {
          return true;
        }
        // Stop if a piece is blocking the way.
        break;
      }
      new_x += direction[0];
      new_y += direction[1];
    }
  }
  return false;
}

auto square_is_attacked_by_bishop_or_queen(BoardState &board_state,
                                           int &x_file,
                                           int &y_rank,
                                           PieceColor &color_being_attacked)
    -> bool
{
  for (const auto &direction : BISHOP_DIRECTIONS)
  {
    int new_x = x_file + direction[0];
    int new_y = y_rank + direction[1];
    while (new_x >= X_MIN && new_x <= X_MAX && new_y >= Y_MIN && new_y <= Y_MAX)
    {
      Piece *target_piece = board_state.chess_board[new_x][new_y];
      if (target_piece->piece_type != PieceType::EMPTY)
      {
        if ((target_piece->piece_type == PieceType::BISHOP ||
             target_piece->piece_type == PieceType::QUEEN) &&
            target_piece->piece_color != color_being_attacked)
        {
          return true;
        }
        // Stop if a piece is blocking the way.
        break;
      }
      new_x += direction[0];
      new_y += direction[1];
    }
  }
  return false;
}

auto square_is_attacked_by_king(BoardState &board_state,
                                int &x_file,
                                int &y_rank,
                                PieceColor &color_being_attacked) -> bool
{
  // Check for king attacks.
  return std::any_of(
      KING_MOVES.begin(), KING_MOVES.end(),
      [&](const auto &direction)
      {
        int new_x = x_file + direction[0];
        int new_y = y_rank + direction[1];
        return new_x >= X_MIN && new_x <= X_MAX && new_y >= Y_MIN &&
               new_y <= Y_MAX &&
               board_state.chess_board[new_x][new_y]->piece_type ==
                   PieceType::KING &&
               board_state.chess_board[new_x][new_y]->piece_color !=
                   color_being_attacked;
      });
}
} // namespace engine::parts::attack_check