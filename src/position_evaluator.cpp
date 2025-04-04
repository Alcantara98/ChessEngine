#include "position_evaluator.h"

namespace engine::parts::position_evaluator
{
// PUBLIC FUNCTIONS

auto evaluate_position(const BoardState &board_state) -> int
{
  int eval = 0;
  // We pass eval_temp to evaluators now instead of eval directly. This way, the
  // functions can evaluate white and black pieces the same way (positively). We
  // can then check the piece color and add or subtract accordingly to the
  // actual eval.
  int eval_temp = 0;

  // We give points if a player has a bishop pair. Bishop pair is extremely
  // important in the end game as they can cover both color squares from a
  // distance, and can protect pawns effectively.
  int black_bishop_count = 0;
  int white_bishop_count = 0;

  for (Piece *piece_pointer : board_state.piece_list)
  {
    // Captured pieces have x_file == -1 and y_rank == -1.
    // We skip them.
    if (piece_pointer->x_file == -1)
    {
      continue;
    }

    eval_temp = 0;
    Piece &piece = *piece_pointer;
    PieceType &piece_type = piece.piece_type;
    int &x_file = piece.x_file;
    int &y_rank = piece.y_rank;

    switch (piece_type)
    {
    case PieceType::PAWN:
      evaluate_pawn(x_file, y_rank, piece, eval_temp, board_state);
      break;
    case PieceType::ROOK:
      evaluate_rook(x_file, y_rank, piece, eval_temp, board_state);
      break;
    case PieceType::KNIGHT:
      evaluate_knight(x_file, y_rank, piece, eval_temp, board_state);
      break;
    case PieceType::BISHOP:
      if (piece.piece_color == PieceColor::WHITE)
      {
        ++white_bishop_count;
      }
      else
      {
        ++black_bishop_count;
      }
      evaluate_bishop(x_file, y_rank, piece, eval_temp, board_state);
      break;
    case PieceType::QUEEN:
      evaluate_queen(x_file, y_rank, piece, eval_temp, board_state);
      break;
    case PieceType::KING:
      evaluate_king(x_file, y_rank, piece, eval_temp, board_state);
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
        if (white_bishop_count >= BISHOP_PAIR_COUNT)
        {
          eval += MEDIUM_EVAL_VALUE;
        }
        eval += eval_temp;
      }
      else
      {
        if (black_bishop_count >= BISHOP_PAIR_COUNT)
        {
          eval -= MEDIUM_EVAL_VALUE;
        }
        eval -= eval_temp;
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

void evaluate_pawn(const int x_file,
                   const int y_rank,
                   const Piece &pawn_piece,
                   int &eval,
                   const BoardState &board_state)
{
  // Piece value.
  eval += PAWN_VALUE;

  // Position value - x coordinate.
  eval += PAWN_POSITION_EVAL_MAP[x_file];

  // If in the end game, give a pawn more value the closer they are to
  // getting promoted into a main piece.
  int rank_eval = 0;
  if (board_state.is_end_game)
  {
    if (pawn_piece.piece_color == PieceColor::WHITE)
    {
      rank_eval = y_rank * VERY_SMALL_EVAL_VALUE;
    }
    else
    {
      rank_eval = (Y_MAX - y_rank) * VERY_SMALL_EVAL_VALUE;
    }

    eval += rank_eval;
  }

  // If pawn is in the middle of the board, give it a bonus.
  if (!board_state.is_end_game && (x_file == XD_FILE || x_file == XE_FILE) &&
      (y_rank == Y4_RANK || y_rank == Y5_RANK))
  {
    eval += MEDIUM_EVAL_VALUE;
  }

  evaluate_pawn_file_quality(x_file, y_rank, pawn_piece, eval, board_state);
}

void evaluate_pawn_file_quality(int x_file,
                                int y_rank,
                                const Piece &pawn_piece,
                                int &eval,
                                const BoardState &board_state)
{
  int direction;
  if (pawn_piece.piece_color == PieceColor::WHITE)
  {
    direction = POSITIVE_DIRECTION;
  }
  else
  {
    direction = NEGATIVE_DIRECTION;
  }

  bool is_passed_pawn = true;
  for (int current_rank = y_rank + direction;
       current_rank <= Y_MAX && current_rank >= Y_MIN; ++current_rank)
  {
    Piece &piece = *board_state.chess_board[x_file][current_rank];

    // Decrease evaluation if there is a pawn in front of the pawn. This
    // will also cover doubled pawns.
    if (piece.piece_type == PieceType::PAWN)
    {
      eval -= EXTREMELY_SMALL_EVAL_VALUE;
    }

    // If there is an enemy pawn in front of the pawn, it is not a passed
    // pawn.
    if (is_passed_pawn && piece.piece_color != pawn_piece.piece_color)
    {
      is_passed_pawn = false;
    }

    if (!is_passed_pawn)
    {
      // If we know it is not a passed pawn, we no longer need to check for side
      // pawns.
      continue;
    }

    // Check side pawns. If there are any, it is not a passed pawn.
    for (int side_count = -1; side_count <= 1; side_count += 2)
    {
      // Check if the side pawn is in bounds.
      if (x_file + side_count < X_MIN || x_file + side_count > X_MAX)
      {
        continue;
      }

      Piece &side_piece =
          *board_state.chess_board[x_file + side_count][current_rank];
      if (side_piece.piece_type == PieceType::PAWN &&
          side_piece.piece_color != pawn_piece.piece_color)
      {
        is_passed_pawn = false;
      }
    }
  }

  if (is_passed_pawn)
  {
    eval += MEDIUM_EVAL_VALUE;
  }
}

void evaluate_knight(const int x_file,
                     const int y_rank,
                     const Piece &knight_piece,
                     int &eval,
                     const BoardState &board_state)
{
  // Piece value.
  eval += KNIGHT_VALUE;

  // Less value if knight has moved. Development is important.
  if (!knight_piece.piece_has_moved)
  {
    eval -= LARGE_EVAL_VALUE;
  }

  eval += KNIGHT_POSITION_EVAL_MAP[y_rank];
  eval += KNIGHT_POSITION_EVAL_MAP[x_file];

  // The closer a knight is to the enemy king, the better.
  // We check the knight's distance to the enemy king.
  int enemy_king_x;
  int enemy_king_y;
  if (knight_piece.piece_color == PieceColor::WHITE)
  {
    enemy_king_x = board_state.black_king_x_file;
    enemy_king_y = board_state.black_king_y_rank;
  }
  else
  {
    enemy_king_x = board_state.white_king_x_file;
    enemy_king_y = board_state.white_king_y_rank;
  }

  // Check if the knight is within 4 squares of the enemy king. If it is, give
  // it a bonus.
  if (abs(x_file - enemy_king_x) <= 4 && abs(y_rank - enemy_king_y) <= 4)
  {
    eval += MEDIUM_EVAL_VALUE;
  }
}

void evaluate_bishop(const int x_file,
                     const int y_rank,
                     const Piece &bishop_piece,
                     int &eval,
                     const BoardState &board_state)
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
  if (y_rank - direction >= Y_MIN && y_rank - direction <= Y_MAX)
  {
    if (board_state.chess_board[x_file][y_rank - direction]->piece_type ==
        PieceType::PAWN)
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
    new_x = x_file + direction[0];
    new_y = y_rank + direction[1];

    // Check if still within bounds.
    while (new_x >= X_MIN && new_x <= X_MAX && new_y >= Y_MIN && new_y <= Y_MAX)
    {
      if (board_state.chess_board[new_x][new_y]->piece_type != PieceType::EMPTY)
      {
        break;
      }

      eval += EXTREMELY_SMALL_EVAL_VALUE;

      new_x += direction[0];
      new_y += direction[1];
    }
  }
}

void evaluate_rook(const int x_file,
                   const int y_rank,
                   const Piece &rook_piece,
                   int &eval,
                   const BoardState &board_state)
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
      new_x = x_file + direction[0];
      new_y = y_rank + direction[1];

      // Check if still within bounds.
      while (new_x >= X_MIN && new_x <= X_MAX && new_y >= Y_MIN &&
             new_y <= Y_MAX)
      {
        if (board_state.chess_board[new_x][new_y]->piece_type !=
            PieceType::EMPTY)
        {
          break;
        }

        eval += EXTREMELY_SMALL_EVAL_VALUE;

        new_x += direction[0];
        new_y += direction[1];
      }
    }
  }
}

