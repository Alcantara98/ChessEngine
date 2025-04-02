#include "fen_interface.h"

namespace engine::parts::fen_interface
{
auto setup_custom_board(BoardState &board_state,
                        const std::string &fen_configuration) -> bool
{
  board_state.queens_on_board = 0;
  board_state.number_of_main_pieces_left = 0;

  std::string board_configuration;
  std::string color_to_move_local;
  std::string castling_rights;
  std::string en_passant_square;

  std::smatch matches;
  std::regex board_config_pattern(
      R"(^((?:[rnbqkpRNBQKP1-8]{1,8}\/){7}[rnbqkpRNBQKP1-8]{1,8}) ([wb]) (K?Q?k?q?|-) (-|[a-h][36]) \d+ \d+$)");

  if (std::regex_match(fen_configuration, matches, board_config_pattern))
  {
    // Extract the parts using regex captures
    board_configuration = matches[1].str();
    color_to_move_local = matches[2].str(); // Capture color to move
    castling_rights = matches[3].str();
    en_passant_square = matches[4].str();
  }
  else
  {
    return false;
  }

  if (!initialize_board(board_state, board_configuration))
  {
    return false;
  }
  if (!validate_castling_rights(board_state, castling_rights))
  {
    return false;
  }
  if (!validate_en_passant_target(board_state, en_passant_square))
  {
    return false;
  }

  board_state.color_to_move =
      (color_to_move_local[0] == parts::WHITE_PIECE_CHAR) ? PieceColor::WHITE
                                                          : PieceColor::BLACK;

  // Add the initial board state to the visited states hash map.
  board_state.add_current_state_to_visited_states();

  return true;
}

auto initialize_board(BoardState &board_state,
                      std::string board_configuration) -> bool
{
  // Set all squares to empty and set properties to null values.
  board_state.clear_chess_board();

  int board_configuration_index = 0;
  for (int y_rank = Y_MAX; y_rank >= Y_MIN; --y_rank)
  {
    for (int x_file = X_MIN; x_file <= X_MAX; ++x_file)
    {
      // '/' is used to separate ranks. We just ignore it.
      char piece_char = board_configuration[board_configuration_index];
      if (piece_char == '/')
      {
        ++board_configuration_index;
        piece_char = board_configuration[board_configuration_index];
      }

      // Check if the character is a digit 1-8 which indicates empty squares.
      // For example, if 3 is found, the next 3 squares including the current
      // square are empty.
      if (piece_char >= '1' && piece_char <= '8')
      {
        // Convert char to int
        int empty_squares = piece_char - '0';
        if (x_file + empty_squares - 1 > X_MAX)
        {
          return false;
        }
        x_file += empty_squares - 1;
        ++board_configuration_index;
        continue;
      }

      PieceColor piece_color =
          (islower(piece_char) != 0) ? PieceColor::BLACK : PieceColor::WHITE;
      PieceType piece_type = CHAR_TO_PIECE_TYPE.at(std::tolower(piece_char));

      create_pieces(board_state, piece_color, piece_type, x_file, y_rank);

      ++board_configuration_index;
    }
  }
  board_state.is_end_game_check();

  return true;
}

void create_pieces(BoardState &board_state,
                   const PieceColor &piece_color,
                   const PieceType &piece_type,
                   const int &x_file,
                   const int &y_rank)
{
  switch (piece_type)
  {
  case PieceType::QUEEN:
    ++board_state.queens_on_board;
    break;

  case PieceType::ROOK:
    ++board_state.number_of_main_pieces_left;
    break;

  case PieceType::BISHOP:
    ++board_state.number_of_main_pieces_left;
    break;

  case PieceType::KNIGHT:
    ++board_state.number_of_main_pieces_left;
    break;

  case PieceType::KING:
    if (piece_color == PieceColor::WHITE)
    {
      board_state.white_king_is_alive = true;
      board_state.white_king_x_file = x_file;
      board_state.white_king_y_rank = y_rank;
    }
    else
    {
      board_state.black_king_is_alive = true;
      board_state.black_king_x_file = x_file;
      board_state.black_king_y_rank = y_rank;
    }
    break;

  default:
    break;
  }

  if (piece_type == PieceType::PAWN)
  {
    bool has_moved = true;
    // If the pawn is at the starting position, it has not moved since it cannot
    // move backwards.
    if ((piece_color == PieceColor::WHITE && y_rank == Y2_RANK) ||
        (piece_color == PieceColor::BLACK && y_rank == Y7_RANK))
    {
      has_moved = false;
    }
    board_state.chess_board[x_file][y_rank] =
        new Piece(piece_type, piece_color, has_moved);
  }
  else
  {
    board_state.chess_board[x_file][y_rank] =
        new Piece(piece_type, piece_color, true);
  }
}

auto validate_castling_rights(BoardState &board_state,
                              const std::string &castling_rights) -> bool
{
  for (char castling_right : castling_rights)
  {
    switch (castling_right)
    {
    case 'K': // White King Side Castle
      if (!validate_white_king_side_castle(board_state))
      {
        return false;
      }
      break;

    case 'Q': // White Queen Side Castle
      if (!validate_white_queen_side_castle(board_state))
      {
        return false;
      }
      break;

    case 'k': // Black King Side Castle
      if (!validate_black_king_side_castle(board_state))
      {
        return false;
      }
      break;

    case 'q': // Black Queen Side Castle
      if (!validate_black_queen_side_castle(board_state))
      {
        return false;
      }
      break;

    default:
      break;
    }
  }

  return true;
}

auto validate_white_king_side_castle(BoardState &board_state) -> bool
{
  // Check if the white king is on the correct square.
  if (board_state.white_king_x_file != XE_FILE ||
      board_state.white_king_y_rank != Y1_RANK)
  {
    return false;
  }
  // Check if the white rook is on the correct square.
  if (board_state.chess_board[XH_FILE][Y1_RANK]->piece_type !=
          PieceType::ROOK ||
      board_state.chess_board[XH_FILE][Y1_RANK]->piece_color !=
          PieceColor::WHITE)
  {
    return false;
  }
  // Set the rook and king to not have moved.
  board_state.chess_board[XH_FILE][Y1_RANK]->piece_has_moved = false;
  board_state.chess_board[XE_FILE][Y1_RANK]->piece_has_moved = false;

  return true;
}

auto validate_white_queen_side_castle(BoardState &board_state) -> bool
{
  // Check if the white king is on the correct square.
  if (board_state.white_king_x_file != XE_FILE ||
      board_state.white_king_y_rank != Y1_RANK)
  {
    return false;
  }
  // Check if the white rook is on the correct square.
  if (board_state.chess_board[XA_FILE][Y1_RANK]->piece_type !=
          PieceType::ROOK ||
      board_state.chess_board[XA_FILE][Y1_RANK]->piece_color !=
          PieceColor::WHITE)
  {
    return false;
  }
  // Set the rook and king to not have moved.
  board_state.chess_board[XA_FILE][Y1_RANK]->piece_has_moved = false;
  board_state.chess_board[XE_FILE][Y1_RANK]->piece_has_moved = false;

  return true;
}

auto validate_black_king_side_castle(BoardState &board_state) -> bool
{
  // Check if the black king is on the correct square.
  if (board_state.black_king_x_file != XE_FILE ||
      board_state.black_king_y_rank != Y8_RANK)
  {
    return false;
  }
  // Check if the black rook is on the correct square.
  if (board_state.chess_board[XH_FILE][Y8_RANK]->piece_type !=
          PieceType::ROOK ||
      board_state.chess_board[XH_FILE][Y8_RANK]->piece_color !=
          PieceColor::BLACK)
  {
    return false;
  }
  // Set the rook and king to not have moved.
  board_state.chess_board[XH_FILE][Y8_RANK]->piece_has_moved = false;
  board_state.chess_board[XE_FILE][Y8_RANK]->piece_has_moved = false;

  return true;
}

auto validate_black_queen_side_castle(BoardState &board_state) -> bool
{
  // Check if the black king is on the correct square.
  if (board_state.black_king_x_file != XE_FILE ||
      board_state.black_king_y_rank != Y8_RANK)
  {
    return false;
  }
  // Check if the black rook is on the correct square.
  if (board_state.chess_board[XA_FILE][Y8_RANK]->piece_type !=
          PieceType::ROOK ||
      board_state.chess_board[XA_FILE][Y8_RANK]->piece_color !=
          PieceColor::BLACK)
  {
    return false;
  }
  // Set the rook and king to not have moved.
  board_state.chess_board[XA_FILE][Y8_RANK]->piece_has_moved = false;
  board_state.chess_board[XE_FILE][Y8_RANK]->piece_has_moved = false;

  return true;
}

auto validate_en_passant_target(BoardState &board_state,
                                const std::string &en_passant_target) -> bool
{
  if (en_passant_target == "-")
  {
    return true;
  }

  char en_passant_file = en_passant_target[0];
  int en_passant_rank = en_passant_target[1] - '0' - 1;

  // Check if the file and rank are within the bounds of the board.
  if (en_passant_file < 'a' || en_passant_file > 'h')
  {
    return false;
  }
  if (en_passant_rank != Y3_RANK && en_passant_rank != Y6_RANK)
  {
    return false;
  }

  // Get position of the pawn that can be captured en passant.
  int pawn_x_file = en_passant_file - 'a';
  int pawn_y_rank = (en_passant_rank == Y3_RANK) ? Y4_RANK : Y5_RANK;

  Piece *pawn_piece = board_state.chess_board[pawn_x_file][pawn_y_rank];

  // Check if the piece is a pawn.
  if (pawn_piece == nullptr || pawn_piece->piece_type != PieceType::PAWN)
  {
    return false;
  }

  // Determine the original rank of the pawn.
  int original_rank =
      (pawn_piece->piece_color == PieceColor::WHITE) ? Y2_RANK : Y7_RANK;

  Move previous_pawn_move =
      Move(pawn_x_file, original_rank, pawn_x_file, pawn_y_rank, pawn_piece,
           true, true, pawn_x_file, pawn_y_rank);

  board_state.previous_move_stack.push(previous_pawn_move);

  return true;
}
} // namespace engine::parts::fen_interface