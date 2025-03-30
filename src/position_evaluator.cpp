#include "position_evaluator.h"

namespace engine::parts::position_evaluator
{
// PUBLIC FUNCTIONS

auto evaluate_position(BoardState &board_state) -> int
{
  int eval = 0;
  // We pass eval_temp to evaluators now instead of eval directly. This way, the
  // functions can evaluate white and black pieces the same way (positively). We
  // can then check the piece color and add or subtract accordingly to the
  // actual eval.
  int eval_temp = 0;

  for (int y_position = Y_MIN; y_position <= Y_MAX; ++y_position)
  {
    for (int x_position = X_MIN; x_position <= X_MAX; ++x_position)
    {
      eval_temp = 0;
      Piece &piece = *board_state.chess_board[x_position][y_position];
      PieceType &piece_type = piece.piece_type;

      switch (piece_type)
      {
      case PieceType::PAWN:
        evaluate_pawn(x_position, y_position, piece, eval_temp, board_state);
        break;
      case PieceType::ROOK:
        evaluate_rook(x_position, y_position, piece, eval_temp, board_state);
        break;
      case PieceType::KNIGHT:
        evaluate_knight(x_position, y_position, piece, eval_temp, board_state);
        break;
      case PieceType::BISHOP:
        evaluate_bishop(x_position, y_position, piece, eval_temp, board_state);
        break;
      case PieceType::QUEEN:
        evaluate_queen(x_position, y_position, piece, eval_temp, board_state);
        break;
      case PieceType::KING:
        evaluate_king(x_position, y_position, piece, eval_temp, board_state);
        break;
      default:
        // Empty square.
        break;
      }

      // If piece is black, subtract the evaluation.
      if (piece_type != PieceType::EMPTY)
      {
        if (piece.piece_color == PieceColor::WHITE)
        {
          eval += eval_temp;
        }
        else
        {
          eval -= eval_temp;
        }
      }
    }
  }

  // In raw evaluations, positive eval is good for white and negative eval is
  // good for black. Since negamax nodes are always maximizing nodes, we need to
  // negate the evalualtion for black.
  if (board_state.color_to_move == PieceColor::BLACK)
  {
    return -eval;
  }
  return eval;
}

// PRIVATE FUNCTIONS

void evaluate_pawn(int x_position,
                   int y_position,
                   Piece &pawn_piece,
                   int &eval,
                   BoardState &board_state)
{
  // Piece value.
  eval += PAWN_VALUE;

  // Position value - x coordinate.
  eval += PAWN_POSITION_EVAL_MAP[x_position];

  // If in the end game, give a pawn more value the closer they are to
  // getting promoted into a main piece.
  int rank_eval = 0;
  if (pawn_piece.piece_color == PieceColor::WHITE)
  {
    rank_eval = y_position * MEDIUM_EVAL_VALUE;
  }
  else
  {
    rank_eval = (Y_MAX - y_position) * MEDIUM_EVAL_VALUE;
  }
  if (board_state.is_end_game)
  {
    eval += rank_eval;
  }

  // We do not want double pawns.
  // Check if there is pawn in the first two squares in front of the pawn.
  // If there is, decrease evaluation.
  int direction;
  if (pawn_piece.piece_color == PieceColor::WHITE)
  {
    direction = POSITIVE_DIRECTION;
  }
  else
  {
    direction = NEGATIVE_DIRECTION;
  }
  // Check one square in front of the pawn.
  for (int rank_count = 1; rank_count <= MAX_DOUBLE_PAWN_SQUARES_TO_CHECK;
       ++rank_count)
  {
    if (y_position + (direction * rank_count) >= Y_MIN &&
        y_position + (direction * rank_count) <= Y_MAX)
    {
      Piece &piece =
          *board_state
               .chess_board[x_position][y_position + (direction * rank_count)];
      if (piece.piece_type == PieceType::PAWN &&
          piece.piece_color == pawn_piece.piece_color)
      {
        // Decrease evaluation if there is a pawn in front of the pawn.
        eval -= MEDIUM_EVAL_VALUE;
      }
    }
  }
}

void evaluate_knight(int x_position,
                     int y_position,
                     Piece &knight_piece,
                     int &eval,
                     BoardState &board_state)
{
  // Piece value.
  eval += KNIGHT_VALUE;

  // Less value if knight has moved. Development is important.
  if (!knight_piece.piece_has_moved)
  {
    eval -= LARGE_EVAL_VALUE;
  }

  // The more moves a knight has, the better.
  int new_x;
  int new_y;
  for (const auto &move : KNIGHT_MOVES)
  {
    // Increase evaluation based on the number of moves.
    new_x = x_position + move[0];
    new_y = y_position + move[1];

    // Check if within bounds.
    if (new_x >= X_MIN && new_x <= X_MAX && new_y >= Y_MIN && new_y <= Y_MAX)
    {
      eval += VERY_SMALL_EVAL_VALUE;
    }
  }
}

void evaluate_bishop(int x_position,
                     int y_position,
                     Piece &bishop_piece,
                     int &eval,
                     BoardState &board_state)
{
  // Piece value.
  eval += BISHOP_VALUE;
  if (!bishop_piece.piece_has_moved)
  {
    eval -= LARGE_EVAL_VALUE;
  }

  // If bishop is blocking a pawn, decrease evaluation.
  // When I play, I don't like it when my bishops block my pawns.
  // This is a personal preference and is experimental.
  int direction;
  if (bishop_piece.piece_color == PieceColor::WHITE)
  {
    direction = POSITIVE_DIRECTION;
  }
  else
  {
    direction = NEGATIVE_DIRECTION;
  }
  if (y_position - direction >= Y_MIN && y_position - direction <= Y_MAX)
  {
    if (board_state.chess_board[x_position][y_position - direction]
            ->piece_type == PieceType::PAWN)
    {
      eval -= LARGE_EVAL_VALUE;
    }
  }

  // The more moves a bishop has, the better.
  int new_x;
  int new_y;
  for (const auto &direction : BISHOP_DIRECTIONS)
  {
    // Increase evaluation based on the number of moves.
    new_x = x_position + direction[0];
    new_y = y_position + direction[1];

    // Check if still within bounds.
    while (new_x >= X_MIN && new_x <= X_MAX && new_y >= Y_MIN && new_y <= Y_MAX)
    {
      if (board_state.chess_board[new_x][new_y]->piece_type != PieceType::EMPTY)
      {
        break;
      }

      eval += VERY_SMALL_EVAL_VALUE;

      new_x += direction[0];
      new_y += direction[1];
    }
  }
}

void evaluate_rook(int x_position,
                   int y_position,
                   Piece &rook_piece,
                   int &eval,
                   BoardState &board_state)
{
  // Piece value.
  eval += ROOK_VALUE;

  if (board_state.is_end_game)
  {
    // The more moves a rook has, the better.
    int new_x;
    int new_y;
    for (const auto &direction : ROOK_DIRECTIONS)
    {
      // Increase evaluation based on the number of moves.
      new_x = x_position + direction[0];
      new_y = y_position + direction[1];

      // Check if still within bounds.
      while (new_x >= X_MIN && new_x <= X_MAX && new_y >= Y_MIN &&
             new_y <= Y_MAX)
      {
        if (board_state.chess_board[new_x][new_y]->piece_type !=
            PieceType::EMPTY)
        {
          break;
        }

        eval += VERY_SMALL_EVAL_VALUE;

        new_x += direction[0];
        new_y += direction[1];
      }
    }
  }
}

void evaluate_queen(int x_position,
                    int y_position,
                    Piece &queen_piece,
                    int &eval,
                    BoardState &board_state)
{
  // Piece value.
  eval += QUEEN_VALUE;

  // The more moves a queen has, the better.
  int new_x;
  int new_y;
  for (const auto &direction : QUEEN_DIRECTIONS)
  {
    // Increase evaluation based on the number of moves.
    new_x = x_position + direction[0];
    new_y = y_position + direction[1];

    // Check if still within bounds.
    while (new_x >= X_MIN && new_x <= X_MAX && new_y >= Y_MIN && new_y <= Y_MAX)
    {
      if (board_state.chess_board[new_x][new_y]->piece_type != PieceType::EMPTY)
      {
        break;
      }

      eval += VERY_SMALL_EVAL_VALUE;

      new_x += direction[0];
      new_y += direction[1];
    }
  }
}

void evaluate_king(int x_position,
                   int y_position,
                   Piece &king_piece,
                   int &eval,
                   BoardState &board_state)
{
  // Piece value.
  eval += KING_VALUE;

  if (!board_state.is_end_game)
  {
    // Give eval points if the king has castled, but not in the end game where
    // it doesn't matter anymore.
    if ((king_piece.piece_color == PieceColor::WHITE &&
         board_state.white_has_castled) ||
        (king_piece.piece_color == PieceColor::BLACK &&
         board_state.black_has_castled))
    {
      eval += LARGE_EVAL_VALUE;
    }
  }

  // Position value - x coordinate.
  if (!board_state.is_end_game)
  {
    evaluate_king_safety(x_position, y_position, king_piece, eval, board_state);

    // Give points if the king is far away from the center of the board.
    // But not in the end game where it king needs to be active.
    eval += KING_POSITION_EVAL_MAP[x_position];
  }
}

void evaluate_king_safety(int x_position,
                          int y_position,
                          Piece &king_piece,
                          int &eval,
                          BoardState &board_state)
{
  {
    int new_x;
    int new_y;
    // King has same directions as queen.
    for (const auto &direction : QUEEN_DIRECTIONS)
    {
      new_x = x_position + direction[0];
      new_y = y_position + direction[1];
      while (new_x >= X_MIN && new_x <= X_MAX && new_y >= Y_MIN &&
             new_y <= Y_MAX)
      {
        // Decrease evaluation based on the number of moves as it means king
        // is less safe.
        if (board_state.chess_board[new_x][new_y]->piece_type !=
            PieceType::EMPTY)
        {
          break;
        }

        eval -= VERY_SMALL_EVAL_VALUE;

        new_x += direction[0];
        new_y += direction[1];
      }
    }
  }
}
} // namespace engine::parts::position_evaluator