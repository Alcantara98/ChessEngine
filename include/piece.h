#ifndef PIECEBASE_H
#define PIECEBASE_H

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
 * @brief A structure to represent chess moves.
 */
struct Move {
    Piece moving_piece;
    Piece captured_piece;
    Piece promotion_piece;
    int from_x, from_y;
    int to_x, to_y;
    bool is_castling;
    bool is_castling_king_side;
    bool is_en_passant;

    /**
     * @param moving_piece The piece to move.
     * @param captured_piece True if move captures another piece.
     * @param promotion_piece Promote piece to this.
     * @param from_x Initial position of piece in x axis.
     * @param from_y Initial position of piece in y axis.
     * @param to_x Final position of piece in x axis.
     * @param to_y Final position of piece in y axis.
     * @param is_castling True if King is castling.
     * @param is_castling_king_side True if King castles King Side.
     * @param is_en_passant True if Pawn will capture En Passant.
     */
    Move(Piece moving_piece = Piece(), Piece captured_piece = Piece(), Piece promotion_piece = Piece(), int from_x,
         int from_y, int to_x, int to_y, bool is_castling = false, bool is_castling_king_side = true,
         bool is_en_passant = false);
};

/**
 * @brief A structure to represent chess pieces.
 */
struct Piece {
    PieceType type;
    PieceColor color;
    bool moved;
    int value;
    bool pawn_moved_two;

    Piece();

    /**
     * @param type Type of piece (Look at Enum PieceType).
     * @param color Which color the piece is (Look at Enum PieceColor).
     * @param moved Whether the piece has moved or not.
     * @param pawn_moved_two Whether the pawn's last move is two squares forward.
     */
    Piece(PieceType type, PieceColor color, int value, bool moved = false, bool pawn_moved_two = false);
};

#endif
