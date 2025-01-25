#include "best_move_finder.h"
#include "move_generator.h"
#include <stdio.h>

int main(int argc, char **argv) {
  BoardState my_board = BoardState();

  BestMoveFinder move_finder = BestMoveFinder(my_board);
  // Generate all possible moves
  move_finder.calculate_possible_moves();
  int i = 0;
}
