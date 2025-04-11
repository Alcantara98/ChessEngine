#ifndef PIECE_BASE_H
#define PIECE_BASE_H

#include <cstdint>

namespace engine::parts
{
/**
 * @brief Enum to represent chess piece type.
 *
 * @details This enum defines the types of chess pieces, including:
 * - PAWN: Represents a pawn.
 * - KNIGHT: Represents a knight.
 * - BISHOP: Represents a bishop.
 * - ROOK: Represents a rook.
 * - QUEEN: Represents a queen.
 * - KING: Represents a king.
 * - EMPTY: Represents an empty square.
 */
enum class PieceType : std::uint8_t
{
  PAWN,
  KNIGHT,
  BISHOP,
  ROOK,
  QUEEN,
  KING,
  EMPTY,
};

/**
 * @brief Enum to represent piece color.
 *
 * @details This enum defines the colors of chess pieces, including:
 * - WHITE: Represents a white piece.
 * - BLACK: Represents a black piece.
 * - NONE: Represents no color (e.g., for an empty square).
 */
enum class PieceColor : std::uint8_t
{
  WHITE,
  BLACK,
  NONE,
};

/**
 * @brief A structure to represent chess pieces.
 */
struct Piece
{
  // PROPERTIES

  /// @brief Type of piece.
  PieceType piece_type;

  /// @brief Color of piece.
  PieceColor piece_color;

  /// @brief Whether the piece has moved.
  bool piece_has_moved;

  /// @brief The x coordinate of the piece on the chess board.
  int x_file;

  /// @brief The y coordinate of the piece on the chess board.
  int y_rank;

  // CONSTRUCTORS

  /**
   * @brief Constructor with default values.
   *
   * @param piece_type Type of piece (default is EMPTY).
   * @param piece_color Color of the piece (default is NONE).
   * @param piece_has_moved Whether the piece has moved or not (default is
   * false).
   */
  Piece(int x_file,
        int y_rank,
        PieceType piece_type = PieceType::EMPTY,
        PieceColor piece_color = PieceColor::NONE,
        bool piece_has_moved = false);
};
} // namespace engine::parts

#endif