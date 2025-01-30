#ifndef PIECE_BASE_H
#define PIECE_BASE_H

#include <string>
#include <vector>

/**
 * @brief Enum to represent chess piece type.
 */
enum class PieceType { EMPTY, PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING };

/**
 * @brief Enum to represent piece color.
 */
enum class PieceColor { NONE, WHITE, BLACK };

/**
 * @brief A structure to represent chess pieces.
 */
struct Piece {
  PieceType type;
  PieceColor color;
  bool moved;
  int value;
  bool pawn_moved_two;

  /**
   * @param type Type of piece (Look at Enum PieceType).
   * @param color Which color the piece is (Look at Enum PieceColor).
   * @param moved Whether the piece has moved or not.
   * @param pawn_moved_two Whether the pawn's last move is two squares forward.
   */
  Piece(PieceType type = PieceType::EMPTY, PieceColor color = PieceColor::NONE,
        int value = 0, bool moved = false, bool pawn_moved_two = false);
};

#endif