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
          generate_pawn_moves(board_state, x_position, y_position,
                              possible_moves);
          break;
        case PieceType::ROOK:
          generate_rook_moves(board_state, x_position, y_position,
                              possible_moves);
          break;
        case PieceType::KNIGHT:
          generate_knight_moves(board_state, x_position, y_position,
                                possible_moves);
          break;
        case PieceType::BISHOP:
          generate_bishop_moves(board_state, x_position, y_position,
                                possible_moves);
          break;
        case PieceType::QUEEN:
          generate_queen_moves(board_state, x_position, y_position,
                               possible_moves);
          break;
        case PieceType::KING:
          generate_king_moves(board_state, x_position, y_position,
                              possible_moves);
          generate_castle_king_moves(board_state, x_position, y_position,
                                     possible_moves);
          break;
        default:
          // Empty square.
          break;
        }
      }
    }
  }
  // Put captures at the front of the list.
  std::stable_partition(possible_moves.begin(), possible_moves.end(),
                        [](const Move &move)
                        { return move.captured_piece != nullptr; });
  return std::move(possible_moves);
}

// STATIC FUNCTIONS

void generate_pawn_moves(BoardState &board_state, int x_position,
                         int y_position, std::vector<Move> &possible_moves)
{
  chess_board_type &chess_board = board_state.chess_board;
  Piece *pawn_piece = chess_board[x_position][y_position];
  bool first_move = !pawn_piece->piece_has_moved;

  // Create pawn moves.
  int pawn_direction;
  int promotion_rank;
  if (pawn_piece->piece_color == PieceColor::WHITE)
  {
    pawn_direction = POSITIVE_DIRECTION;
    promotion_rank = Y_MAX;
  }
  else
  {
    pawn_direction = NEGATIVE_DIRECTION;
    promotion_rank = Y_MIN;
  }

  generate_normal_pawn_moves(chess_board, x_position, y_position,
                             possible_moves, pawn_piece, pawn_direction,
                             first_move, promotion_rank);

  generate_pawn_capture_moves(chess_board, x_position, y_position,
                              possible_moves, pawn_piece, pawn_direction,
                              first_move, promotion_rank);

  if (!board_state.previous_move_stack.empty())
  {
    generate_en_passant_pawn_capture_moves(
        chess_board, x_position, y_position, possible_moves, pawn_piece,
        pawn_direction, first_move, board_state.previous_move_stack.top());
  }
}

void generate_normal_pawn_moves(chess_board_type &chess_board, int x_position,
                                int y_position,
                                std::vector<Move> &possible_moves,
                                Piece *pawn_piece, int pawn_direction,
                                bool first_move, int promotion_rank)
{
  // One square move forward.
  // Check if the square in front of the pawn is empty.
  int new_y_position = y_position + pawn_direction;
  if (new_y_position >= Y_MIN && new_y_position <= Y_MAX)
  {
    if (chess_board[x_position][new_y_position]->piece_type == PieceType::EMPTY)
    {

      if (new_y_position == promotion_rank)
      {
        // Promotion moves.
        for (auto piece_type : {PieceType::QUEEN, PieceType::BISHOP,
                                PieceType::KNIGHT, PieceType::ROOK})
        {
          possible_moves.emplace_back(x_position, y_position, x_position,
                                      new_y_position, pawn_piece, piece_type);
        }
      }
      else
      {
        // Normal move.
        possible_moves.emplace_back(x_position, y_position, x_position,
                                    new_y_position, pawn_piece, first_move);
      }
    }
    // Two square move forward.
    // Check if both squares in front of the pawn are empty.
    int new_y_position_two_squares = y_position + (2 * pawn_direction);
    if (first_move &&
        chess_board[x_position][new_y_position]->piece_type ==
            PieceType::EMPTY &&
        chess_board[x_position][new_y_position_two_squares]->piece_type ==
            PieceType::EMPTY)
    {
      possible_moves.emplace_back(x_position, y_position, x_position,
                                  new_y_position_two_squares, pawn_piece, true,
                                  true, x_position, new_y_position_two_squares);
    }
  }
}

void generate_pawn_capture_moves(chess_board_type &chess_board, int x_position,
                                 int y_position,
                                 std::vector<Move> &possible_moves,
                                 Piece *pawn_piece, int pawn_direction,
                                 bool first_move, int promotion_rank)
{
  // Pawn can capture left and right.
  for (int capture_direction : {NEGATIVE_DIRECTION, POSITIVE_DIRECTION})
  {
    int new_x_position = x_position + capture_direction;
    int new_y_position = y_position + pawn_direction;
    if (new_x_position >= X_MIN && new_x_position <= X_MAX &&
        new_y_position >= Y_MIN && new_y_position <= Y_MAX)
    {
      Piece *captured_piece = chess_board[new_x_position][new_y_position];
      if (captured_piece->piece_type != PieceType::EMPTY &&
          captured_piece->piece_color != pawn_piece->piece_color)
      {
        if (y_position + pawn_direction == promotion_rank)
        {
          // Add promotion moves.
          for (auto piece_type : {PieceType::QUEEN, PieceType::BISHOP,
                                  PieceType::KNIGHT, PieceType::ROOK})
          {
            possible_moves.emplace_back(x_position, y_position, new_x_position,
                                        new_y_position, pawn_piece,
                                        captured_piece, piece_type);
          }
        }
        else
        {
          // Normal capture move.
          possible_moves.emplace_back(x_position, y_position, new_x_position,
                                      new_y_position, pawn_piece,
                                      captured_piece, first_move);
        }
      }
    }
  }
}

