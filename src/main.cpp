#include "move_generator.h"
#include <stdio.h>

int main(int argc, char **argv) {
  BoardState my_board = BoardState();
  std::vector<Move> possible_moves;
  generatePawnMove(my_board.chess_board, 4, 1, possible_moves);
  // Generate all possible moves
}
