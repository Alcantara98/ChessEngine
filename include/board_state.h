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
using chess_board_type =
    std::array<std::array<Piece *, BOARD_HEIGHT>, BOARD_WIDTH>;

/**
 * @brief Class to represent the current state of the chess board.
 */
class BoardState
{
public:
  // 8 x 8 array to represent a chess board.
  chess_board_type chess_board;

  // Stack to keep track of previous moves.
  std::stack<Move> previous_move_stack;

  // Represents which color is to move.
  PieceColor color_to_move = PieceColor::WHITE;

  /**
   * @brief Default Constructor - sets chess_board using reset_board.
   *
   * @param color_to_move Color to move with current state. White by default
   * starts the game.
   * @param engine_color Determines which color to maximise for.
   */
  BoardState(PieceColor color_to_move = PieceColor::WHITE);

  /**
   * @brief Use if boardstate is not default chess starting state.
   *
   * @param input_chess_board Initialises chess_board to equal
   * input_chess_board.
   * @param color_to_move Color to move with current state. White by default
   * starts the game.
   * @param engine_color Determines which color to maximise for.
   */
  BoardState(chess_board_type &input_chess_board,
             PieceColor color_to_move = PieceColor::WHITE);

  // Deep copy constructor
  BoardState(const BoardState &other);

  // Destructor
  ~BoardState();

  /**
   * @brief Resets chess board to default starting piece positions.
   */
  void reset_board();

  /**
   * @brief Prints the board.
   */
  void print_board(PieceColor color);

  /**
   * @brief Will apply the given move unto board_state;
   *
   * @param move Move to apply on board_state.
   */
  void apply_move(Move &move);

  /**
   * @brief Will undo the given move unto board_state;
   */
  void undo_move();

  /**
   * @brief Will apply a null move unto board_state;
   */
  void apply_null_move();

  /**
   * @brief Will undo a null move unto board_state;
   */
  void undo_null_move();

  /**
   * @brief Checks if the given square is attacked.
   *
   * @param x_position, y_position The coordinate of the square.
   * @param color_being_attacked The color of the attacked pieces.
   *
   * @return True if the square is attacked, false otherwise.
   */
  auto square_is_attacked(int x_position, int y_position,
                          PieceColor color_being_attacked) -> bool;

  /**
   * @brief Checks if the king of the given color is checked.
   *
   * @param color_of_king The color of the king to check.
   *
   * @return True if the king is checked, false otherwise.
   */
  auto king_is_checked(PieceColor &color_of_king) -> bool;

  /**
   * @brief Computes the Zobrist hash for the current board state.
   *
   * @return The Zobrist hash value.
   */
  [[nodiscard]] auto compute_zobrist_hash() const -> size_t;

  /**
   * @brief Checks if the given move leaves the king in check.
   * @param move The move to check.
   *
   * @return True if the move leaves the king in check, false otherwise.
   */
  auto move_leaves_king_in_check(Move &move) -> bool;

private:
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

  /**
   * @brief Initialises the zobrist keys.
   */
  void initialize_zobrist_keys();

  /**
   * @brief Helper function to check if a square is attacked by a pawn.
   *
   * @param x_position, y_position The coordinate of the pawn.
   * @param color_being_attacked The color of the attacked pieces.
   *
   * @return True if the square is attacked, false otherwise.
   */
  auto square_is_attacked_by_pawn(int &x_position, int &y_position,
                                  PieceColor &color_being_attacked) -> bool;

  /**
   * @brief Helper function to check if a square is attacked by a knight.
   *
   * @param x_position, y_position The coordinate of the knight.
   * @param color_being_attacked The color of the attacked pieces.
   *
   * @return True if the square is attacked, false otherwise.
   */
  auto square_is_attacked_by_knight(int &x_position, int &y_position,
                                    PieceColor &color_being_attacked) -> bool;

  /**
   * @brief Helper function to check if a square is attacked by a rook or queen.
   *
   * @param x_position, y_position The coordinate of the rook or queen.
   * @param color_being_attacked The color of the attacked pieces.
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
   * @param x_position, y_position The coordinate of the bishop or queen.
   * @param color_being_attacked The color of the attacked pieces.
   *
   * @return True if the square is attacked, false otherwise.
   */
  auto square_is_attacked_by_bishop_or_queen(int &x_position, int &y_position,
                                             PieceColor &color_being_attacked)
      -> bool;

  /**
   * @brief Helper function to check if a square is attacked by a king.
   *
   * @param x_position, y_position The coordinate of the king.
   * @param color_being_attacked The color of the attacked pieces.
   *
   * @return True if the square is attacked, false otherwise.
   */
  auto square_is_attacked_by_king(int &x_position, int &y_position,
                                  PieceColor &color_being_attacked) -> bool;
};
} // namespace engine::parts

#endif
