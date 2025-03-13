#include "position_evaluator.h"

namespace engine::parts::position_evaluator
{
// PUBLIC FUNCTIONS

auto evaluate_position(BoardState &board_state) -> int
{
  int eval = 0;

  for (int y_position = Y_MIN; y_position <= Y_MAX; ++y_position)
  {
    for (int x_position = X_MIN; x_position <= X_MAX; ++x_position)
    {
      Piece &piece = *board_state.chess_board[x_position][y_position];
      PieceType &piece_type = piece.piece_type;

      switch (piece_type)
      {
      case PieceType::PAWN:
        evaluate_pawn(x_position, y_position, piece, eval,
                      board_state.is_end_game);
        break;
      case PieceType::ROOK:
        evaluate_rook(x_position, y_position, piece, eval);
        break;
      case PieceType::KNIGHT:
        evaluate_knight(x_position, y_position, piece, eval);
        break;
      case PieceType::BISHOP:
        evaluate_bishop(x_position, y_position, piece, eval);
        break;
      case PieceType::QUEEN:
        evaluate_queen(x_position, y_position, piece, eval);
        break;
      case PieceType::KING:
        evaluate_king(x_position, y_position, piece, eval,
                      board_state.is_end_game);
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

void evaluate_pawn(int x_position, int y_position, Piece &pawn_piece, int &eval,
                   bool &is_end_game)
{
  // Piece value.
  if (pawn_piece.piece_color == PieceColor::WHITE)
  {
    eval += PAWN_VALUE;
  }
  else
  {
    eval -= PAWN_VALUE;
  }

  // Position value - x coordinate.
  if (pawn_piece.piece_color == PieceColor::WHITE)
  {
    eval += PAWN_POSITION_EVAL_MAP[x_position];

    // If in the end game, give pawn value the closer they are to promotion.
    if (is_end_game)
    {
      eval += y_position * MEDIUM_EVAL_VALUE;
    }
  }
  else
  {
    eval -= PAWN_POSITION_EVAL_MAP[x_position];

    // If in the end game, give pawn value the closer they are to promotion.
    if (is_end_game)
    {
      eval -= (Y_MAX - y_position) * MEDIUM_EVAL_VALUE;
    }
  }
}

void evaluate_knight(int x_position, int y_position, Piece &knight_piece,
                     int &eval)
{
  // Piece value.
  if (knight_piece.piece_color == PieceColor::WHITE)
  {
    eval += KNIGHT_VALUE;
    if (!knight_piece.piece_has_moved)
    {
      eval -= LARGE_EVAL_VALUE;
    }
  }
  else
  {
    eval -= KNIGHT_VALUE;
    if (!knight_piece.piece_has_moved)
    {
      eval += LARGE_EVAL_VALUE;
    }
  }

  // The more moves a knight has, the better.
  int new_x;
  int new_y;
  for (const auto &move : KNIGHT_MOVES)
  {
    new_x = x_position + move[0];
    new_y = y_position + move[1];
    if (new_x >= X_MIN && new_x <= X_MAX && new_y >= Y_MIN && new_y <= Y_MAX)
    {
      // Increase evaluation based on the number of moves.
      if (knight_piece.piece_color == PieceColor::WHITE)
      {
        eval += VERY_SMALL_EVAL_VALUE;
      }
      else
      {
        eval -= VERY_SMALL_EVAL_VALUE;
      }
    }
  }
}

void evaluate_bishop(int x_position, int y_position, Piece &bishop_piece,
                     int &eval)
{
  // Piece value.
  if (bishop_piece.piece_color == PieceColor::WHITE)
  {
    eval += BISHOP_VALUE;
    if (!bishop_piece.piece_has_moved)
    {
      eval -= LARGE_EVAL_VALUE;
    }
  }
  else
  {
    eval -= BISHOP_VALUE;
    if (!bishop_piece.piece_has_moved)
    {
      eval += LARGE_EVAL_VALUE;
    }
  }

  // The more moves a bishop has, the better.
  int new_x;
  int new_y;
  for (const auto &direction : BISHOP_DIRECTIONS)
  {
    new_x = x_position + direction[0];
    new_y = y_position + direction[1];
    while (new_x >= X_MIN && new_x <= X_MAX && new_y >= Y_MIN && new_y <= Y_MAX)
    {
      // Increase evaluation based on the number of moves.
      if (bishop_piece.piece_color == PieceColor::WHITE)
      {
        eval += VERY_SMALL_EVAL_VALUE;
      }
      else
      {
        eval -= VERY_SMALL_EVAL_VALUE;
      }

      new_x += direction[0];
      new_y += direction[1];
    }
  }
}

void evaluate_rook(int x_position, int y_position, Piece &rook_piece, int &eval)
{
  // Piece value.
  if (rook_piece.piece_color == PieceColor::WHITE)
  {
    eval += ROOK_VALUE;
  }
  else
  {
    eval -= ROOK_VALUE;
  }

  // The more moves a rook has, the better.
  int new_x;
  int new_y;
  for (const auto &direction : ROOK_DIRECTIONS)
  {
    new_x = x_position + direction[0];
    new_y = y_position + direction[1];
    while (new_x >= X_MIN && new_x <= X_MAX && new_y >= Y_MIN && new_y <= Y_MAX)
    {
      // Increase evaluation based on the number of moves.
      if (rook_piece.piece_color == PieceColor::WHITE)
      {
        eval += EXTREMELY_SMALL_EVAL_VALUE;
      }
      else
      {
        eval -= EXTREMELY_SMALL_EVAL_VALUE;
      }

      new_x += direction[0];
      new_y += direction[1];
    }
  }
}

void evaluate_queen(int x_position, int y_position, Piece &queen_piece,
                    int &eval)
{
  // Piece value.
  if (queen_piece.piece_color == PieceColor::WHITE)
  {
    eval += QUEEN_VALUE;
  }
  else
  {
    eval -= QUEEN_VALUE;
  }

  // The more moves a queen has, the better.
  int new_x;
  int new_y;
  for (const auto &direction : QUEEN_DIRECTIONS)
  {
    new_x = x_position + direction[0];
    new_y = y_position + direction[1];
    while (new_x >= X_MIN && new_x <= X_MAX && new_y >= Y_MIN && new_y <= Y_MAX)
    {
      // Increase evaluation based on the number of moves.
      if (queen_piece.piece_color == PieceColor::WHITE)
      {
        eval += EXTREMELY_SMALL_EVAL_VALUE;
      }
      else
      {
        eval -= EXTREMELY_SMALL_EVAL_VALUE;
      }

      new_x += direction[0];
      new_y += direction[1];
    }
  }
}

void evaluate_king(int x_position, int y_position, Piece &king_piece, int &eval,
                   bool &is_end_game)
{
  // Piece value.
  if (king_piece.piece_color == PieceColor::WHITE)
  {
    eval += KING_VALUE;
  }
  else
  {
    eval -= KING_VALUE;
  }

  // Position value - x coordinate.
  if (!is_end_game && king_piece.piece_color == PieceColor::WHITE)
  {
    eval += KING_POSITION_EVAL_MAP[x_position];
  }
  else
  {
    eval -= KING_POSITION_EVAL_MAP[x_position];
  }
}
} // namespace engine::parts::position_evaluator