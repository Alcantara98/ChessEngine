#include "board_state.h"

namespace engine::parts
{
// CONSTRUCTORS

BoardState::BoardState(PieceColor color_to_move) : color_to_move(color_to_move)
{
  initialize_zobrist_keys();
  setup_board();
}

BoardState::BoardState(chess_board_type &input_chess_board,
                       PieceColor color_to_move)
    : chess_board(input_chess_board), color_to_move(color_to_move)
{
  initialize_zobrist_keys();
}

BoardState::BoardState(const BoardState &other)
    : color_to_move(other.color_to_move),
      previous_move_stack(other.previous_move_stack),
      zobrist_keys(other.zobrist_keys),
      zobrist_side_to_move(other.zobrist_side_to_move)
{
  for (int x_position = X_MIN; x_position <= X_MAX; ++x_position)
  {
    for (int y_position = Y_MIN; y_position <= Y_MAX; ++y_position)
    {
      if (other.chess_board[x_position][x_position] != nullptr)
      {
        chess_board[x_position][y_position] =
            new Piece(*other.chess_board[x_position][y_position]);
      }
      else
      {
        chess_board[x_position][y_position] = nullptr;
      }
    }
  }
}

BoardState::~BoardState() { clear_pointers(); }

// PUBLIC FUNCTIONS

void BoardState::setup_board()
{
  // Set empty squares.
  for (int y_position = Y2_RANK; y_position <= Y6_RANK; ++y_position)
  {
    for (int x_position = X_MIN; x_position <= X_MAX; ++x_position)
    {
      chess_board[x_position][y_position] = &empty_piece;
    }
  }
  // Set Pawns.
  for (int x_position = X_MIN; x_position <= X_MAX; ++x_position)
  {
    chess_board[x_position][Y2_RANK] =
        new Piece(PieceType::PAWN, PieceColor::WHITE);
    chess_board[x_position][Y7_RANK] =
        new Piece(PieceType::PAWN, PieceColor::BLACK);
  }
  // Set Rooks.
  chess_board[XA_FILE][Y1_RANK] = new Piece(PieceType::ROOK, PieceColor::WHITE);
  chess_board[XH_FILE][Y1_RANK] = new Piece(PieceType::ROOK, PieceColor::WHITE);
  chess_board[XA_FILE][Y8_RANK] = new Piece(PieceType::ROOK, PieceColor::BLACK);
  chess_board[XH_FILE][Y8_RANK] = new Piece(PieceType::ROOK, PieceColor::BLACK);
  // Set Knights.
  chess_board[XB_FILE][Y1_RANK] =
      new Piece(PieceType::KNIGHT, PieceColor::WHITE);
  chess_board[XG_FILE][Y1_RANK] =
      new Piece(PieceType::KNIGHT, PieceColor::WHITE);
  chess_board[XB_FILE][Y8_RANK] =
      new Piece(PieceType::KNIGHT, PieceColor::BLACK);
  chess_board[XG_FILE][Y8_RANK] =
      new Piece(PieceType::KNIGHT, PieceColor::BLACK);
  // Set Bishops.
  chess_board[XC_FILE][Y1_RANK] =
      new Piece(PieceType::BISHOP, PieceColor::WHITE);
  chess_board[XF_FILE][Y1_RANK] =
      new Piece(PieceType::BISHOP, PieceColor::WHITE);
  chess_board[XC_FILE][Y8_RANK] =
      new Piece(PieceType::BISHOP, PieceColor::BLACK);
  chess_board[XF_FILE][Y8_RANK] =
      new Piece(PieceType::BISHOP, PieceColor::BLACK);
  // Set Queens.
  chess_board[XD_FILE][Y1_RANK] =
      new Piece(PieceType::QUEEN, PieceColor::WHITE);
  chess_board[XD_FILE][Y8_RANK] =
      new Piece(PieceType::QUEEN, PieceColor::BLACK);
  // Set Kings.
  chess_board[XE_FILE][Y1_RANK] = new Piece(PieceType::KING, PieceColor::WHITE);
  chess_board[XE_FILE][Y8_RANK] = new Piece(PieceType::KING, PieceColor::BLACK);
}

void BoardState::reset_board()
{
  while (!previous_move_stack.empty())
  {
    undo_move();
  }
  clear_pointers();
  setup_board();
}

void BoardState::print_board(PieceColor color)
{
  printf("\n");
  if (color == PieceColor::WHITE)
  {
    // Print board from white's perspective. (White at bottom)
    for (int y_position = Y_MAX; y_position >= Y_MIN; --y_position)
    {
      for (int x_position = X_MIN; x_position <= X_MAX; ++x_position)
      {
        Piece *piece = chess_board[x_position][y_position];
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
    for (int y_position = Y_MIN; y_position <= Y_MAX; ++y_position)
    {
      for (int x_position = X_MAX; x_position >= X_MIN; --x_position)
      {
        Piece *piece = chess_board[x_position][y_position];
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
    // Castle move if king moved two squares, so move rooks.
    switch (move.to_x - move.from_x)
    {
    case 2:
      // King Side Castle.
      chess_board[XH_FILE][move.to_y]->piece_has_moved = true;
      std::swap(chess_board[XH_FILE][move.to_y],
                chess_board[XF_FILE][move.to_y]);
      break;
    case -2:
      // Queen Side Castle.
      chess_board[XA_FILE][move.to_y]->piece_has_moved = true;
      std::swap(chess_board[XA_FILE][move.to_y],
                chess_board[XD_FILE][move.to_y]);
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

  if (move.captured_piece != nullptr && !move.capture_is_en_passant)
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
  if (previous_move_stack.empty())
  {
    return;
  }
  Move &move = previous_move_stack.top();
  if (move.capture_is_en_passant)
  {
    int captured_y_pos = (move.moving_piece->piece_color == PieceColor::WHITE)
                             ? move.to_y - 1
                             : move.to_y + 1;
    // Add captured pawn.
    chess_board[move.to_x][captured_y_pos] = move.captured_piece;
  }
  else if (move.moving_piece->piece_type == PieceType::KING)
  {
    // Castle move if king moved two squares.
    switch (move.to_x - move.from_x)
    {
    case 2:
      // King Side Castle.
      std::swap(chess_board[XF_FILE][move.to_y],
                chess_board[XH_FILE][move.to_y]);
      chess_board[XH_FILE][move.to_y]->piece_has_moved = false;
      break;
    case -2:
      // Queen Side Castle.
      std::swap(chess_board[XD_FILE][move.to_y],
                chess_board[XA_FILE][move.to_y]);
      chess_board[XA_FILE][move.to_y]->piece_has_moved = false;
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

  if (move.captured_piece != nullptr && !move.capture_is_en_passant)
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

auto BoardState::square_is_attacked(int x_position, int y_position,
                                    PieceColor color_being_attacked) -> bool
{
  return square_is_attacked_by_pawn(x_position, y_position,
                                    color_being_attacked) ||
         square_is_attacked_by_knight(x_position, y_position,
                                      color_being_attacked) ||
         square_is_attacked_by_rook_or_queen(x_position, y_position,
                                             color_being_attacked) ||
         square_is_attacked_by_bishop_or_queen(x_position, y_position,
                                               color_being_attacked) ||
         square_is_attacked_by_king(x_position, y_position,
                                    color_being_attacked);
}

auto BoardState::king_is_checked(PieceColor &color_of_king) -> bool
{
  for (int x_position = X_MIN; x_position <= X_MAX; ++x_position)
  {
    for (int y_position = Y_MIN; y_position <= Y_MAX; ++y_position)
    {
      Piece *test_piece = chess_board[x_position][y_position];
      if (test_piece->piece_type == PieceType::KING &&
          test_piece->piece_color == color_of_king)
      {
        return BoardState::square_is_attacked(x_position, y_position,
                                              color_of_king);
      }
    }
  }
  return false;
}

auto BoardState::move_leaves_king_in_check(Move &move) -> bool
{
  apply_move(move);
  bool king_is_checked_after_move =
      king_is_checked(move.moving_piece->piece_color);
  undo_move();
  return king_is_checked_after_move;
}

auto BoardState::compute_zobrist_hash() const -> size_t
{
  size_t hash = 0;

  for (int y_position = Y_MIN; y_position <= Y_MAX; ++y_position)
  {
    for (int x_position = X_MIN; x_position <= X_MAX; ++x_position)
    {
      Piece *piece = chess_board[x_position][y_position];
      if (piece->piece_type != PieceType::EMPTY)
      {
        int piece_index = static_cast<int>(piece->piece_type) - 1;
        int color_index = (piece->piece_color == PieceColor::WHITE) ? 0 : 1;
        hash ^= zobrist_keys[y_position * BOARD_WIDTH + x_position][piece_index]
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

void BoardState::clear_pointers()
{
  for (int x_position = X_MIN; x_position <= Y_MAX; ++x_position)
  {
    for (int y_position = Y_MIN; y_position <= Y_MAX; ++y_position)
    {
      if (chess_board[x_position][y_position] != nullptr &&
          chess_board[x_position][y_position] != &empty_piece)
      {
        delete chess_board[x_position][y_position];
        chess_board[x_position][y_position] = nullptr;
      }
    }
  }
}

void BoardState::initialize_zobrist_keys()
{
  std::mt19937_64 rng(0); // Use a fixed seed for reproducibility
  std::uniform_int_distribution<size_t> dist;

  for (int square = 0; square < NUM_OF_SQUARES; ++square)
  {
    for (int piece = 0; piece < NUM_OF_PIECE_TYPES; ++piece)
    {
      zobrist_keys[square][piece][0] = dist(rng); // White piece
      zobrist_keys[square][piece][1] = dist(rng); // Black piece
    }
  }
  zobrist_side_to_move = dist(rng);
}

auto BoardState::square_is_attacked_by_pawn(
    int &x_position, int &y_position, PieceColor &color_being_attacked) -> bool
{
  int pawn_direction = (color_being_attacked == PieceColor::WHITE)
                           ? POSITIVE_DIRECTION
                           : NEGATIVE_DIRECTION;
  // Check for pawn attacks in negative x direction.
  if (x_position > X_MIN && y_position + pawn_direction >= Y_MIN &&
      y_position + pawn_direction <= Y_MAX)
  {
    if (chess_board[x_position - 1][y_position + pawn_direction]->piece_type ==
            PieceType::PAWN &&
        chess_board[x_position - 1][y_position + pawn_direction]->piece_color !=
            color_being_attacked)
    {
      return true;
    }
  }
  // Check for pawn attacks in positive x direction.
  if (x_position < X_MAX && y_position + pawn_direction >= Y_MIN &&
      y_position + pawn_direction <= Y_MAX)
  {
    if (chess_board[x_position + 1][y_position + pawn_direction]->piece_type ==
            PieceType::PAWN &&
        chess_board[x_position + 1][y_position + pawn_direction]->piece_color !=
            color_being_attacked)
    {
      return true;
    }
  }
  return false;
}

auto BoardState::square_is_attacked_by_knight(
    int &x_position, int &y_position, PieceColor &color_being_attacked) -> bool
{
  return std::any_of(
      KNIGHT_MOVES.begin(), KNIGHT_MOVES.end(),
      [&](const auto &direction)
      {
        int new_x = x_position + direction[0];
        int new_y = y_position + direction[1];
        return new_x >= X_MIN && new_x <= X_MAX && new_y >= Y_MIN &&
               new_y <= Y_MAX &&
               chess_board[new_x][new_y]->piece_type == PieceType::KNIGHT &&
               chess_board[new_x][new_y]->piece_color != color_being_attacked;
      });
}

auto BoardState::square_is_attacked_by_rook_or_queen(
    int &x_position, int &y_position, PieceColor &color_being_attacked) -> bool
{
  for (const auto &direction : ROOK_DIRECTIONS)
  {
    int new_x = x_position + direction[0];
    int new_y = y_position + direction[1];
    while (new_x >= X_MIN && new_x <= X_MAX && new_y >= Y_MIN && new_y < Y_MAX)
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
        // Stop if a piece is blocking the way.
        break;
      }
      new_x += direction[0];
      new_y += direction[1];
    }
  }
  return false;
}

auto BoardState::square_is_attacked_by_bishop_or_queen(
    int &x_position, int &y_position, PieceColor &color_being_attacked) -> bool
{
  for (const auto &direction : BISHOP_DIRECTIONS)
  {
    int new_x = x_position + direction[0];
    int new_y = y_position + direction[1];
    while (new_x >= XA_FILE && new_x <= XH_FILE && new_y >= Y1_RANK &&
           new_y <= Y8_RANK)
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
        // Stop if a piece is blocking the way.
        break;
      }
      new_x += direction[0];
      new_y += direction[1];
    }
  }
  return false;
}

auto BoardState::square_is_attacked_by_king(
    int &x_position, int &y_position, PieceColor &color_being_attacked) -> bool
{
  // Check for king attacks.
  return std::any_of(
      KING_MOVES.begin(), KING_MOVES.end(),
      [&](const auto &direction)
      {
        int new_x = direction[0];
        int new_y = direction[1];
        return new_x >= X_MIN && new_x <= X_MAX && new_y >= Y_MIN &&
               new_y <= Y_MAX &&
               chess_board[new_x][new_y]->piece_type == PieceType::KING &&
               chess_board[new_x][new_y]->piece_color != color_being_attacked;
      });
}
} // namespace engine::parts
