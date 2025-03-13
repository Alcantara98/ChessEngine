#ifndef BOARD_STATE_H
#define BOARD_STATE_H

#include "engine_constants.h"
#include "move.h"
#include "piece.h"

#include <algorithm>
#include <array>
#include <cstdio>
#include <random>
#include <stack>
#include <unordered_map>
#include <vector>

namespace engine::parts
{
// 8 x 8 array type to represent a chess board.
using chess_board_type =
    std::array<std::array<Piece *, BOARD_HEIGHT>, BOARD_WIDTH>;

/**
 * @brief Class to represent the current state of the chess board.
 */
class BoardState
{
public:
  // PROPERTIES

  // 8 x 8 array to represent a chess board.
  chess_board_type chess_board;

  // Stack to keep track of previous moves.
  std::stack<Move> previous_move_stack;

  // Represents which color is to move.
  PieceColor color_to_move = PieceColor::WHITE;

  // Piece counts on the board.
  bool white_king_on_board = true;
  bool black_king_on_board = true;
  int queens_on_board = START_QUEENS_COUNT;
  int number_of_main_pieces_left = START_MAIN_PIECES_COUNT;

  // Game state.
  bool is_end_game = false;

  // CONSTRUCTORS

  /**
   * @brief Default Constructor - sets chess_board using setup_board.
   *
   * @param color_to_move Color to move with current state. White by default
   * starts the game.
   */
  BoardState(PieceColor color_to_move = PieceColor::WHITE);

  /**
   * @brief Constructor to initialize board state with a given chess board.
   *
   * @param input_chess_board Initializes chess_board to equal
   * input_chess_board.
   * @param color_to_move Color to move with current state. White by default
   * starts the game.
   */
  BoardState(chess_board_type &input_chess_board,
             PieceColor color_to_move = PieceColor::WHITE);

  /**
   * @brief Copy constructor to copy another board state.
   *
   * @param other The board state to copy.
   */
  BoardState(const BoardState &other);

  /**
   * @brief Destructor - clears all pointers in the chess board.
   */
  ~BoardState();

  // FUNCTIONS

  /**
   * @brief Resets chess board to default starting piece positions.
   */
  void setup_board();

  /**
   * @brief Resets chess board to default starting piece positions.
   */
  void reset_board();

  /**
   * @brief Prints the board.
   *
   * @param color The color at the bottom of the board.
   */
  void print_board(PieceColor color);

  /**
   * @brief Applies the given move to the board state.
   *
   * @param move Move to apply on board_state.
   */
  void apply_move(Move &move);

  /**
   * @brief Undoes the last move applied to the board state.
   */
  void undo_move();

  /**
   * @brief Applies a null move to the board state.
   */
  void apply_null_move();

  /**
   * @brief Undoes a null move applied to the board state.
   */
  void undo_null_move();

  /**
   * @brief Checks if the given square is attacked.
   *
   * @param x_position The x coordinate of the square.
   * @param y_position The y coordinate of the square.
   * @param color_being_attacked The color of the pieces being attacked.
   *
   * @return True if the square is attacked, false otherwise.
   */
  auto square_is_attacked(int x_position, int y_position,
                          PieceColor color_being_attacked) -> bool;

  /**
   * @brief Checks if the king of the given color is in check.
   *
   * @param color_of_king The color of the king to check.
   *
   * @return True if the king is in check, false otherwise.
   */
  auto king_is_checked(PieceColor &color_of_king) -> bool;

  /**
   * @brief Checks if the given move leaves the king in check.
   *
   * @param move The move to check.
   *
   * @return True if the move leaves the king in check, false otherwise.
   */
  auto move_leaves_king_in_check(Move &move) -> bool;

  /**
   * @brief Computes the Zobrist hash for the current board state.
   *
   * @return The Zobrist hash value.
   */
  [[nodiscard]] auto compute_zobrist_hash() const -> size_t;

private:
  // PROPERTIES

