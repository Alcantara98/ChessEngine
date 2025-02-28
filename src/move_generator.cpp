#include "move_generator.h"

namespace engine::parts
{
//  PUBLIC FUNCTIONS
void MoveGenerator::generate_pawn_move(BoardState &board_state,
                                       int x_coordinate, int y_coordinate,
                                       std::vector<Move> &possible_moves)
{
  std::array<std::array<Piece *, 8>, 8> &board = board_state.chess_board;
  Piece *pawn_piece = board[x_coordinate][y_coordinate];
  bool first_move = !pawn_piece->piece_has_moved;

  // Create pawn moves.
  int pawn_direction, promotion_tile;
  if (pawn_piece->piece_color == PieceColor::WHITE)
  {
    pawn_direction = 1;
    promotion_tile = 7;
  }
  else
  {
    pawn_direction = -1;
    promotion_tile = 0;
  }
  // Helper variables.
  int y_plus_pd = y_coordinate + pawn_direction;
  int x_minus_1 = x_coordinate - 1;
  int x_plus_1 = x_coordinate + 1;

  if (y_plus_pd != promotion_tile)
  {
    // Normal one square move forward.
    if (board[x_coordinate][y_plus_pd]->piece_type == PieceType::EMPTY)
    {
      possible_moves.emplace_back(x_coordinate, y_coordinate, x_coordinate,
                                  y_plus_pd, pawn_piece, first_move);
    }
    // Normal two square move forward.
    if (first_move &&
        board[x_coordinate][y_plus_pd]->piece_type == PieceType::EMPTY &&
        board[x_coordinate][y_coordinate + (2 * pawn_direction)]->piece_type ==
            PieceType::EMPTY)
    {
      possible_moves.emplace_back(x_coordinate, y_coordinate, x_coordinate,
                                  y_coordinate + (2 * pawn_direction),
                                  pawn_piece, true, true, x_coordinate,
                                  y_coordinate + (2 * pawn_direction));
    }
    // Normal capture.
    if (x_coordinate > 0)
    {
      Piece *capture_left = board[x_minus_1][y_plus_pd];
      if (capture_left->piece_type != PieceType::EMPTY &&
          capture_left->piece_color != pawn_piece->piece_color)
      {
        possible_moves.emplace_back(x_coordinate, y_coordinate, x_minus_1,
                                    y_plus_pd, pawn_piece, capture_left,
                                    first_move);
      }
    }
    if (x_coordinate < 7)
    {
      Piece *capture_right = board[x_plus_1][y_plus_pd];
      if (capture_right->piece_type != PieceType::EMPTY &&
          capture_right->piece_color != pawn_piece->piece_color)
      {
        possible_moves.emplace_back(x_coordinate, y_coordinate, x_plus_1,
                                    y_plus_pd, pawn_piece, capture_right,
                                    first_move);
      }
    }
    // En-passant captures.
    if ((y_coordinate == 4 && pawn_piece->piece_color == PieceColor::WHITE) ||
        (y_coordinate == 3 && pawn_piece->piece_color == PieceColor::BLACK))
    {
      Move &previous_move = board_state.previous_move_stack.top();
      if (x_coordinate > 0)
      {
        Piece *left_piece = board[x_minus_1][y_coordinate];
        if (left_piece->piece_type == PieceType::PAWN &&
            previous_move.pawn_moved_two_squares_to_x == (x_minus_1) &&
            previous_move.pawn_moved_two_squares_to_y == y_coordinate)
        {
          if (left_piece->piece_color != pawn_piece->piece_color &&
              left_piece->pawn_moved_two_squares)
          {
            if (board[x_minus_1][y_plus_pd]->piece_type == PieceType::EMPTY)
            {
              possible_moves.emplace_back(x_coordinate, y_coordinate, x_minus_1,
                                          y_plus_pd, pawn_piece, left_piece,
                                          first_move, true);
            }
          }
        }
      }
      if (x_coordinate < 7)
      {
        Piece *right_piece = board[x_plus_1][y_coordinate];
        if (right_piece->piece_type == PieceType::PAWN &&
            previous_move.pawn_moved_two_squares_to_x == (x_plus_1) &&
            previous_move.pawn_moved_two_squares_to_y == y_coordinate)
        {
          if (right_piece->piece_color != pawn_piece->piece_color &&
              right_piece->pawn_moved_two_squares)
          {
            if (board[x_plus_1][y_plus_pd]->piece_type == PieceType::EMPTY)
            {
              possible_moves.emplace_back(x_coordinate, y_coordinate, x_plus_1,
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
    int new_x, new_y;
    // Promotion through normal one square move forward.
    if (board[x_coordinate][y_plus_pd]->piece_type == PieceType::EMPTY)
    {
      possible_moves.emplace_back(x_coordinate, y_coordinate, x_coordinate,
                                  y_plus_pd, pawn_piece, PieceType::QUEEN);
      possible_moves.emplace_back(x_coordinate, y_coordinate, x_coordinate,
                                  y_plus_pd, pawn_piece, PieceType::BISHOP);
      possible_moves.emplace_back(x_coordinate, y_coordinate, x_coordinate,
                                  y_plus_pd, pawn_piece, PieceType::KNIGHT);
      possible_moves.emplace_back(x_coordinate, y_coordinate, x_coordinate,
                                  y_plus_pd, pawn_piece, PieceType::ROOK);
    }
    // Promotion through capture.
    if (x_coordinate > 0)
    { // Check if x_minus_1 is within bounds
      Piece *capture_left = board[x_minus_1][y_plus_pd];
      if (capture_left->piece_type != PieceType::EMPTY &&
          capture_left->piece_color != pawn_piece->piece_color)
      {
        possible_moves.emplace_back(x_coordinate, y_coordinate, x_minus_1,
                                    y_plus_pd, pawn_piece, capture_left,
                                    PieceType::QUEEN);
        possible_moves.emplace_back(x_coordinate, y_coordinate, x_minus_1,
                                    y_plus_pd, pawn_piece, capture_left,
                                    PieceType::BISHOP);
        possible_moves.emplace_back(x_coordinate, y_coordinate, x_minus_1,
                                    y_plus_pd, pawn_piece, capture_left,
                                    PieceType::KNIGHT);
        possible_moves.emplace_back(x_coordinate, y_coordinate, x_minus_1,
                                    y_plus_pd, pawn_piece, capture_left,
                                    PieceType::ROOK);
      }
    }
    if (x_coordinate < 7)
    { // Check if x_plus_1 is within bounds
      Piece *capture_right = board[x_plus_1][y_plus_pd];
      if (capture_right->piece_type != PieceType::EMPTY &&
          capture_right->piece_color != pawn_piece->piece_color)
      {
        possible_moves.emplace_back(x_coordinate, y_coordinate, x_plus_1,
                                    y_plus_pd, pawn_piece, capture_right,
                                    PieceType::QUEEN);
        possible_moves.emplace_back(x_coordinate, y_coordinate, x_plus_1,
                                    y_plus_pd, pawn_piece, capture_right,
                                    PieceType::BISHOP);
        possible_moves.emplace_back(x_coordinate, y_coordinate, x_plus_1,
                                    y_plus_pd, pawn_piece, capture_right,
                                    PieceType::KNIGHT);
        possible_moves.emplace_back(x_coordinate, y_coordinate, x_plus_1,
                                    y_plus_pd, pawn_piece, capture_right,
                                    PieceType::ROOK);
      }
    }
  }
}

void MoveGenerator::generate_king_move(BoardState &board_state,
                                       int x_coordinate, int y_coordinate,
                                       std::vector<Move> &possible_moves)
{
  std::array<std::array<Piece *, 8>, 8> &board = board_state.chess_board;
  Piece *king_piece = board[x_coordinate][y_coordinate];
  bool first_move = !king_piece->piece_has_moved;

  // Helper variables.
  int x_minus_1 = x_coordinate - 1;
  int x_plus_1 = x_coordinate + 1;

  for (int new_x = x_coordinate - 1; new_x <= x_coordinate + 1; ++new_x)
  {
    for (int new_y = y_coordinate - 1; new_y <= y_coordinate + 1; ++new_y)
    {
      // Continue if coordinate is out of the chess board.
      if (new_x < 0 || new_x > 7 || new_y < 0 || new_y > 7)
      {
        continue;
      }
      // Continue if the position is the kings current position.
      if (new_x == x_coordinate && new_y == y_coordinate)
      {
        continue;
      }
      Piece *target_piece = board[new_x][new_y];
      // Normal move.
      if (target_piece->piece_type == PieceType::EMPTY)
      {
        possible_moves.emplace_back(x_coordinate, y_coordinate, new_x, new_y,
                                    king_piece, first_move);
      }
      // Capture move.
      else if (target_piece->piece_color != king_piece->piece_color)
      {
        possible_moves.emplace_back(x_coordinate, y_coordinate, new_x, new_y,
                                    king_piece, target_piece, first_move);
      }
    }
  }
  // Castle Moves
  if (first_move && !board_state.square_is_attacked(x_coordinate, y_coordinate,
                                                    king_piece->piece_color))
  {
    // Castle king side.
    Piece *rook = board[7][y_coordinate];
    if (rook->piece_type == PieceType::ROOK && rook->piece_has_moved == false)
    {
      if (board[x_plus_1][y_coordinate]->piece_type == PieceType::EMPTY &&
          board[x_coordinate + 2][y_coordinate]->piece_type == PieceType::EMPTY)
      {
        if (!board_state.square_is_attacked(x_plus_1, y_coordinate,
                                            king_piece->piece_color) &&
            !board_state.square_is_attacked(x_coordinate + 2, y_coordinate,
                                            king_piece->piece_color))
        {
          possible_moves.emplace_back(x_coordinate, y_coordinate,
                                      x_coordinate + 2, y_coordinate,
                                      king_piece, first_move, false);
        }
      }
    }
    // Castle queen side.
    rook = board[0][y_coordinate];
    if (rook->piece_type == PieceType::ROOK && rook->piece_has_moved == false)
    {
      if (board[x_minus_1][y_coordinate]->piece_type == PieceType::EMPTY &&
          board[x_coordinate - 2][y_coordinate]->piece_type ==
              PieceType::EMPTY &&
          board[x_coordinate - 3][y_coordinate]->piece_type == PieceType::EMPTY)
      {
        if (!board_state.square_is_attacked(x_minus_1, y_coordinate,
                                            king_piece->piece_color) &&
            !board_state.square_is_attacked(x_coordinate - 2, y_coordinate,
                                            king_piece->piece_color) &&
            !board_state.square_is_attacked(x_coordinate - 3, y_coordinate,
                                            king_piece->piece_color))
        {
          possible_moves.emplace_back(x_coordinate, y_coordinate,
                                      x_coordinate - 2, y_coordinate,
                                      king_piece, first_move, true);
        }
      }
    }
  }
}

void MoveGenerator::generate_knight_move(BoardState &board_state,
                                         int x_coordinate, int y_coordinate,
                                         std::vector<Move> &possible_moves)
{
  std::array<std::array<Piece *, 8>, 8> &board = board_state.chess_board;
  Piece *knight_piece = board[x_coordinate][y_coordinate];
  bool first_move = !knight_piece->piece_has_moved;

  std::vector<int> x_pos_list = {
      x_coordinate - 2, x_coordinate - 2, x_coordinate - 1, x_coordinate - 1,
      x_coordinate + 1, x_coordinate + 1, x_coordinate + 2, x_coordinate + 2};
  std::vector<int> y_pos_list = {
      y_coordinate - 1, y_coordinate + 1, y_coordinate - 2, y_coordinate + 2,
      y_coordinate - 2, y_coordinate + 2, y_coordinate - 1, y_coordinate + 1};

  for (int index = 0; index < 8; ++index)
  {
    int new_x = x_pos_list[index];
    int new_y = y_pos_list[index];
    // Continue if coordinate is out of the chess board.
    if (new_x < 0 || new_x > 7 || new_y < 0 || new_y > 7)
    {
      continue;
    }
    Piece *target_piece = board[new_x][new_y];
    // Normal move.
    if (target_piece->piece_type == PieceType::EMPTY)
    {
      possible_moves.emplace_back(x_coordinate, y_coordinate, new_x, new_y,
                                  knight_piece, first_move);
    }
    // Capture move.
    else if (target_piece->piece_color != knight_piece->piece_color)
    {
      possible_moves.emplace_back(x_coordinate, y_coordinate, new_x, new_y,
                                  knight_piece, target_piece, first_move);
    }
  }
}

void MoveGenerator::generate_bishop_move(BoardState &board_state,
                                         int x_coordinate, int y_coordinate,
                                         std::vector<Move> &possible_moves)
{
  // Each respective pair of x and y directions represent a diagonal.
  rook_bishop_move_helper(board_state, x_coordinate, y_coordinate, 1, 1,
                          possible_moves);
  rook_bishop_move_helper(board_state, x_coordinate, y_coordinate, 1, -1,
                          possible_moves);
  rook_bishop_move_helper(board_state, x_coordinate, y_coordinate, -1, 1,
                          possible_moves);
  rook_bishop_move_helper(board_state, x_coordinate, y_coordinate, -1, -1,
                          possible_moves);
}

void MoveGenerator::generate_rook_move(BoardState &board_state,
                                       int x_coordinate, int y_coordinate,
                                       std::vector<Move> &possible_moves)
{
  // Each respective pair of x and y directions represent horizontal or
  // vertical moves.
  rook_bishop_move_helper(board_state, x_coordinate, y_coordinate, 1, 0,
                          possible_moves);
  rook_bishop_move_helper(board_state, x_coordinate, y_coordinate, -1, 0,
                          possible_moves);
  rook_bishop_move_helper(board_state, x_coordinate, y_coordinate, 0, 1,
                          possible_moves);
  rook_bishop_move_helper(board_state, x_coordinate, y_coordinate, 0, -1,
                          possible_moves);
}

void MoveGenerator::generate_queen_move(BoardState &board_state,
                                        int x_coordinate, int y_coordinate,
                                        std::vector<Move> &possible_moves)
{
  // Queen moves are a combination of rook and bishop moves.
  generate_rook_move(board_state, x_coordinate, y_coordinate, possible_moves);
  generate_bishop_move(board_state, x_coordinate, y_coordinate, possible_moves);
}

// PRIVATE FUNCTIONS
void MoveGenerator::rook_bishop_move_helper(BoardState &board_state,
                                            int x_coordinate, int y_coordinate,
                                            int x_direction, int y_direction,
                                            std::vector<Move> &possible_moves)
{
  std::array<std::array<Piece *, 8>, 8> &board = board_state.chess_board;
  Piece *moving_piece = board[x_coordinate][y_coordinate];
  bool first_move = !moving_piece->piece_has_moved;

  int new_x = x_coordinate + x_direction;
  int new_y = y_coordinate + y_direction;
  for (; new_x >= 0 && new_x < 8 && new_y >= 0 && new_y < 8;
       new_x += x_direction, new_y += y_direction)
  {
    Piece *target_piece = board[new_x][new_y];
    if (target_piece->piece_type == PieceType::EMPTY)
    {
      possible_moves.emplace_back(x_coordinate, y_coordinate, new_x, new_y,
                                  moving_piece, first_move);
    }
    else if (target_piece->piece_color != moving_piece->piece_color)
    {
      possible_moves.emplace_back(x_coordinate, y_coordinate, new_x, new_y,
                                  moving_piece, target_piece, first_move);
      break;
    }
    else
    {
      break;
    }
  }
}
} // namespace engine::parts