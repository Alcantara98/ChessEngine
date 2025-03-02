#include "board_state.h"

namespace engine::parts
{
// CONSTUCTORS
BoardState::BoardState(PieceColor color_to_move) : color_to_move(color_to_move)
{
  initialize_zobrist_keys();
  reset_board();
}

BoardState::BoardState(chess_board_type &input_chess_board,
                       PieceColor color_to_move)
    : chess_board(input_chess_board), color_to_move(color_to_move)
{
  initialize_zobrist_keys();
}

// Deep copy constructor
BoardState::BoardState(const BoardState &other)
    : color_to_move(other.color_to_move),
      previous_move_stack(other.previous_move_stack),
      zobrist_keys(other.zobrist_keys),
      zobrist_side_to_move(other.zobrist_side_to_move)
{
  for (int x_coordinate = 0; x_coordinate < 8; ++x_coordinate)
  {
    for (int y_coordinate = 0; y_coordinate < 8; ++y_coordinate)
    {
      if (other.chess_board[x_coordinate][x_coordinate] != nullptr)
      {
        chess_board[x_coordinate][y_coordinate] =
            new Piece(*other.chess_board[x_coordinate][y_coordinate]);
      }
      else
      {
        chess_board[x_coordinate][y_coordinate] = nullptr;
      }
    }
  }
}

// Destructor
BoardState::~BoardState()
{
  for (int x_coordinate = 0; x_coordinate < 8; ++x_coordinate)
  {
    for (int y_coordinate = 0; y_coordinate < 8; ++y_coordinate)
    {
      if (chess_board[x_coordinate][y_coordinate] != nullptr &&
          chess_board[x_coordinate][y_coordinate] != &empty_piece)
      {
        delete chess_board[x_coordinate][y_coordinate];
        chess_board[x_coordinate][y_coordinate] = nullptr;
      }
    }
  }
}

// PUBLIC FUNCTIONS
void BoardState::reset_board()
{
  // Set empty squares.
  for (int y_coordinate = 2; y_coordinate < 6; ++y_coordinate)
  {
    for (int x_coordinate = 0; x_coordinate < 8; ++x_coordinate)
    {
      chess_board[x_coordinate][y_coordinate] = &empty_piece;
    }
  }
  // Set Pawns.
  for (int x_coordinate = 0; x_coordinate < 8; ++x_coordinate)
  {
    chess_board[x_coordinate][1] =
        new Piece(PieceType::PAWN, PieceColor::WHITE);
  }
  for (int x_coordinate = 0; x_coordinate < 8; ++x_coordinate)
  {
    chess_board[x_coordinate][6] =
        new Piece(PieceType::PAWN, PieceColor::BLACK);
  }
  // Set Rooks.
  chess_board[0][0] = new Piece(PieceType::ROOK, PieceColor::WHITE);
  chess_board[7][0] = new Piece(PieceType::ROOK, PieceColor::WHITE);
  chess_board[0][7] = new Piece(PieceType::ROOK, PieceColor::BLACK);
  chess_board[7][7] = new Piece(PieceType::ROOK, PieceColor::BLACK);
  // Set Knights.
  chess_board[1][0] = new Piece(PieceType::KNIGHT, PieceColor::WHITE);
  chess_board[6][0] = new Piece(PieceType::KNIGHT, PieceColor::WHITE);
  chess_board[1][7] = new Piece(PieceType::KNIGHT, PieceColor::BLACK);
  chess_board[6][7] = new Piece(PieceType::KNIGHT, PieceColor::BLACK);
  // Set Bishops.
  chess_board[2][0] = new Piece(PieceType::BISHOP, PieceColor::WHITE);
  chess_board[5][0] = new Piece(PieceType::BISHOP, PieceColor::WHITE);
  chess_board[2][7] = new Piece(PieceType::BISHOP, PieceColor::BLACK);
  chess_board[5][7] = new Piece(PieceType::BISHOP, PieceColor::BLACK);
  // Set Queens.
  chess_board[3][0] = new Piece(PieceType::QUEEN, PieceColor::WHITE);
  chess_board[3][7] = new Piece(PieceType::QUEEN, PieceColor::BLACK);
  // Set Kings.
  chess_board[4][0] = new Piece(PieceType::KING, PieceColor::WHITE);
  chess_board[4][7] = new Piece(PieceType::KING, PieceColor::BLACK);
}

void BoardState::print_board(PieceColor color)
{
  if (color == PieceColor::WHITE)
  {
    // Print board from white's perspective. (White at bottom)
    for (int y_coordinate = 7; y_coordinate >= 0; --y_coordinate)
    {
      for (int x_coordinate = 0; x_coordinate < 8; ++x_coordinate)
      {
        Piece *piece = chess_board[x_coordinate][y_coordinate];
        char piece_char = (piece->piece_color == PieceColor::WHITE)
                              ? white_piece_to_char_map.at(piece->piece_type)
                              : black_piece_to_char_map.at(piece->piece_type);
        printf("%c ", piece_char);
      }
      printf("\n");
    }
    printf("\n");
  }
  else
  {
    // Print board from black perspective. (Black at bottom)
    for (int y_coordinate = 0; y_coordinate < 8; ++y_coordinate)
    {
      for (int x_coordinate = 7; x_coordinate >= 0; --x_coordinate)
      {
        Piece *piece = chess_board[x_coordinate][y_coordinate];
        char piece_char = (piece->piece_color == PieceColor::WHITE)
                              ? white_piece_to_char_map.at(piece->piece_type)
                              : black_piece_to_char_map.at(piece->piece_type);
        printf("%c ", piece_char);
      }
      printf("\n");
    }
    printf("\n");
  }
}

void BoardState::apply_move(Move &move)
{
  if (move.pawn_moved_two_squares)
  {
    move.moving_piece->pawn_moved_two_squares = true;
  }

  if (move.capture_is_en_passant)
  {
    // Remove captured pawn.
    int captured_y_pos = (move.moving_piece->piece_color == PieceColor::WHITE)
                             ? move.to_y - 1
                             : move.to_y + 1;
    chess_board[move.to_x][captured_y_pos] = &empty_piece;
  }
  else if (move.moving_piece->piece_type == PieceType::KING)
  {
    // Castle move if king moved two squares.
    switch (move.to_x - move.from_x)
    {
    case 2:
      // King Side Castle.
      chess_board[7][move.to_y]->piece_has_moved = true;
      std::swap(chess_board[5][move.to_y], chess_board[7][move.to_y]);
      break;
    case -2:
      // Queen Side Castle.
      chess_board[0][move.to_y]->piece_has_moved = true;
      std::swap(chess_board[0][move.to_y], chess_board[3][move.to_y]);
      break;
    default:
      break;
    }
  }

  // Move moving piece to new square.
  std::swap(chess_board[move.from_x][move.from_y],
            chess_board[move.to_x][move.to_y]);

  if (move.promotion_piece_type != PieceType::EMPTY)
  {
    // If pawn is promoting, moving piece will become promotion piece type.
    chess_board[move.to_x][move.to_y] =
        new Piece(move.promotion_piece_type, move.moving_piece->piece_color);
  }

  if (move.captured_piece != nullptr && move.capture_is_en_passant == false)
  {
    // If capturing, original square will not be empty after swap.
    // Clear old square (point to empty_piece).
    chess_board[move.from_x][move.from_y] = &empty_piece;
  }

  if (move.first_move_of_moving_piece)
  {
    move.moving_piece->piece_has_moved = true;
  }

  // Update move color, it is now the other player's turn.
  color_to_move = (color_to_move == PieceColor::WHITE) ? PieceColor::BLACK
                                                       : PieceColor::WHITE;

  // Store move in previous moves stack for undoing moves.
  previous_move_stack.push(move);
}

void BoardState::undo_move()
{
  Move &move = previous_move_stack.top();
  if (move.pawn_moved_two_squares)
  {
    move.moving_piece->pawn_moved_two_squares = false;
  }

  if (move.capture_is_en_passant)
  {
    // Add captured pawn.
    int captured_y_pos = (move.moving_piece->piece_color == PieceColor::WHITE)
                             ? move.to_y - 1
                             : move.to_y + 1;
    chess_board[move.to_x][captured_y_pos] = move.captured_piece;
  }
  else if (move.moving_piece->piece_type == PieceType::KING)
  {
    // Castle move if king moved two squares.
    switch (move.to_x - move.from_x)
    {
    case 2:
      // King Side Castle.
      std::swap(chess_board[5][move.to_y], chess_board[7][move.to_y]);
      chess_board[7][move.to_y]->piece_has_moved = false;
      break;
    case -2:
      // Queen Side Castle.
      std::swap(chess_board[0][move.to_y], chess_board[3][move.to_y]);
      chess_board[0][move.to_y]->piece_has_moved = false;
      break;
    default:
      break;
    }
  }

  if (move.promotion_piece_type != PieceType::EMPTY)
  {
    // If pawn was promoted, pawn piece is replaced.
    // Add back the pawn piece, and delete the promotion piece.
    delete chess_board[move.to_x][move.to_y];
    chess_board[move.to_x][move.to_y] = move.moving_piece;
  }

  if (move.captured_piece != nullptr && move.capture_is_en_passant == false)
  {
    // If a piece was captured, add the piece back.
    chess_board[move.from_x][move.from_y] = move.captured_piece;
  }

  // Move piece back to original square.
  std::swap(chess_board[move.from_x][move.from_y],
            chess_board[move.to_x][move.to_y]);

  if (move.first_move_of_moving_piece)
  {
    move.moving_piece->piece_has_moved = false;
  }

  // Update move color, it is now the other player's turn.
  color_to_move = (color_to_move == PieceColor::WHITE) ? PieceColor::BLACK
                                                       : PieceColor::WHITE;

  // Remove move from moves stack, move is undone.
  previous_move_stack.pop();
}

void BoardState::apply_null_move()
{
  // Update move color, it is now the other player's turn.
  color_to_move = (color_to_move == PieceColor::WHITE) ? PieceColor::BLACK
                                                       : PieceColor::WHITE;
}

void BoardState::undo_null_move()
{
  // Update move color, it is now the other player's turn.
  color_to_move = (color_to_move == PieceColor::WHITE) ? PieceColor::BLACK
                                                       : PieceColor::WHITE;
}

auto BoardState::square_is_attacked(int x, int y,
                                    PieceColor color_being_attacked) -> bool
{
  // Check for pawn attacks
  int pawn_direction = (color_being_attacked == PieceColor::WHITE) ? 1 : -1;
  if (x > 0 && y + pawn_direction >= 0 && y + pawn_direction < 8)
  {
    if (chess_board[x - 1][y + pawn_direction]->piece_type == PieceType::PAWN &&
        chess_board[x - 1][y + pawn_direction]->piece_color !=
            color_being_attacked)
    {
      return true;
    }
  }
  if (x < 7 && y + pawn_direction >= 0 && y + pawn_direction < 8)
  {
    if (chess_board[x + 1][y + pawn_direction]->piece_type == PieceType::PAWN &&
        chess_board[x + 1][y + pawn_direction]->piece_color !=
            color_being_attacked)
    {
      return true;
    }
  }

  // Check for knight attacks
  std::vector<std::pair<int, int>> knight_moves = {
      {x - 2, y - 1}, {x - 2, y + 1}, {x - 1, y - 2}, {x - 1, y + 2},
      {x + 1, y - 2}, {x + 1, y + 2}, {x + 2, y - 1}, {x + 2, y + 1}};
  for (auto &move : knight_moves)
  {
    int new_x = move.first;
    int new_y = move.second;
    if (new_x >= 0 && new_x < 8 && new_y >= 0 && new_y < 8)
    {
      if (chess_board[new_x][new_y]->piece_type == PieceType::KNIGHT &&
          chess_board[new_x][new_y]->piece_color != color_being_attacked)
      {
        return true;
      }
    }
  }

  // Check for rook/queen attacks (horizontal and vertical)
  std::vector<std::pair<int, int>> directions = {
      {1, 0}, {-1, 0}, {0, 1}, {0, -1}};
  for (auto &direction : directions)
  {
    int new_x = x + direction.first;
    int new_y = y + direction.second;
    while (new_x >= 0 && new_x < 8 && new_y >= 0 && new_y < 8)
    {
      Piece *target_piece = chess_board[new_x][new_y];
      if (target_piece->piece_type != PieceType::EMPTY)
      {
        if ((target_piece->piece_type == PieceType::ROOK ||
             target_piece->piece_type == PieceType::QUEEN) &&
            target_piece->piece_color != color_being_attacked)
        {
          return true;
        }
        break;
      }
      new_x += direction.first;
      new_y += direction.second;
    }
  }

  // Check for bishop/queen attacks (diagonal)
  directions = {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
  for (auto &direction : directions)
  {
    int new_x = x + direction.first;
    int new_y = y + direction.second;
    while (new_x >= 0 && new_x < 8 && new_y >= 0 && new_y < 8)
    {
      Piece *target_piece = chess_board[new_x][new_y];
      if (target_piece->piece_type != PieceType::EMPTY)
      {
        if ((target_piece->piece_type == PieceType::BISHOP ||
             target_piece->piece_type == PieceType::QUEEN) &&
            target_piece->piece_color != color_being_attacked)
        {
          return true;
        }
        break;
      }
      new_x += direction.first;
      new_y += direction.second;
    }
  }

  // Check for king attacks
  std::vector<std::pair<int, int>> king_moves = {
      {x - 1, y - 1}, {x - 1, y},     {x - 1, y + 1}, {x, y - 1},
      {x, y + 1},     {x + 1, y - 1}, {x + 1, y},     {x + 1, y + 1}};
  for (auto &move : king_moves)
  {
    int new_x = move.first;
    int new_y = move.second;
    if (new_x >= 0 && new_x < 8 && new_y >= 0 && new_y < 8)
    {
      if (chess_board[new_x][new_y]->piece_type == PieceType::KING &&
          chess_board[new_x][new_y]->piece_color != color_being_attacked)
      {
        return true;
      }
    }
  }

  return false;
}

auto BoardState::king_is_checked(PieceColor color_of_king) -> bool
{
  for (int x_coordinate = 0; x_coordinate < 8; ++x_coordinate)
  {
    for (int y_coordinate = 0; y_coordinate < 8; ++y_coordinate)
    {
      Piece *test_piece = chess_board[x_coordinate][y_coordinate];
      if (test_piece->piece_type == PieceType::KING &&
          test_piece->piece_color == color_of_king)
      {
        return BoardState::square_is_attacked(x_coordinate, y_coordinate,
                                              color_of_king);
      }
    }
  }
  return false;
}

auto BoardState::compute_zobrist_hash() const -> size_t
{
  size_t hash = 0;

  for (int y_coordinate = 0; y_coordinate < 8; ++y_coordinate)
  {
    for (int x_coordinate = 0; x_coordinate < 8; ++x_coordinate)
    {
      Piece *piece = chess_board[x_coordinate][y_coordinate];
      if (piece->piece_type != PieceType::EMPTY)
      {
        int piece_index = static_cast<int>(piece->piece_type) - 1;
        int color_index = (piece->piece_color == PieceColor::WHITE) ? 0 : 1;
        hash ^= zobrist_keys[y_coordinate * 8 + x_coordinate][piece_index]
                            [color_index];
      }
    }
  }

  if (color_to_move == PieceColor::BLACK)
  {
    hash ^= zobrist_side_to_move;
  }

  return hash;
}

// PRIVATE FUNCTIONS
void BoardState::initialize_zobrist_keys()
{
  std::mt19937_64 rng(0); // Use a fixed seed for reproducibility
  std::uniform_int_distribution<size_t> dist;

  for (int square = 0; square < 64; ++square)
  {
    for (int piece = 0; piece < 6; ++piece)
    {
      zobrist_keys[square][piece][0] = dist(rng); // White piece
      zobrist_keys[square][piece][1] = dist(rng); // Black piece
    }
  }
  zobrist_side_to_move = dist(rng);
}
} // namespace engine::parts
