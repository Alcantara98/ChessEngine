#include "piece.h"

namespace engine::parts
{
Piece::Piece(PieceType piece_type, PieceColor piece_color, bool piece_has_moved,
             bool pawn_moved_two_squares)
    : piece_type(piece_type), piece_color(piece_color),
      piece_has_moved(piece_has_moved),
      pawn_moved_two_squares(pawn_moved_two_squares)
{
}
} // namespace engine::parts