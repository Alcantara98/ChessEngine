#ifndef PIECE_BASE_H
#define PIECE_BASE_H

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
  // Type of piece.
  PieceType piece_type;

  // Color of piece.
  PieceColor piece_color;

  // Whether the piece has moved.
  bool piece_has_moved;

  // Whether the pawn's last move is two squares forward.
  bool pawn_moved_two_squares;

  /**
   * @param piece_type Type of piece.
   * @param piece_color Color of the piece.
   * @param piece_has_moved Whether the piece has moved or not.
   * @param pawn_moved_two_squares Whether the pawn's last move is two squares
   * forward.
   */
  Piece(PieceType piece_type = PieceType::EMPTY,
        PieceColor piece_color = PieceColor::NONE, bool piece_has_moved = false,
        bool pawn_moved_two_squares = false);
};

#endif