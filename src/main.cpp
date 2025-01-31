#include "best_move_finder.h"

#include <random>
#include <stdio.h>
#include <iostream>

int main(int argc, char **argv) {
  BoardState my_board = BoardState();
  BestMoveFinder my_engine = BestMoveFinder(my_board);

  std::random_device rd;
  std::mt19937 gen(rd());

  // Create a uniform distribution in the range [0, 10]
  std::uniform_int_distribution<> dis(0, 20);
  for (int i = 0; i < 50; ++i) {
    my_engine.calculate_possible_moves();
    my_board.apply_move(my_engine.possible_moves[dis(gen)]);
    my_board.printBoard();
    printf("\n");
    my_engine.possible_moves.clear();
    
    // Wait for user to press Enter
    std::cin.get();
  }
}
