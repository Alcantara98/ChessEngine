#ifndef MOVE_GENERATOR_H
#define MOVE_GENERATOR_H


#include "piece.h"
#include "move_generator.h"

#include <vector>
#include <array>

struct Move {
  Piece moving_piece;
  Piece captured_piece;
  PieceType promotion_piece_type;
  int from_x, from_y;
  int to_x, to_y;
  bool is_en_passant;
  bool first_move;
  bool pawn_moved_two;

  /**
   * @brief Constructor for normal move without captures.
   *
   * @param from_x Initial position of piece in x axis.
   * @param from_y Initial position of piece in y axis.
   * @param to_x Final position of piece in x axis.
   * @param to_y Final position of piece in y axis.
   * @param moving_piece The piece to move.
   * @param first_move Specifies whether this is the piece's first move.
   * @param pawn_moved_two Specifies whether a pawn moved two squares forward.
   */
  Move(int from_x, int from_y, int to_x, int to_y, Piece &moving_piece,
       bool first_move = false, bool pawn_moved_two = false);

  /**
   * @brief Constructor for pawn promotion through normal move.
   *
   * @param from_x Initial position of piece in x axis.
   * @param from_y Initial position of piece in y axis.
   * @param to_x Final position of piece in x axis.
   * @param to_y Final position of piece in y axis.
   * @param moving_piece The piece to move.
   * @param promotion_piece_type Promote piece to this piece type.
   */
  Move(int from_x, int from_y, int to_x, int to_y, Piece &moving_piece,
       PieceType promotion_piece_type);

  /**
   * @brief Constructor for captures.
   *
   * @param from_x Initial position of piece in x axis.
   * @param from_y Initial position of piece in y axis.
   * @param to_x Final position of piece in x axis.
   * @param to_y Final position of piece in y axis.
   * @param moving_piece The piece to move.
   * @param captured_piece True if move captures another piece.
   * @param is_en_passant True if Pawn will capture En Passant.
   * @param first_move Specifies whether this is the piece's first move.
   */
  Move(int from_x, int from_y, int to_x, int to_y, Piece &moving_piece,
       Piece &captured_piece, bool first_move = false,
       bool is_en_passant = false);

  /**
   * @brief Constructor for pawn promotion through captures.
   *
   * @param from_x Initial position of piece in x axis.
   * @param from_y Initial position of piece in y axis.
   * @param to_x Final position of piece in x axis.
   * @param to_y Final position of piece in y axis.
   * @param moving_piece The piece to move.
   * @param captured_piece True if move captures another piece.
   * @param promotion_piece_type Promote piece to this piece type.
   */
  Move(int from_x, int from_y, int to_x, int to_y, Piece &moving_piece,
       Piece &captured_piece, PieceType promotion_piece_type);
};

/**
 * @brief Generates all possible moves for a given pawn.
 *
 * @param board Reference of the current board state.
 * @param x The x-coordinate of the pawn.
 * @param y The y-coordinate of the pawn.
 * @param possible_moves Reference to the list of possible moves of current
 * board_state.
 * @return A vector of possible moves.
 */
void generatePawnMove(std::array<std::array<Piece, 8>, 8> &board, int x,
                      int y, std::vector<Move> &possible_moves);

/**
 * @brief Generates all possible moves for a given knight.
 *
 * @param board Reference of the current board state.
 * @param x The x-coordinate of the knight.
 * @param y The y-coordinate of the knight.
 * @param possible_moves Reference to the list of possible moves of current
 * board_state.
 * @return A vector of possible moves.
 */
void generateKnightMove(std::array<std::array<Piece, 8>, 8> &board, int x,
                        int y, std::vector<Move> &possible_moves);

/**
 * @brief Generates all possible moves for a given bishop.
 *
 * @param board Reference of the current board state.
 * @param x The x-coordinate of the bishop.
 * @param y The y-coordinate of the bishop.
 * @param possible_moves Reference to the list of possible moves of current
 * board_state.
 * @return A vector of possible moves.
 */
void generateBishopMove(std::array<std::array<Piece, 8>, 8> &board, int x,
                        int y, std::vector<Move> &possible_moves);

/**
 * @brief Generates all possible moves for a given rook.
 *
 * @param board Reference of the current board state.
 * @param x The x-coordinate of the rook.
 * @param y The y-coordinate of the rook.
 * @param possible_moves Reference to the list of possible moves of current
 * board_state.
 * @return A vector of possible moves.
 */
void generateRookMove(std::array<std::array<Piece, 8>, 8> &board, int x,
                      int y, std::vector<Move> &possible_moves);

/**
 * @brief Generates all possible moves for a given queen.
 *
 * @param board Reference of the current board state.
 * @param x The x-coordinate of the queen.
 * @param y The y-coordinate of the queen.
 * @param possible_moves Reference to the list of possible moves of current
 * board_state.
 * @return A vector of possible moves.
 */
void generateQueenMove(std::array<std::array<Piece, 8>, 8> &board, int x,
                       int y, std::vector<Move> &possible_moves);

/**
 * @brief Generates all possible moves for a given king.
 *
 * @param board Reference of the current board state.
 * @param x The x-coordinate of the king.
 * @param y The y-coordinate of the king.
 * @param possible_moves Reference to the list of possible moves of current
 * board_state.
 * @return A vector of possible moves.
 */
void generateKingMove(std::array<std::array<Piece, 8>, 8> &board, int x,
                      int y, std::vector<Move> &possible_moves);

#endif // MOVE_GENERATOR_H