#ifndef BOARD_STATE_H
#define BOARD_STATE_H

#include "move.h"
#include "piece.h"

#include <array>
#include <cstdio>
#include <random>
#include <stack>
#include <unordered_map>
#include <vector>

class BoardState {
private:
  // All empty squares point to the same instance.
  Piece empty_piece = Piece();

  // Char to represent white pieces.
  const std::unordered_map<PieceType, char> w_piece_to_char = {
      {PieceType::EMPTY, '-'},  {PieceType::KING, 'K'},
      {PieceType::QUEEN, 'Q'},  {PieceType::ROOK, 'R'},
      {PieceType::BISHOP, 'B'}, {PieceType::KNIGHT, 'N'},
      {PieceType::PAWN, 'P'}};

  // Char to represent black pieces.
  const std::unordered_map<PieceType, char> b_piece_to_char = {
      {PieceType::EMPTY, '-'},  {PieceType::KING, 'k'},
      {PieceType::QUEEN, 'q'},  {PieceType::ROOK, 'r'},
      {PieceType::BISHOP, 'b'}, {PieceType::KNIGHT, 'n'},
      {PieceType::PAWN, 'p'}};

  // Zobrist keys
  std::array<std::array<std::array<size_t, 2>, 6>, 64> zobrist_keys;
  size_t zobrist_side_to_move;

  /**
   * @brief Initialises the zobrist keys.
   */
  void initialize_zobrist_keys();

public:
  // 8 x 8 array to represent a chess board.
  std::array<std::array<Piece *, 8>, 8> chess_board;

  // Represents who is to move.
  PieceColor move_color = PieceColor::WHITE;

  // List of all moves that have been applied to the current board.
  std::stack<Move> previous_moves;

  /**
   * @brief Default Constructor - sets chess_board using reset_board.
   * @param move_color Color to move with current state. White by default starts
   * the game.
   * @param engine_color Determines which color to maximise for.
   */
  BoardState(PieceColor move_color = PieceColor::WHITE,
             PieceColor engine_color = PieceColor::BLACK);

  /**
   * @brief Use if boardstate is not default chess starting state.
   * @param input_chess_board Initialises chess_board to equal
   * input_chess_board.
   * @param move_color Color to move with current state. White by default starts
   * the game.
   * @param engine_color Determines which color to maximise for.
   */
  BoardState(std::array<std::array<Piece *, 8>, 8> &input_chess_board,
             PieceColor move_color = PieceColor::WHITE,
             PieceColor engine_color = PieceColor::BLACK);

  /**
   * @brief Resets chess board to default starting piece positions.
   */
  void reset_board();

  /**
   * @brief Prints the board.
   */
  void print_board();

  /**
   * @brief Will apply the given move unto board_state;
   * @param move Move to apply on board_state.
   */
  void apply_move(Move &move);

  /**
   * @brief Will undo the given move unto board_state;
   */
  void undo_move();

  /**
   * @brief Checks if the given square is attacked.
   * @param x, y The coordinate of the square.
   * @param color The color of the attacked pieces.
   * @return True if the square is attacked, false otherwise.
   */
  bool square_is_attacked(int x, int y, PieceColor color);

  /**
   * @brief Checks if the king of the given color is checked.
   */
  bool king_is_checked(PieceColor color);

  /**
   * @brief Computes the Zobrist hash for the current board state.
   * @return The Zobrist hash value.
   */
  size_t compute_zobrist_hash() const;
};

#endif