  // PieceType to Char mapping for white pieces.
  const std::unordered_map<PieceType, char> white_piece_to_char_map = {
      {PieceType::EMPTY, '-'},  {PieceType::KING, 'K'},
      {PieceType::QUEEN, 'Q'},  {PieceType::ROOK, 'R'},
      {PieceType::BISHOP, 'B'}, {PieceType::KNIGHT, 'N'},
      {PieceType::PAWN, 'P'}};

  // PieceType to Char mapping for black pieces.
  const std::unordered_map<PieceType, char> black_piece_to_char_map = {
      {PieceType::EMPTY, '-'},  {PieceType::KING, 'k'},
      {PieceType::QUEEN, 'q'},  {PieceType::ROOK, 'r'},
      {PieceType::BISHOP, 'b'}, {PieceType::KNIGHT, 'n'},
      {PieceType::PAWN, 'p'}};

  // Zobrist keys
  std::array<std::array<std::array<size_t, NUM_OF_COLORS>, NUM_OF_PIECE_TYPES>,
             NUM_OF_SQUARES>
      zobrist_keys;

  // Zobrist key for the side to move.
  size_t zobrist_side_to_move;

  // All empty squares point to the same Piece instance.
  Piece empty_piece;

  // FUNCTIONS

  /**
   * @brief Clears all pointers in the chess board.
   */
  void clear_pointers();

  /**
   * @brief Initializes the Zobrist keys.
   */
  void initialize_zobrist_keys();

  /**
   * @brief Helper function to check if a square is attacked by a pawn.
   *
   * @param x_position The x coordinate of the pawn.
   * @param y_position The y coordinate of the pawn.
   * @param color_being_attacked The color of the pieces being attacked.
   *
   * @return True if the square is attacked, false otherwise.
   */
  auto square_is_attacked_by_pawn(int &x_position, int &y_position,
                                  PieceColor &color_being_attacked) -> bool;

  /**
   * @brief Helper function to check if a square is attacked by a knight.
   *
   * @param x_position The x coordinate of the knight.
   * @param y_position The y coordinate of the knight.
   * @param color_being_attacked The color of the pieces being attacked.
   *
   * @return True if the square is attacked, false otherwise.
   */
  auto square_is_attacked_by_knight(int &x_position, int &y_position,
                                    PieceColor &color_being_attacked) -> bool;

  /**
   * @brief Helper function to check if a square is attacked by a rook or queen.
   *
   * @param x_position The x coordinate of the rook or queen.
   * @param y_position The y coordinate of the rook or queen.
   * @param color_being_attacked The color of the pieces being attacked.
   *
   * @return True if the square is attacked, false otherwise.
   */
  auto
  square_is_attacked_by_rook_or_queen(int &x_position, int &y_position,
                                      PieceColor &color_being_attacked) -> bool;

  /**
   * @brief Helper function to check if a square is attacked by a bishop or
   * queen.
   *
   * @param x_position The x coordinate of the bishop or queen.
   * @param y_position The y coordinate of the bishop or queen.
   * @param color_being_attacked The color of the pieces being attacked.
   *
   * @return True if the square is attacked, false otherwise.
   */
  auto square_is_attacked_by_bishop_or_queen(int &x_position, int &y_position,
                                             PieceColor &color_being_attacked)
      -> bool;

  /**
   * @brief Helper function to check if a square is attacked by a king.
   *
   * @param x_position The x coordinate of the king.
   * @param y_position The y coordinate of the king.
   * @param color_being_attacked The color of the pieces being attacked.
   *
   * @return True if the square is attacked, false otherwise.
   */
  auto square_is_attacked_by_king(int &x_position, int &y_position,
                                  PieceColor &color_being_attacked) -> bool;

  /**
   * @brief Manages the piece counts after a move.
   *
   * @param move Manage piece counts for this move.
   */
  void manage_piece_counts_on_apply(Move &move);

  /**
   * @brief Manages the piece counts after undoing a move.
   *
   * @param move Manage piece counts for this move.
   */
  void manage_piece_counts_on_undo(Move &move);

  /**
   * @brief Checks if the game is in an end game state and updates the
   * is_end_game property.
   */
  void is_end_game_check();
};
} // namespace engine::parts

#endif