void evaluate_queen(const int x_file,
                    const int y_rank,
                    const Piece &queen_piece,
                    int &eval,
                    const BoardState &board_state)
{
  // Piece value.
  eval += QUEEN_VALUE;

  // The more moves a queen has, the better.
  int new_x;
  int new_y;
  for (const auto &direction : QUEEN_DIRECTIONS)
  {
    // Increase evaluation based on the number of moves.
    new_x = x_file + direction[0];
    new_y = y_rank + direction[1];

    // Check if still within bounds.
    while (new_x >= X_MIN && new_x <= X_MAX && new_y >= Y_MIN && new_y <= Y_MAX)
    {
      if (board_state.chess_board[new_x][new_y]->piece_type != PieceType::EMPTY)
      {
        break;
      }

      eval += EXTREMELY_SMALL_EVAL_VALUE;

      new_x += direction[0];
      new_y += direction[1];
    }
  }
}

void evaluate_king(const int x_file,
                   const int y_rank,
                   const Piece &king_piece,
                   int &eval,
                   const BoardState &board_state)
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
    evaluate_king_safety(x_file, y_rank, king_piece, eval, board_state);

    // Give points if the king is far away from the center of the board.
    // But not in the end game where it king needs to be active.
    eval += KING_POSITION_EVAL_MAP[x_file];
  }
}

void evaluate_king_safety(const int x_file,
                          const int y_rank,
                          const Piece &king_piece,
                          int &eval,
                          const BoardState &board_state)
{
  {
    int new_x;
    int new_y;
    // King has same directions as queen.
    for (const auto &direction : QUEEN_DIRECTIONS)
    {
      new_x = x_file + direction[0];
      new_y = y_rank + direction[1];
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
