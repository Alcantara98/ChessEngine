#include <algorithm>
#include <array>
#include <iostream>
#include <vector>

enum Piece {
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

class BoardState
{
private:
    // 8 x 8 array to represent a chess board.
    std::array<std::array<Piece, 8>, 8> chess_board;

public:
    BoardState() { resetBoard(); }

    BoardState(const std::array<std::array<Piece, 8>, 8>& input_chess_board)
        : chess_board(input_chess_board)
    {
    }

    void resetBoard()
    {
        /**
         * @brief Reset chess board to default starting piece positions.
         */

        // Set empty squares.
        for(int y = 2; y < 6; ++y) {
            for(int x = 0; x < 8; ++x) {
                chess_board[x][y] = Piece::EMPTY;
            }
        }
        // Set Pawns.
        for(int x = 0; x < 8; ++x) {
            chess_board[x][1] = Piece::PAWN_WHITE;
        }
        for(int x = 0; x < 8; ++x) {
            chess_board[x][6] = Piece::PAWN_BLACK;
        }
        // Set Rooks.
        chess_board[0][0] = Piece::ROOK_WHITE;
        chess_board[7][0] = Piece::ROOK_WHITE;
        chess_board[0][7] = Piece::ROOK_BLACK;
        chess_board[7][7] = Piece::ROOK_BLACK;
        // Set Knights.
        chess_board[1][0] = Piece::KNIGHT_WHITE;
        chess_board[6][0] = Piece::KNIGHT_WHITE;
        chess_board[1][7] = Piece::KNIGHT_BLACK;
        chess_board[6][7] = Piece::KNIGHT_BLACK;
        // Set Bishops.
        chess_board[2][0] = Piece::BISHOP_WHITE;
        chess_board[5][0] = Piece::BISHOP_WHITE;
        chess_board[2][7] = Piece::BISHOP_BLACK;
        chess_board[5][7] = Piece::BISHOP_BLACK;
        // Set Queens.
        chess_board[3][0] = Piece::QUEEN_WHITE;
        chess_board[3][7] = Piece::QUEEN_BLACK;
        // Set Kings.
        chess_board[4][0] = Piece::KING_WHITE;
        chess_board[4][7] = Piece::KING_BLACK;
    }
};