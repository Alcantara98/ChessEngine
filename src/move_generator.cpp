#include "move_generator.h"

namespace engine::parts::move_generator
{
//  PUBLIC FUNCTIONS
auto calculate_possible_moves(BoardState &board_state) -> std::vector<Move>
{
  std::vector<Move> possible_moves;
  for (int y_position = Y_MIN; y_position <= Y_MAX; ++y_position)
  {
    for (int x_position = X_MIN; x_position <= X_MAX; ++x_position)
    {
      Piece *current_piece = board_state.chess_board[x_position][y_position];
      PieceType &piece_type = current_piece->piece_type;

      if (current_piece->piece_color == board_state.color_to_move)
      {
        switch (piece_type)
        {
        case PieceType::PAWN:
          generate_pawn_move(board_state, x_position, y_position,
                             possible_moves);
          break;
        case PieceType::ROOK:
          generate_rook_move(board_state, x_position, y_position,
                             possible_moves);
          break;
        case PieceType::KNIGHT:
          generate_knight_move(board_state, x_position, y_position,
                               possible_moves);
          break;
        case PieceType::BISHOP:
          generate_bishop_move(board_state, x_position, y_position,
                               possible_moves);
          break;
        case PieceType::QUEEN:
          generate_queen_move(board_state, x_position, y_position,
                              possible_moves);
          break;
        case PieceType::KING:
          generate_king_move(board_state, x_position, y_position,
                             possible_moves);
          break;
        default:
          // Empty square.
          break;
        }
      }
    }
  }
  return std::move(possible_moves);
}

// STATIC FUNCTIONS
void generate_pawn_move(BoardState &board_state, int x_position, int y_position,
                        std::vector<Move> &possible_moves)
{
  chess_board_type &board = board_state.chess_board;
  Piece *pawn_piece = board[x_position][y_position];
  bool first_move = !pawn_piece->piece_has_moved;

  // Create pawn moves.
  int pawn_direction;
  int promotion_tile;
  if (pawn_piece->piece_color == PieceColor::WHITE)
  {
    pawn_direction = POSITIVE_DIRECTION;
    promotion_tile = Y_MAX;
  }
  else
  {
    pawn_direction = NEGATIVE_DIRECTION;
    promotion_tile = Y_MIN;
  }
  // Helper variables.
  int y_plus_pd = y_position + pawn_direction;
  int x_minus_1 = x_position - 1;
  int x_plus_1 = x_position + 1;

  if (y_plus_pd != promotion_tile)
  {
    // Normal one square move forward.
    if (board[x_position][y_plus_pd]->piece_type == PieceType::EMPTY)
    {
      possible_moves.emplace_back(x_position, y_position, x_position, y_plus_pd,
                                  pawn_piece, first_move);
    }
    // Normal two square move forward.
    if (first_move &&
        board[x_position][y_plus_pd]->piece_type == PieceType::EMPTY &&
        board[x_position][y_position + (2 * pawn_direction)]->piece_type ==
            PieceType::EMPTY)
    {
      possible_moves.emplace_back(x_position, y_position, x_position,
                                  y_position + (2 * pawn_direction), pawn_piece,
                                  true, true, x_position,
                                  y_position + (2 * pawn_direction));
    }
    // Normal capture.
    if (x_position > X_MIN)
    {
      Piece *capture_left = board[x_minus_1][y_plus_pd];
      if (capture_left->piece_type != PieceType::EMPTY &&
          capture_left->piece_color != pawn_piece->piece_color)
      {
        possible_moves.emplace_back(x_position, y_position, x_minus_1,
                                    y_plus_pd, pawn_piece, capture_left,
                                    first_move);
      }
    }
    if (x_position < X_MAX)
    {
      Piece *capture_right = board[x_plus_1][y_plus_pd];
      if (capture_right->piece_type != PieceType::EMPTY &&
          capture_right->piece_color != pawn_piece->piece_color)
      {
        possible_moves.emplace_back(x_position, y_position, x_plus_1, y_plus_pd,
                                    pawn_piece, capture_right, first_move);
      }
    }
    // En-passant captures.
    if ((y_position == 4 && pawn_piece->piece_color == PieceColor::WHITE) ||
        (y_position == 3 && pawn_piece->piece_color == PieceColor::BLACK))
    {
      Move &previous_move = board_state.previous_move_stack.top();
      if (x_position > X_MIN)
      {
        Piece *left_piece = board[x_minus_1][y_position];
        if (left_piece->piece_type == PieceType::PAWN &&
            previous_move.pawn_moved_two_squares_to_x == (x_minus_1) &&
            previous_move.pawn_moved_two_squares_to_y == y_position)
        {
          if (left_piece->piece_color != pawn_piece->piece_color &&
              left_piece->pawn_moved_two_squares)
          {
            if (board[x_minus_1][y_plus_pd]->piece_type == PieceType::EMPTY)
            {
              possible_moves.emplace_back(x_position, y_position, x_minus_1,
                                          y_plus_pd, pawn_piece, left_piece,
                                          first_move, true);
            }
          }
        }
      }
      if (x_position < X_MAX)
      {
        Piece *right_piece = board[x_plus_1][y_position];
        if (right_piece->piece_type == PieceType::PAWN &&
            previous_move.pawn_moved_two_squares_to_x == (x_plus_1) &&
            previous_move.pawn_moved_two_squares_to_y == y_position)
        {
          if (right_piece->piece_color != pawn_piece->piece_color &&
              right_piece->pawn_moved_two_squares)
          {
            if (board[x_plus_1][y_plus_pd]->piece_type == PieceType::EMPTY)
            {
              possible_moves.emplace_back(x_position, y_position, x_plus_1,
                                          y_plus_pd, pawn_piece, right_piece,
                                          first_move, true);
            }
          }
        }
      }
    }
  }
  // Promotion moves.
  else
  {
    int new_x;
    int new_y;
    // Promotion through normal one square move forward.
    if (board[x_position][y_plus_pd]->piece_type == PieceType::EMPTY)
    {
      possible_moves.emplace_back(x_position, y_position, x_position, y_plus_pd,
                                  pawn_piece, PieceType::QUEEN);
      possible_moves.emplace_back(x_position, y_position, x_position, y_plus_pd,
                                  pawn_piece, PieceType::BISHOP);
      possible_moves.emplace_back(x_position, y_position, x_position, y_plus_pd,
                                  pawn_piece, PieceType::KNIGHT);
      possible_moves.emplace_back(x_position, y_position, x_position, y_plus_pd,
                                  pawn_piece, PieceType::ROOK);
    }
    // Promotion through capture.
    if (x_position > X_MIN)
    { // Check if x_minus_1 is within bounds
      Piece *capture_left = board[x_minus_1][y_plus_pd];
      if (capture_left->piece_type != PieceType::EMPTY &&
          capture_left->piece_color != pawn_piece->piece_color)
      {
        possible_moves.emplace_back(x_position, y_position, x_minus_1,
                                    y_plus_pd, pawn_piece, capture_left,
                                    PieceType::QUEEN);
        possible_moves.emplace_back(x_position, y_position, x_minus_1,
                                    y_plus_pd, pawn_piece, capture_left,
                                    PieceType::BISHOP);
        possible_moves.emplace_back(x_position, y_position, x_minus_1,
                                    y_plus_pd, pawn_piece, capture_left,
                                    PieceType::KNIGHT);
        possible_moves.emplace_back(x_position, y_position, x_minus_1,
                                    y_plus_pd, pawn_piece, capture_left,
                                    PieceType::ROOK);
      }
    }
    if (x_position < X_MAX)
    { // Check if x_plus_1 is within bounds
      Piece *capture_right = board[x_plus_1][y_plus_pd];
      if (capture_right->piece_type != PieceType::EMPTY &&
          capture_right->piece_color != pawn_piece->piece_color)
      {
        possible_moves.emplace_back(x_position, y_position, x_plus_1, y_plus_pd,
                                    pawn_piece, capture_right,
                                    PieceType::QUEEN);
        possible_moves.emplace_back(x_position, y_position, x_plus_1, y_plus_pd,
                                    pawn_piece, capture_right,
                                    PieceType::BISHOP);
        possible_moves.emplace_back(x_position, y_position, x_plus_1, y_plus_pd,
                                    pawn_piece, capture_right,
                                    PieceType::KNIGHT);
        possible_moves.emplace_back(x_position, y_position, x_plus_1, y_plus_pd,
                                    pawn_piece, capture_right, PieceType::ROOK);
      }
    }
  }
}

void generate_king_move(BoardState &board_state, int x_position, int y_position,
                        std::vector<Move> &possible_moves)
{
  chess_board_type &board = board_state.chess_board;
  Piece *king_piece = board[x_position][y_position];
  bool first_move = !king_piece->piece_has_moved;

  // Helper variables.
  int x_minus_1 = x_position - 1;
  int x_plus_1 = x_position + 1;

  for (int new_x = x_position - 1; new_x <= x_position + 1; ++new_x)
  {
    for (int new_y = y_position - 1; new_y <= y_position + 1; ++new_y)
    {
      // Continue if coordinate is out of the chess board.
      if (new_x < X_MIN || new_x > X_MAX || new_y < Y_MIN || new_y > Y_MAX)
      {
        continue;
      }
      // Continue if the position is the kings current position.
      if (new_x == x_position && new_y == y_position)
      {
        continue;
      }
      Piece *target_piece = board[new_x][new_y];
      // Normal move.
      if (target_piece->piece_type == PieceType::EMPTY)
      {
        possible_moves.emplace_back(x_position, y_position, new_x, new_y,
                                    king_piece, first_move);
      }
      // Capture move.
      else if (target_piece->piece_color != king_piece->piece_color)
      {
        possible_moves.emplace_back(x_position, y_position, new_x, new_y,
                                    king_piece, target_piece, first_move);
      }
    }
  }
  // Castle Moves
  if (first_move && !board_state.square_is_attacked(x_position, y_position,
                                                    king_piece->piece_color))
  {
    // Castle king side.
    Piece *rook = board[X_MAX][y_position];
    if (rook->piece_type == PieceType::ROOK && !rook->piece_has_moved)
    {
      if (board[x_plus_1][y_position]->piece_type == PieceType::EMPTY &&
          board[x_position + 2][y_position]->piece_type == PieceType::EMPTY)
      {
        if (!board_state.square_is_attacked(x_plus_1, y_position,
                                            king_piece->piece_color) &&
            !board_state.square_is_attacked(x_position + 2, y_position,
                                            king_piece->piece_color))
        {
          possible_moves.emplace_back(x_position, y_position, x_position + 2,
                                      y_position, king_piece, first_move,
                                      false);
        }
      }
    }
    // Castle queen side.
    rook = board[0][y_position];
    if (rook->piece_type == PieceType::ROOK && !rook->piece_has_moved)
    {
      if (board[x_minus_1][y_position]->piece_type == PieceType::EMPTY &&
          board[x_position - 2][y_position]->piece_type == PieceType::EMPTY &&
          board[x_position - 3][y_position]->piece_type == PieceType::EMPTY)
      {
        if (!board_state.square_is_attacked(x_minus_1, y_position,
                                            king_piece->piece_color) &&
            !board_state.square_is_attacked(x_position - 2, y_position,
                                            king_piece->piece_color) &&
            !board_state.square_is_attacked(x_position - 3, y_position,
                                            king_piece->piece_color))
        {
          possible_moves.emplace_back(x_position, y_position, x_position - 2,
                                      y_position, king_piece, first_move, true);
        }
      }
    }
  }
}

void generate_knight_move(BoardState &board_state, int x_position,
                          int y_position, std::vector<Move> &possible_moves)
{
  chess_board_type &board = board_state.chess_board;
  Piece *knight_piece = board[x_position][y_position];
  bool first_move = !knight_piece->piece_has_moved;

  std::vector<int> x_pos_list = {x_position - 2, x_position - 2, x_position - 1,
                                 x_position - 1, x_position + 1, x_position + 1,
                                 x_position + 2, x_position + 2};
  std::vector<int> y_pos_list = {y_position - 1, y_position + 1, y_position - 2,
                                 y_position + 2, y_position - 2, y_position + 2,
                                 y_position - 1, y_position + 1};

  for (int index = 0; index < MAX_MOVES_KNIGHT; ++index)
  {
    int new_x = x_pos_list[index];
    int new_y = y_pos_list[index];
    // Continue if coordinate is out of the chess board.
    if (new_x < X_MIN || new_x > X_MAX || new_y < Y_MIN || new_y > Y_MAX)
    {
      continue;
    }
    Piece *target_piece = board[new_x][new_y];
    // Normal move.
    if (target_piece->piece_type == PieceType::EMPTY)
    {
      possible_moves.emplace_back(x_position, y_position, new_x, new_y,
                                  knight_piece, first_move);
    }
    // Capture move.
    else if (target_piece->piece_color != knight_piece->piece_color)
    {
      possible_moves.emplace_back(x_position, y_position, new_x, new_y,
                                  knight_piece, target_piece, first_move);
    }
  }
}

void generate_bishop_move(BoardState &board_state, int x_position,
                          int y_position, std::vector<Move> &possible_moves)
{
  // Each respective pair of x and y directions represent a diagonal.
  rook_bishop_move_helper(board_state, x_position, y_position, 1, 1,
                          possible_moves);
  rook_bishop_move_helper(board_state, x_position, y_position, 1, -1,
                          possible_moves);
  rook_bishop_move_helper(board_state, x_position, y_position, -1, 1,
                          possible_moves);
  rook_bishop_move_helper(board_state, x_position, y_position, -1, -1,
                          possible_moves);
}

void generate_rook_move(BoardState &board_state, int x_position, int y_position,
                        std::vector<Move> &possible_moves)
{
  // Each respective pair of x and y directions represent horizontal or
  // vertical moves.
  rook_bishop_move_helper(board_state, x_position, y_position, 1, 0,
                          possible_moves);
  rook_bishop_move_helper(board_state, x_position, y_position, -1, 0,
                          possible_moves);
  rook_bishop_move_helper(board_state, x_position, y_position, 0, 1,
                          possible_moves);
  rook_bishop_move_helper(board_state, x_position, y_position, 0, -1,
                          possible_moves);
}

void generate_queen_move(BoardState &board_state, int x_position,
                         int y_position, std::vector<Move> &possible_moves)
{
  // Queen moves are a combination of rook and bishop moves.
  generate_rook_move(board_state, x_position, y_position, possible_moves);
  generate_bishop_move(board_state, x_position, y_position, possible_moves);
}

// PRIVATE FUNCTIONS
void rook_bishop_move_helper(BoardState &board_state, int x_position,
                             int y_position, int x_direction, int y_direction,
                             std::vector<Move> &possible_moves)
{
  chess_board_type &board = board_state.chess_board;
  Piece *moving_piece = board[x_position][y_position];
  bool first_move = !moving_piece->piece_has_moved;

  int new_x = x_position + x_direction;
  int new_y = y_position + y_direction;
  for (; new_x >= X_MIN && new_x <= X_MAX && new_y >= Y_MIN && new_y <= Y_MAX;
       new_x += x_direction, new_y += y_direction)
  {
    Piece *target_piece = board[new_x][new_y];
    if (target_piece->piece_type == PieceType::EMPTY)
    {
      possible_moves.emplace_back(x_position, y_position, new_x, new_y,
                                  moving_piece, first_move);
    }
    else if (target_piece->piece_color != moving_piece->piece_color)
    {
      possible_moves.emplace_back(x_position, y_position, new_x, new_y,
                                  moving_piece, target_piece, first_move);
      break;
    }
    else
    {
      break;
    }
  }
}
} // namespace engine::parts::move_generator