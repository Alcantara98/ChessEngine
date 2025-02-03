#include "best_move_finder.h"

#include <iostream>
#include <random>
#include <stack>
#include <stdio.h>

int main(int argc, char **argv) {
  BoardState my_board = BoardState();
  BestMoveFinder my_engine = BestMoveFinder(my_board);

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(0, 10);

  std::array<int, 10> move_num = {13, 1, 9, 1, 9, 1, 5, 1, 7, 1};

  // Test Applying Moves.
  int i = 0;
  for (; i < 10; ++i) {
    printf("Move: %d\n", i);
    my_engine.calculate_possible_moves();
    Move &move = my_engine.possible_moves[move_num[i]];
    my_board.apply_move(move);
    my_board.print_board();
    printf("\n");
    my_engine.possible_moves.clear();

    // Wait for user to press Enter
    std::cin.get();
  }

  // Test Undoing Moves.
  while (i > 0) {
    --i;
    printf("Undo Move: %d\n", i);

    my_board.undo_move();
    my_board.print_board();
    printf("\n");
    std::cin.get();
  }
}
