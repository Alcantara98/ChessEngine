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
 * @brief A structure to represent chess moves.
 */
struct Move {
    Piece moving_piece;
    Piece captured_piece;
    PieceType promotion_piece_type;
    int from_x, from_y;
    int to_x, to_y;
    bool is_castling;
    bool is_castling_king_side;
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
     * @param is_castling True if King is castling.
     * @param is_castling_king_side True if King castles King Side.
     * @param first_move Specifies whether this is the piece's first move.
     * @param pawn_moved_two Specifies whether a pawn moved two squares forward.
     */
    Move(int from_x, int from_y, int to_x, int to_y, Piece moving_piece, bool first_move = false,
         bool is_castling = false, bool is_castling_king_side = true, bool pawn_moved_two = false);

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
    Move(int from_x, int from_y, int to_x, int to_y, Piece moving_piece, PieceType promotion_piece_type);

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
    Move(int from_x, int from_y, int to_x, int to_y, Piece moving_piece, Piece captured_piece,
         bool first_move = false, bool is_en_passant = false);

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
    Move(int from_x, int from_y, int to_x, int to_y, Piece moving_piece, Piece captured_piece,
         PieceType promotion_piece_type);
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

    /**
     * @param type Type of piece (Look at Enum PieceType).
     * @param color Which color the piece is (Look at Enum PieceColor).
     * @param moved Whether the piece has moved or not.
     * @param pawn_moved_two Whether the pawn's last move is two squares forward.
     */
    Piece(PieceType type = PieceType::EMPTY, PieceColor color = PieceColor::NONE, int value = 0, bool moved = false,
          bool pawn_moved_two = false);
};

#endif