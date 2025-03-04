#include "piece.h"

namespace engine::parts
{
Piece::Piece(PieceType piece_type, PieceColor piece_color, bool piece_has_moved)
    : piece_type(piece_type), piece_color(piece_color),
      piece_has_moved(piece_has_moved)

{
}
} // namespace engine::parts