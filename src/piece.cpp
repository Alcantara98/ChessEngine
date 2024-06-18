#include "piece.h"

#include <array>
#include <iostream>
#include <string>
#include <unordered_set>
#include <vector>

Move::Move(int fx, int fy, int tx, int ty, int captured = -1, int promo = -1, bool castling = false, int rfx = -1,
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

void PieceBase::updatePosition(int new_pos_x, int new_pos_y)
{
    pos_x = new_pos_x;
    pos_y = new_pos_y;
    updatePieceValue();
}

int PieceBase::getValue() const { return value; }

std::string PieceBase::getPieceName() const { return piece_name; }