void generate_en_passant_pawn_capture_moves(chess_board_type &chess_board,
                                            int x_position, int y_position,
                                            std::vector<Move> &possible_moves,
                                            Piece *pawn_piece,
                                            int pawn_direction, bool first_move,
                                            Move &previous_move)
{
  // En-passant moves can only be made on the 5th rank for white and 4th rank
  // for black.
  if ((y_position == Y5_RANK && pawn_piece->piece_color == PieceColor::WHITE) ||
      (y_position == Y4_RANK && pawn_piece->piece_color == PieceColor::BLACK))
  {
    // Pawn can capture, en-passant, left and right.
    for (int capture_direction : {NEGATIVE_DIRECTION, POSITIVE_DIRECTION})
    {
      int new_x_position = x_position + capture_direction;
      int new_y_position = y_position + pawn_direction;
      if (new_x_position >= X_MIN && new_x_position <= X_MAX)
      {
        Piece *captured_piece = chess_board[new_x_position][y_position];
        if (captured_piece->piece_type == PieceType::PAWN &&
            previous_move.pawn_moved_two_squares_to_x == new_x_position &&
            previous_move.pawn_moved_two_squares_to_y == y_position &&
            captured_piece->piece_color != pawn_piece->piece_color &&
            chess_board[new_x_position][new_y_position]->piece_type ==
                PieceType::EMPTY)
        {

          possible_moves.emplace_back(x_position, y_position, new_x_position,
                                      new_y_position, pawn_piece,
                                      captured_piece, first_move, true);
        }
      }
    }
  }
}

void generate_king_moves(BoardState &board_state, int x_position,
                         int y_position, std::vector<Move> &possible_moves)
{
  chess_board_type &board = board_state.chess_board;
  Piece *king_piece = board[x_position][y_position];
  bool first_move = !king_piece->piece_has_moved;

  for (auto direction : KING_MOVES)
  {
    int new_x = x_position + direction[0];
    int new_y = y_position + direction[1];
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

void generate_castle_king_moves(BoardState &board_state, int x_position,
                                int y_position,
                                std::vector<Move> &possible_moves)
{
  chess_board_type &chess_board = board_state.chess_board;
  Piece *king_piece = chess_board[x_position][y_position];
  bool first_move = !king_piece->piece_has_moved;
  // Check if the king is not in check and has not moved.
  if (first_move && !board_state.square_is_attacked(x_position, y_position,
                                                    king_piece->piece_color))
  {
    // Castle king side.
    Piece *potential_rook_piece = chess_board[X_MAX][y_position];
    if (can_castle(board_state, king_piece, y_position, potential_rook_piece,
                   {XF_FILE, XG_FILE}))
    {
      possible_moves.emplace_back(x_position, y_position, x_position + 2,
                                  y_position, king_piece, first_move, false);
    }

    // Castle queen side.
    potential_rook_piece = chess_board[0][y_position];
    if (can_castle(board_state, king_piece, y_position, potential_rook_piece,
                   {XB_FILE, XC_FILE, XD_FILE}))
    {
      possible_moves.emplace_back(x_position, y_position, x_position - 2,
                                  y_position, king_piece, first_move, true);
    }
  }
}

auto can_castle(BoardState &board_state, Piece *king_piece, int y_position,
                Piece *potential_rook_piece,
                const std::vector<int> &castle_path) -> bool
{
  // Check if the piece is a rook and has not moved.
  if (potential_rook_piece->piece_type != PieceType::ROOK &&
      potential_rook_piece->piece_has_moved)
  {
    return false;
  }
  // Check if the squares between the king and rook are empty and not
  // attacked.
  for (int file : castle_path)
  {
    if (board_state.chess_board[file][y_position]->piece_type !=
            PieceType::EMPTY ||
        board_state.square_is_attacked(file, y_position,
                                       king_piece->piece_color))
    {
      return false;
    }
  }
  return true;
}

void generate_knight_moves(BoardState &board_state, int x_position,
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

void generate_bishop_moves(BoardState &board_state, int x_position,
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

void generate_rook_moves(BoardState &board_state, int x_position,
                         int y_position, std::vector<Move> &possible_moves)
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

void generate_queen_moves(BoardState &board_state, int x_position,
                          int y_position, std::vector<Move> &possible_moves)
{
  // Queen moves are a combination of rook and bishop moves.
  generate_rook_moves(board_state, x_position, y_position, possible_moves);
  generate_bishop_moves(board_state, x_position, y_position, possible_moves);
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