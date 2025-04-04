#include "piece.h"

namespace engine::parts
{
// CONSTRUCTORS

Piece::Piece(int x_file,
             int y_rank,
             PieceType piece_type,
             PieceColor piece_color,
             bool piece_has_moved)
    : x_file(x_file), y_rank(y_rank), piece_type(piece_type),
      piece_color(piece_color), piece_has_moved(piece_has_moved)
{
}
} // namespace engine::parts