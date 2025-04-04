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
  int queens_on_board = INITIAL_QUEENS_COUNT;

  /// @brief Number of main pieces left on the board (Rooks, Bishops, Knights).
  int number_of_main_pieces_left = INITIAL_MAIN_PIECES_COUNT;

  /// @brief White King's file position.
  int white_king_x_file = XE_FILE;

  /// @brief White King's rank position.
  int white_king_y_rank = Y1_RANK;

  /// @brief Black King's file position.
  int black_king_x_file = XE_FILE;

  /// @brief Black King's rank position.
  int black_king_y_rank = Y8_RANK;

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
   *
   * @details This function sets up the chess board with the default starting
   * positions for all pieces and resets all related properties.
   */
  void setup_default_board();

  /**
   * @brief Resets chess board to default starting state.
   *
   * @details This function clears the board and reinitializes it to the default
   * state, including resetting all properties to their initial values.
   */
  void reset_board();

  /**
   * @brief Prints the board.
   *
   * @param color The color at the bottom of the board (WHITE or BLACK).
   */
  void print_board(PieceColor color);

  /**
   * @brief Applies the given move to the board state.
   *
   * @param move Move to apply on the board state.
   *
   * @details Updates the board state after applying the move. Manages piece
   * counts, and recalculates the Zobrist hash for the new state.
   */
  void apply_move(Move &move);

  /**
   * @brief Undoes the last move applied to the board state.
   *
   * @details Reverts the board state to the previous state before the last move
   * was applied. Manages piece counts, and recalculates
   * the Zobrist hash for the new state.
   */
  void undo_move();

  /**
   * @brief Applies a null move to the board state.
   *
   * @details A null move is where a player passes their turn without moving a
   * piece. Used in the Null Move Heuristic.
   */
  void apply_null_move();

  /**
   * @brief Undoes a null move applied to the board state.
   */
  void undo_null_move();

  /**
   * @brief Clears all pieces from the chess board.
   *
   * @details Also resets properties to null values since there are no pieces
   * on the board.
   */
  void clear_chess_board();

  /**
   * @brief Gets the board state hash from visited_states_hash_stack.
   *
   * @details The current board state hash is calculated after a move is applied
   * in apply_move and stored in visited_states_hash_stack and
   * visited_states_hash_map.
   *
   * @return The Zobrist hash of the current board state.
   */
  auto get_current_state_hash() -> uint64_t;

  /**
   * @brief Checks if the current state has been repeated three times.
   *
   * @details This is used to detect threefold repetition. The game is drawn if
   * the same state is repeated three times.
   *
   * @return True if the current state has been repeated three times, false
   * otherwise.
   */
  auto current_state_has_been_repeated_three_times() -> bool;

  /**
   * @brief Checks if the current state has been visited.
   *
   * @return True if the current state has been visited, false otherwise.
   */
  auto current_state_has_been_visited() -> bool;

  /**
   * @brief Adds the current state hash to the visited states map and stack.
   *
   * @details If the state has never been visited, it will be added to the map
   * with a count of 1. If the state has been visited before, the count will be
   * incremented by 1.
   */
  void add_current_state_to_visited_states();

  /**
   * @brief Removes the current state hash from the visited states map and
   * stack.
   *
   * @details If the state has been visited before, the count will be
   * decremented by 1. If the count is 0, the state will be removed from the
   * map.
   */
  void remove_current_state_from_visited_states();

  /**
   * @brief Checks if the game is in an end game state and updates the
   * is_end_game property.
   *
   * @details The end game state is determined based on the number of pieces
   * left on the board and other conditions.
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
