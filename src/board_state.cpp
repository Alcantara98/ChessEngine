#include "board_state.h"

namespace engine::parts
{
// CONSTRUCTORS

BoardState::BoardState(PieceColor color_to_move) : color_to_move(color_to_move)
{
  initialize_zobrist_keys();
  setup_default_board();
  add_current_state_to_visited_states();
}

BoardState::BoardState(const BoardState &other)
    : color_to_move(other.color_to_move),
      previous_move_stack(other.previous_move_stack),
      zobrist_keys(other.zobrist_keys),
      zobrist_side_to_move(other.zobrist_side_to_move),
      white_king_is_alive(other.white_king_is_alive),
      black_king_is_alive(other.black_king_is_alive),
      queens_on_board(other.queens_on_board),
      number_of_main_pieces_left(other.number_of_main_pieces_left),
      is_end_game(other.is_end_game),
      visisted_states_hash_map(other.visisted_states_hash_map),
      visisted_states_hash_stack(other.visisted_states_hash_stack),
      empty_piece(other.empty_piece),
      white_king_x_file(other.white_king_x_file),
      white_king_y_rank(other.white_king_y_rank),
      black_king_x_file(other.black_king_x_file),
      black_king_y_rank(other.black_king_y_rank),
      white_has_castled(other.white_has_castled),
      black_has_castled(other.black_has_castled)
{
  for (int x_file = X_MIN; x_file <= X_MAX; ++x_file)
  {
    for (int y_rank = Y_MIN; y_rank <= Y_MAX; ++y_rank)
    {
      if (other.chess_board[x_file][x_file] != nullptr)
      {
        chess_board[x_file][y_rank] =
            new Piece(*other.chess_board[x_file][y_rank]);
      }
      else
      {
        chess_board[x_file][y_rank] = nullptr;
      }
    }
  }
  // Update pointers in piece_list to point to the new pieces.
  update_pieces_list();
}

BoardState::~BoardState() { clear_pointers(); }

// PUBLIC FUNCTIONS

void BoardState::setup_default_board()
{
  // Set empty squares.
  for (int y_rank = Y2_RANK; y_rank <= Y6_RANK; ++y_rank)
  {
    for (int x_file = X_MIN; x_file <= X_MAX; ++x_file)
    {
      chess_board[x_file][y_rank] = &empty_piece;
    }
  }
  // Set Pawns.
  for (int x_file = X_MIN; x_file <= X_MAX; ++x_file)
  {
    chess_board[x_file][Y2_RANK] =
        new Piece(x_file, Y2_RANK, PieceType::PAWN, PieceColor::WHITE);
    chess_board[x_file][Y7_RANK] =
        new Piece(x_file, Y7_RANK, PieceType::PAWN, PieceColor::BLACK);
  }
  // Set Rooks.
  chess_board[XA_FILE][Y1_RANK] =
      new Piece(XA_FILE, Y1_RANK, PieceType::ROOK, PieceColor::WHITE);
  chess_board[XH_FILE][Y1_RANK] =
      new Piece(XH_FILE, Y1_RANK, PieceType::ROOK, PieceColor::WHITE);
  chess_board[XA_FILE][Y8_RANK] =
      new Piece(XA_FILE, Y8_RANK, PieceType::ROOK, PieceColor::BLACK);
  chess_board[XH_FILE][Y8_RANK] =
      new Piece(XH_FILE, Y8_RANK, PieceType::ROOK, PieceColor::BLACK);
  // Set Knights.
  chess_board[XB_FILE][Y1_RANK] =
      new Piece(XB_FILE, Y1_RANK, PieceType::KNIGHT, PieceColor::WHITE);
  chess_board[XG_FILE][Y1_RANK] =
      new Piece(XG_FILE, Y1_RANK, PieceType::KNIGHT, PieceColor::WHITE);
  chess_board[XB_FILE][Y8_RANK] =
      new Piece(XB_FILE, Y8_RANK, PieceType::KNIGHT, PieceColor::BLACK);
  chess_board[XG_FILE][Y8_RANK] =
      new Piece(XG_FILE, Y8_RANK, PieceType::KNIGHT, PieceColor::BLACK);
  // Set Bishops.
  chess_board[XC_FILE][Y1_RANK] =
      new Piece(XC_FILE, Y1_RANK, PieceType::BISHOP, PieceColor::WHITE);
  chess_board[XF_FILE][Y1_RANK] =
      new Piece(XF_FILE, Y1_RANK, PieceType::BISHOP, PieceColor::WHITE);
  chess_board[XC_FILE][Y8_RANK] =
      new Piece(XC_FILE, Y8_RANK, PieceType::BISHOP, PieceColor::BLACK);
  chess_board[XF_FILE][Y8_RANK] =
      new Piece(XF_FILE, Y8_RANK, PieceType::BISHOP, PieceColor::BLACK);
  // Set Queens.
  chess_board[XD_FILE][Y1_RANK] =
      new Piece(XD_FILE, Y1_RANK, PieceType::QUEEN, PieceColor::WHITE);
  chess_board[XD_FILE][Y8_RANK] =
      new Piece(XD_FILE, Y8_RANK, PieceType::QUEEN, PieceColor::BLACK);
  // Set Kings.
  chess_board[XE_FILE][Y1_RANK] =
      new Piece(XE_FILE, Y1_RANK, PieceType::KING, PieceColor::WHITE);
  chess_board[XE_FILE][Y8_RANK] =
      new Piece(XE_FILE, Y8_RANK, PieceType::KING, PieceColor::BLACK);

  update_pieces_list();
  add_current_state_to_visited_states();
}

void BoardState::reset_board()
{
  // Empty previous move stack.
  while (!previous_move_stack.empty())
  {
    undo_move();
  }

  // Clear visited states hash map and stack.
  visisted_states_hash_map.clear();
  while (!visisted_states_hash_stack.empty())
  {
    visisted_states_hash_stack.pop();
  }

  color_to_move = PieceColor::WHITE;
  white_king_is_alive = true;
  black_king_is_alive = true;
  queens_on_board = INITIAL_QUEENS_COUNT;
  number_of_main_pieces_left = INITIAL_MAIN_PIECES_COUNT;
  white_king_y_rank = Y1_RANK;
  white_king_x_file = XE_FILE;
  black_king_y_rank = Y8_RANK;
  black_king_x_file = XE_FILE;
  white_has_castled = false;
  black_has_castled = false;
  is_end_game = false;

  clear_pointers();
  setup_default_board();
}

void BoardState::print_board(PieceColor color)
{
  printf("\n");
  if (color == PieceColor::WHITE)
  {
    // Print board from white's perspective. (White at bottom)
    for (int y_rank = Y_MAX; y_rank >= Y_MIN; --y_rank)
    {
      for (int x_file = X_MIN; x_file <= X_MAX; ++x_file)
      {
        Piece *piece = chess_board[x_file][y_rank];
        char piece_char = PIECE_TYPE_TO_CHAR.at(piece->piece_type);
        if (piece->piece_color == PieceColor::WHITE)
        {
          piece_char = std::toupper(piece_char);
        }
        printf("%c ", piece_char);
      }
      printf("\n");
    }
    printf("\n");
  }
  else
  {
    // Print board from black perspective. (Black at bottom)
    for (int y_rank = Y_MIN; y_rank <= Y_MAX; ++y_rank)
    {
      for (int x_file = X_MAX; x_file >= X_MIN; --x_file)
      {
        Piece *piece = chess_board[x_file][y_rank];
        char piece_char = PIECE_TYPE_TO_CHAR.at(piece->piece_type);
        if (piece->piece_color == PieceColor::WHITE)
        {
          piece_char = std::toupper(piece_char);
        }
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
    // Clear old square (point to empty_piece).
    chess_board[move.to_x][move.from_y] = &empty_piece;
  }
  else if (move.moving_piece->piece_type == PieceType::KING)
  {
    // Keep track of the king's positions for evaluation.
    if (move.moving_piece->piece_color == PieceColor::WHITE)
    {
      white_king_x_file = move.to_x;
      white_king_y_rank = move.to_y;
    }
    else
    {
      black_king_x_file = move.to_x;
      black_king_y_rank = move.to_y;
    }

    int king_move_distance = move.to_x - move.from_x;
    if (king_move_distance == 2 || king_move_distance == -2)
    {
      // Keep track of castling for evaluation.
      if (move.moving_piece->piece_color == PieceColor::WHITE)
      {
        white_has_castled = true;
      }
      else
      {
        black_has_castled = true;
      }

      // If castle move, move rook to new square.
      switch (king_move_distance)
      {
      case 2:
        // King Side Castle.
        std::swap(chess_board[XH_FILE][move.to_y],
                  chess_board[XF_FILE][move.to_y]);
        chess_board[XF_FILE][move.to_y]->piece_has_moved = true;
        chess_board[XF_FILE][move.to_y]->x_file = XF_FILE;
        break;
      case -2:
        // Queen Side Castle.
        std::swap(chess_board[XA_FILE][move.to_y],
                  chess_board[XD_FILE][move.to_y]);
        chess_board[XD_FILE][move.to_y]->piece_has_moved = true;
        chess_board[XD_FILE][move.to_y]->x_file = XD_FILE;
        break;
      default:
        break;
      }
    }
  }

  if (move.captured_piece != nullptr)
  {
    // Update position of captured piece. If piece is captured, set position to
    // -1, -1. This will mean the piece is captured and not on the board.
    move.captured_piece->x_file = -1;
    move.captured_piece->y_rank = -1;
  }

  if (move.captured_piece != nullptr && !move.capture_is_en_passant)
  {
    // If capturing, clear old square (point to empty_piece).
    chess_board[move.to_x][move.to_y] = &empty_piece;
  }

  // Move moving piece to new square.
  std::swap(chess_board[move.from_x][move.from_y],
            chess_board[move.to_x][move.to_y]);

  if (move.promotion_piece_type != PieceType::EMPTY)
  {
    // If pawn is promoting, update piece type to promotion piece type.
    chess_board[move.to_x][move.to_y]->piece_type = move.promotion_piece_type;
  }

  if (move.first_move_of_moving_piece)
  {
    move.moving_piece->piece_has_moved = true;
  }

  // Update position of moving piece.
  move.moving_piece->x_file = move.to_x;
  move.moving_piece->y_rank = move.to_y;

  // Update move color, it is now the other player's turn.
  color_to_move = (color_to_move == PieceColor::WHITE) ? PieceColor::BLACK
                                                       : PieceColor::WHITE;

  // Store move in previous moves stack for undoing moves.
  previous_move_stack.push(move);
  manage_piece_counts_on_apply(move);

  // Update hash for new board state.
  add_current_state_to_visited_states();
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
    // Update position of captured piece.
    move.captured_piece->x_file = move.to_x;
    move.captured_piece->y_rank = move.from_y;

    // Add captured pawn.
    chess_board[move.to_x][move.from_y] = move.captured_piece;
  }
  else if (move.moving_piece->piece_type == PieceType::KING)
  {
    // Keep track of the king's positions for evaluation.
    if (move.moving_piece->piece_color == PieceColor::WHITE)
    {
      white_king_x_file = move.from_x;
      white_king_y_rank = move.from_y;
    }
    else
    {
      black_king_x_file = move.from_x;
      black_king_y_rank = move.from_y;
    }

    int king_move_distance = move.to_x - move.from_x;
    if (king_move_distance == 2 || king_move_distance == -2)
    {
      // Keep track of castling for evaluation.
      if (move.moving_piece->piece_color == PieceColor::WHITE)
      {
        white_has_castled = false;
      }
      else
      {
        black_has_castled = false;
      }

      // If castle move, move rook back to original square.
      switch (king_move_distance)
      {
      case 2:
        // King Side Castle.
        std::swap(chess_board[XF_FILE][move.to_y],
                  chess_board[XH_FILE][move.to_y]);
        chess_board[XH_FILE][move.to_y]->piece_has_moved = false;
        chess_board[XH_FILE][move.to_y]->x_file = XH_FILE;
        break;
      case -2:
        // Queen Side Castle.
        std::swap(chess_board[XD_FILE][move.to_y],
                  chess_board[XA_FILE][move.to_y]);
        chess_board[XA_FILE][move.to_y]->piece_has_moved = false;
        chess_board[XA_FILE][move.to_y]->x_file = XA_FILE;
        break;
      default:
        break;
      }
    }
  }

  if (move.promotion_piece_type != PieceType::EMPTY)
  {
    // If pawn was promoted, change piece type back to pawn.
    chess_board[move.to_x][move.to_y]->piece_type = PieceType::PAWN;
  }

  // Move piece back to original square.
  std::swap(chess_board[move.from_x][move.from_y],
            chess_board[move.to_x][move.to_y]);

  if (move.captured_piece != nullptr && !move.capture_is_en_passant)
  {
    // Update position of captured piece.
    move.captured_piece->x_file = move.to_x;
    move.captured_piece->y_rank = move.to_y;

    // If a piece was captured, add the piece back.
    chess_board[move.to_x][move.to_y] = move.captured_piece;
  }

  if (move.first_move_of_moving_piece)
  {
    move.moving_piece->piece_has_moved = false;
  }

  // Update position of moving piece.
  move.moving_piece->x_file = move.from_x;
  move.moving_piece->y_rank = move.from_y;

  // Update move color, it is now the other player's turn.
  color_to_move = (color_to_move == PieceColor::WHITE) ? PieceColor::BLACK
                                                       : PieceColor::WHITE;

  // Remove move from moves stack, move is undone.
  previous_move_stack.pop();
  manage_piece_counts_on_undo(move);

  // Update hash for new board state.
  remove_current_state_from_visited_states();
}

void BoardState::apply_null_move()
{
  // Update move color, it is now the other player's turn.
  color_to_move = (color_to_move == PieceColor::WHITE) ? PieceColor::BLACK
                                                       : PieceColor::WHITE;
  // Need to update hash since color_to_move also affects hash of board state.
  add_current_state_to_visited_states();
}

void BoardState::undo_null_move()
{
  // Update move color, it is now the other player's turn.
  color_to_move = (color_to_move == PieceColor::WHITE) ? PieceColor::BLACK
                                                       : PieceColor::WHITE;
  // Need to update hash since color_to_move also affects hash of board state.
  remove_current_state_from_visited_states();
}

void BoardState::clear_chess_board()
{
  // Empty previous move stack.
  while (!previous_move_stack.empty())
  {
    undo_move();
  }

  // Clear visited states hash map and stack.
  visisted_states_hash_map.clear();
  while (!visisted_states_hash_stack.empty())
  {
    visisted_states_hash_stack.pop();
  }

  color_to_move = PieceColor::NONE;
  white_king_is_alive = false;
  black_king_is_alive = false;
  queens_on_board = 0;
  number_of_main_pieces_left = 0;
  white_king_y_rank = -1;
  white_king_x_file = -1;
  black_king_y_rank = -1;
  black_king_x_file = -1;
  white_has_castled = false;
  black_has_castled = false;
  is_end_game = false;

  for (int y_rank = Y_MIN; y_rank <= Y_MAX; ++y_rank)
  {
    for (int x_file = X_MIN; x_file <= X_MAX; ++x_file)
    {
      // Clear old piece to avoid memory leaks. All empty squares point to the
      // same empty piece.
      if (chess_board[x_file][y_rank] != &empty_piece)
      {
        delete chess_board[x_file][y_rank];
        chess_board[x_file][y_rank] = &empty_piece;
      }
    }
  }
}

auto BoardState::get_current_state_hash() -> uint64_t
{
  if (visisted_states_hash_stack.empty())
  {
    return 0;
  }
  return visisted_states_hash_stack.top();
}

auto BoardState::current_state_has_been_repeated_three_times() -> bool
{
  return visisted_states_hash_map[get_current_state_hash()] >= 3;
}

auto BoardState::current_state_has_been_visited() -> bool
{
  return visisted_states_hash_map[get_current_state_hash()] > 1;
}

void BoardState::add_current_state_to_visited_states()
{
  uint64_t current_state_hash = compute_zobrist_hash();
  visisted_states_hash_stack.push(current_state_hash);
  if (visisted_states_hash_map.find(current_state_hash) ==
      visisted_states_hash_map.end())
  {
    // If the state has never been visited, it will be added to the map with a
    // count of 1.
    visisted_states_hash_map[current_state_hash] = 1;
  }
  else
  {
    // If the state has been visited before, the count will be incremented by 1.
    visisted_states_hash_map[current_state_hash]++;
  }
}

void BoardState::remove_current_state_from_visited_states()
{
  uint64_t current_state_hash = get_current_state_hash();
  if (visisted_states_hash_map.find(current_state_hash) !=
      visisted_states_hash_map.end())
  {
    // If the state has been visited before, the count will be decremented by 1.
    visisted_states_hash_map[current_state_hash]--;
    if (visisted_states_hash_map[current_state_hash] == 0)
    {
      // If the count is 0, the state will be removed from the map.
      visisted_states_hash_map.erase(current_state_hash);
    }
  }
  visisted_states_hash_stack.pop();
}

void BoardState::is_end_game_check()
{
  if (queens_on_board == 2)
  {
    is_end_game = number_of_main_pieces_left <= END_GAME_CONDITION_TWO_QUEENS;
  }
  else if (queens_on_board == 1)
  {
    is_end_game = number_of_main_pieces_left <= END_GAME_CONDITION_ONE_QUEEN;
  }
  else
  {
    is_end_game = number_of_main_pieces_left <= END_GAME_CONDITION_NO_QUEENS;
  }
}

void BoardState::update_pieces_list()
{
  piece_list.clear();
  for (int x_file = X_MIN; x_file <= X_MAX; ++x_file)
  {
    for (int y_rank = Y_MIN; y_rank <= Y_MAX; ++y_rank)
    {
      Piece *piece = chess_board[x_file][y_rank];
      if (piece->piece_type != PieceType::EMPTY)
      {
        piece_list.push_back(piece);
      }
    }
  }
}

// PRIVATE FUNCTIONS

void BoardState::clear_pointers()
{
  for (int x_file = X_MIN; x_file <= Y_MAX; ++x_file)
  {
    for (int y_rank = Y_MIN; y_rank <= Y_MAX; ++y_rank)
    {
      if (chess_board[x_file][y_rank] != nullptr &&
          chess_board[x_file][y_rank] != &empty_piece)
      {
        delete chess_board[x_file][y_rank];
        chess_board[x_file][y_rank] = nullptr;
      }
    }
  }
}

void BoardState::initialize_zobrist_keys()
{
  std::mt19937_64 rng(0); // Use a fixed seed for reproducibility
  std::uniform_int_distribution<uint64_t> dist;

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

auto BoardState::compute_zobrist_hash() const -> uint64_t
{
  uint64_t hash = 0;

  for (int y_rank = Y_MIN; y_rank <= Y_MAX; ++y_rank)
  {
    for (int x_file = X_MIN; x_file <= X_MAX; ++x_file)
    {
      Piece *piece = chess_board[x_file][y_rank];
      if (piece->piece_type != PieceType::EMPTY)
      {
        int piece_index = static_cast<int>(piece->piece_type);
        int color_index = (piece->piece_color == PieceColor::WHITE) ? 0 : 1;
        hash ^= zobrist_keys[y_rank * BOARD_WIDTH + x_file][piece_index]
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

void BoardState::manage_piece_counts_on_apply(Move &move)
{
  if (move.captured_piece == nullptr)
  {
    return;
  }

  switch (move.captured_piece->piece_type)
  {
  case PieceType::EMPTY:
    break;
  case PieceType::PAWN:
    break;
  case PieceType::KING:
    if (move.captured_piece->piece_color == PieceColor::WHITE)
    {
      white_king_is_alive = false;
    }
    else
    {
      black_king_is_alive = false;
    }
    break;
  case PieceType::QUEEN:
    --queens_on_board;
    is_end_game_check();
    break;
  default:
    --number_of_main_pieces_left;
    is_end_game_check();
    break;
  }

  if (move.promotion_piece_type == PieceType::EMPTY)
  {
    return;
  }

  switch (move.promotion_piece_type)
  {
  case PieceType::QUEEN:
    ++queens_on_board;
    break;

  default:
    ++number_of_main_pieces_left;
    break;
  }
}

void BoardState::manage_piece_counts_on_undo(Move &move)
{
  if (move.captured_piece == nullptr)
  {
    return;
  }

  switch (move.captured_piece->piece_type)
  {
  case PieceType::EMPTY:
    break;
  case PieceType::PAWN:
    break;
  case PieceType::KING:
    if (move.captured_piece->piece_color == PieceColor::WHITE)
    {
      white_king_is_alive = true;
    }
    else
    {
      black_king_is_alive = true;
    }
    break;
  case PieceType::QUEEN:
    ++queens_on_board;
    is_end_game_check();
    break;
  default:
    ++number_of_main_pieces_left;
    is_end_game_check();
    break;
  }

  if (move.promotion_piece_type == PieceType::EMPTY)
  {
    return;
  }

  switch (move.promotion_piece_type)
  {
  case PieceType::QUEEN:
    --queens_on_board;
    break;

  default:
    --number_of_main_pieces_left;
    break;
  }
}
} // namespace engine::parts
