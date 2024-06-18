#include "board_state.h"

#include "piece.h"

BoardState::BoardState() { resetBoard(); }

BoardState::BoardState(const std::array<std::array<PieceType, 8>, 8>& input_chess_board)
    : chess_board(input_chess_board)
{
}

void BoardState::resetBoard()
{
    // Set empty squares.
    for(int y = 2; y < 6; ++y) {
        for(int x = 0; x < 8; ++x) {
            chess_board[x][y] = PieceType::EMPTY;
        }
    }
    // Set Pawns.
    for(int x = 0; x < 8; ++x) {
        chess_board[x][1] = PieceType::PAWN_WHITE;
    }
    for(int x = 0; x < 8; ++x) {
        chess_board[x][6] = PieceType::PAWN_BLACK;
    }
    // Set Rooks.
    chess_board[0][0] = PieceType::ROOK_WHITE;
    chess_board[7][0] = PieceType::ROOK_WHITE;
    chess_board[0][7] = PieceType::ROOK_BLACK;
    chess_board[7][7] = PieceType::ROOK_BLACK;
    // Set Knights.
    chess_board[1][0] = PieceType::KNIGHT_WHITE;
    chess_board[6][0] = PieceType::KNIGHT_WHITE;
    chess_board[1][7] = PieceType::KNIGHT_BLACK;
    chess_board[6][7] = PieceType::KNIGHT_BLACK;
    // Set Bishops.
    chess_board[2][0] = PieceType::BISHOP_WHITE;
    chess_board[5][0] = PieceType::BISHOP_WHITE;
    chess_board[2][7] = PieceType::BISHOP_BLACK;
    chess_board[5][7] = PieceType::BISHOP_BLACK;
    // Set Queens.
    chess_board[3][0] = PieceType::QUEEN_WHITE;
    chess_board[3][7] = PieceType::QUEEN_BLACK;
    // Set Kings.
    chess_board[4][0] = PieceType::KING_WHITE;
    chess_board[4][7] = PieceType::KING_BLACK;
}
