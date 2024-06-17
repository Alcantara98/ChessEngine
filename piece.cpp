#include <array>
#include <iostream>
#include <string>
#include <unordered_set>
#include <vector>

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
         int rfy = -1, int rtx = -1, int rty = -1, bool enPassant = false)
        : from_x(fx)
        , from_y(fy)
        , to_x(tx)
        , to_y(ty)
        , capturedPiece(captured)
        , promotion(promo)
        , isCastling(castling)
        , rook_from_x(rfx)
        , rook_from_y(rfy)
        , rook_to_x(rtx)
        , rook_to_y(rty)
        , isEnPassant(enPassant)
    {
    }
};

class Piece
{
private:
    const int piece_value;
    const int position_value[8][8];
    const std::string piece_name;

protected:
    int pos_x, pos_y;
    int value;
    static int move_count;

    virtual void updatePieceValue() = 0;

public:
    void updatePosition(int new_pos_x, int new_pos_y)
    {
        pos_x = new_pos_x;
        pos_y = new_pos_y;
        updatePieceValue();
    }

    virtual Move generateValidMoves() = 0;

    int getValue() { return value; }

    std::string getPieceName() { return piece_name; }
};
