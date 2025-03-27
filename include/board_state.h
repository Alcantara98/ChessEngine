#ifndef BOARD_STATE_H
#define BOARD_STATE_H

#include "engine_constants.h"
#include "move.h"
#include "piece.h"

#include <algorithm>
#include <array>
#include <cctype>
#include <cstdio>
#include <random>
#include <regex>
#include <stack>
#include <unordered_map>
#include <vector>

namespace engine::parts
{
/// @brief 8 x 8 array type to represent a chess board.
using chess_board_type =
    std::array<std::array<Piece *, BOARD_HEIGHT>, BOARD_WIDTH>;

/**
 * @brief Class to represent the current state of the chess board.
 *
 * @note Ensure that new properties are reset accordingly in reset_board and
 * clear_chess_board functions.
 */
class BoardState
{
public:
  // PROPERTIES

  /// @brief 8 x 8 array to represent a chess board.
  chess_board_type chess_board;

  /// @brief Stack to keep track of previous moves.
  std::stack<Move> previous_move_stack;

  /// @brief Represents which color is to move.
  PieceColor color_to_move = PieceColor::WHITE;

  // EVALUATION HELPER PROPERTIES

  /// @brief Flag to check if the white king is on the board.
  bool white_king_is_alive = true;

  /// @brief Flag to check if the black king is on the board.
  bool black_king_is_alive = true;

  /// @brief Number of queens on the board.
  int queens_on_board = START_QUEENS_COUNT;

  /// @brief Number of main pieces left on the board (Rooks, Bishops, Knights).
  int number_of_main_pieces_left = START_MAIN_PIECES_COUNT;

  /// @brief White King's file position.
  int white_king_x_position = XE_FILE;

  /// @brief White King's rank position.
  int white_king_y_position = Y1_RANK;

  /// @brief Black King's file position.
  int black_king_x_position = XE_FILE;

  /// @brief Black King's rank position.
  int black_king_y_position = Y8_RANK;

  /// @brief Flag to check if the white king has castled.
  bool white_has_castled = false;

  /// @brief Flag to check if the black king has castled.
  bool black_has_castled = false;

  /// @brief Game state.
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
  void setup_default_board();

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
  auto square_is_attacked(int x_position,
                          int y_position,
                          PieceColor color_being_attacked) -> bool;

  /**
   * @brief Checks if the king of the given color is in check.
   *
   * @param color_of_king The color of the king to check.
   *
   * @return True if the king is in check, false otherwise.
   */
  auto king_is_checked(PieceColor color_of_king) -> bool;

  /**
   * @brief Checks if the given move leaves the king in check.
   *
   * @param move The move to check.
   *
   * @return True if the move leaves the king in check, false otherwise.
   */
  auto move_leaves_king_in_check(Move &move) -> bool;

  /**
   * @brief Clears all pieces from the chess board.
   *
   * @details Also resets properties to null values since there are no pieces on
   * the board.
   */
  void clear_chess_board();

  /**
   * @brief Gets the board state hash from visisted_states_hash_stack. The
   * current board state hash will be calculated after a move is applied in
   * apply_move and stored in the visisted_states_hash_stack and
   * visisted_states_hash_map.
   */
  auto get_current_state_hash() -> uint64_t;

  /**
   * @brief Checks if the current state has been repeated three times. This is
   * used to detect three fold repetition. Game is drawn if the same state is
   * repeated three times.
   *
   * @return True if the current state has been repeated three times, false
   * otherwise.
   */
  auto current_state_has_been_repeated_three_times() -> bool;

  /**
   * @brief Checks if the current state has been visited.
   */
  auto current_state_has_been_visited() -> bool;

  /**
   * @brief Add hash to visited states map and stack.
   *
   * @note If the state has never been visited, it will be added to the map with
   * a count of 1. If the state has been visited before, the count will be
   * incremented by 1.
   */
  void add_current_state_to_visited_states();

  /**
   * @brief Remove hash from visited states map and stack.
   *
   * @note If the state has been visited before, the count will be decremented
   * by 1. If the count is 0, the state will be removed from the map.
   */
  void remove_current_state_from_visited_states();

  /**
   * @brief Checks if the game is in an end game state and updates the
   * is_end_game property.
   */
  void is_end_game_check();

private:
  // PROPERTIES

  /// @brief Zobrist keys.
  std::array<
      std::array<std::array<uint64_t, NUM_OF_COLORS>, NUM_OF_PIECE_TYPES>,
      NUM_OF_SQUARES>
      zobrist_keys;

  /// @brief Zobrist key for the side to move.
  uint64_t zobrist_side_to_move;

  /// @brief All empty squares point to the same Piece instance.
  Piece empty_piece;

  /// @brief Map to keep track of visited states. This is used to detect three
  /// fold repetition. Game is drawn if the same state is repeated three times.
  std::map<uint64_t, int> visisted_states_hash_map;

  /// @brief Stack to keep track of the sequence of states that have been
  /// visited (analogous to previous_move_stack, but for the hash of the state).
  /// This is allows us to know which state in visisted_states_hash_map to
  /// decrement or remove when undoing a move.
  /// @note Top of the stack will always be the hash of the current state.
  std::stack<uint64_t> visisted_states_hash_stack;

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
   * @brief Computes the Zobrist hash for the current board state.
   *
   * @return The Zobrist hash value.
   */
  [[nodiscard]] auto compute_zobrist_hash() const -> uint64_t;

  /**
   * @brief Helper function to check if a square is attacked by a pawn.
   *
   * @param x_position The x coordinate of the pawn.
   * @param y_position The y coordinate of the pawn.
   * @param color_being_attacked The color of the pieces being attacked.
   *
   * @return True if the square is attacked, false otherwise.
   */
  auto square_is_attacked_by_pawn(int &x_position,
                                  int &y_position,
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
  auto square_is_attacked_by_knight(int &x_position,
                                    int &y_position,
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
  square_is_attacked_by_rook_or_queen(int &x_position,
                                      int &y_position,
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
  auto square_is_attacked_by_bishop_or_queen(int &x_position,
                                             int &y_position,
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
  auto square_is_attacked_by_king(int &x_position,
                                  int &y_position,
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
};
} // namespace engine::parts

#endif
