#ifndef PIECEBASE_H
#define PIECEBASE_H

#include <string>
#include <vector>

/**
 * @brief Enum to represent all possible chess pieces.
 */
enum PieceType {
    EMPTY = 0,
    PAWN_WHITE,
    KNIGHT_WHITE,
    BISHOP_WHITE,
    ROOK_WHITE,
    QUEEN_WHITE,
    KING_WHITE,
    PAWN_BLACK,
    KNIGHT_BLACK,
    BISHOP_BLACK,
    ROOK_BLACK,
    QUEEN_BLACK,
    KING_BLACK
};

/**
 * @brief A structure to represent chess moves.
 */
struct Move {
    int from_x, from_y;
    int to_x, to_y;
    int capturedPiece;
    int promotion;
    bool isCastling;
    int rook_from_x, rook_from_y;
    int rook_to_x, rook_to_y;
    bool isEnPassant;

    Move(int fx, int fy, int tx, int ty, int captured = -1, int promo = -1, bool castling = false, int rfx = -1,
         int rfy = -1, int rtx = -1, int rty = -1, bool enPassant = false);
};

class PieceBase
{
private:
    /**
     * @brief Type of piece, value from PieceType enum.
     */
    const PieceType piece_name;

protected:
    /**
     * @brief Value of a piece used for board state evaluation.
     */

    const int piece_value;
    /**
     * @brief Value of piece on specific position on the baord. Used for board state evaluation.
     */

    const std::array<std::array<PieceType, 8>, 8> position_value;

    /**
     * @brief Over all piece value as calculated by evaluation in board state.
     */
    int evaluation_value;

    /**
     *
     */
    bool alive;

    /**
     * @brief Calculates piece value.
     * @param moves The move count maybe used for evaluating piece value.
     * @param pos_x Piece position on the x axis.
     * @param pos_y Piece position on the y axis.
     */
    virtual void evaluatedValue(const int& moves, const int& pos_x, const int& pos_y) = 0;

public:
    virtual Move generateValidMoves() = 0;
    int getEvaluatedValue() const;
    std::string getPieceName() const;
};

#endif
