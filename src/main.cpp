#include "best_move_finder.h"
#include "move_interface.h"

#include <iostream>
#include <random>
#include <stack>
#include <stdio.h>

int main(int argc, char **argv) {
  BoardState my_board = BoardState();
  BestMoveFinder my_engine = BestMoveFinder(my_board);
  MoveInterface my_interface =
      MoveInterface(my_board, my_engine.possible_moves);

  std::vector<std::string> moves{"pe2e4", "pe7e5", "ng1f3",
                                 "nb8c6", "bf1c4", "ng8f6"};

  // Test Applying Moves.
  int i = 0;
  for (; i < moves.size(); ++i) {
    printf("Move: %d - %s\n", i, moves[i].c_str());
    my_engine.calculate_possible_moves();
    Move move = my_interface.input_to_move(moves[i]);
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
