#ifndef BOARDSTATE_H
#define BOARDSTATE_H

#include "piece.h"

#include <array>

class BoardState
{
private:
    // 8 x 8 array to represent a chess board.
    std::array<std::array<PieceType, 8>, 8> chess_board;

public:
    /**
     * @brief Default Constructor - sets chess_board using resetBoard.
     */
    BoardState();

    /**
     * @brief Parameterized constructor.
     * @param input_chess_board Initialises chess_baord to equal input_chess_board.
     */
    BoardState(const std::array<std::array<PieceType, 8>, 8>& input_chess_board);

    /**
     * @brief Resets chess board to default starting piece positions.
     */
    void resetBoard();
};

#endif
