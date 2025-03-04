#ifndef PIECE_BASE_H
#define PIECE_BASE_H

#include <cstdint>

namespace engine::parts
{
/**
 * @brief Enum to represent chess piece type.
 */
enum class PieceType : std::uint8_t
{
  EMPTY,
  PAWN,
  KNIGHT,
  BISHOP,
  ROOK,
  QUEEN,
  KING
};

/**
 * @brief Enum to represent piece color.
 */
enum class PieceColor : std::uint8_t
{
  NONE,
  WHITE,
  BLACK
};

/**
 * @brief A structure to represent chess pieces.
 */
struct Piece
{
  // Type of piece.
  PieceType piece_type;

  // Color of piece.
  PieceColor piece_color;

  // Whether the piece has moved.
  bool piece_has_moved;

  /**
   * @brief Constructor with default values.
   *
   * @param piece_type Type of piece.
   * @param piece_color Color of the piece.
   * @param piece_has_moved Whether the piece has moved or not.
   * forward.
   */
  Piece(PieceType piece_type = PieceType::EMPTY,
        PieceColor piece_color = PieceColor::NONE,
        bool piece_has_moved = false);
};
} // namespace engine::parts

#endif