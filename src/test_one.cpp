#include "test_one.h"
#include "move_interface.h"
#include "search_engine.h"

#include <cstdio>
#include <iostream>
#include <random>
#include <stack>

void testing_one() {
  BoardState my_board = BoardState();
  SearchEngine my_engine = SearchEngine(my_board);
  MoveInterface my_interface = MoveInterface(my_board);

  std::vector<std::string> move_strings{
      "pe2e4",  "pe7e5",  "ng1f3",  "nb8c6",   "bf1c4",    "ng8f6",  "pd2d3",
      "bf8c5",  "O-O",    "O-O",    "bc1g5",   "ph7h6",    "bg5xf6", "qd8xf6",
      "nb1c3",  "pd7d6",  "nc3d5",  "qf6d8",   "qd1d2",    "bc8e6",  "pa2a3",
      "pa7a6",  "pb2b4",  "bc5a7",  "bc4b3",   "kg8h8",    "pc2c4",  "be6xd5",
      "pc4xd5", "nc6d4",  "nf3xd4", "ba7xd4",  "ra1c1",    "pc7c5",  "pd5xc6",
      "pb7xc6", "rc1xc6", "qd8f6",  "rf1c1",   "rf8d8",    "rc6xa6", "pg7g5",
      "ra6xd6", "qf6xd6", "pa3a4",  "ra8c8",   "pa4a5",    "rd8f8",  "pa5a6",
      "rf8d8",  "pa6a7",  "rd8f8",  "pa7a8=q", "rf8d8",    "pb4b5",  "rd8f8",
      "pb5b6",  "rf8d8",  "pb6b7",  "rd8f8",   "pb7xc8=r", "rf8d8",  "rc8xd8"};

  // Test Applying Moves.
  int i = 0;
  for (; i < move_strings.size(); ++i) {
    std::string move_color;
    if (my_board.move_color == PieceColor::WHITE) {
      move_color = "White";
    } else {
      move_color = "Black";
    }
    printf("Move(%s): %d - %s\n", move_color.c_str(), i,
           move_strings[i].c_str());
    Move move = my_interface.input_to_move(
        my_engine.calculate_possible_moves(my_board), move_strings[i]);
    my_board.apply_move(move);
    my_board.print_board(PieceColor::WHITE);
    printf("\n");

    // Wait for user to press Enter
    std::cin.get();
  }

  // Test Undoing Moves.
  while (i > 0) {
    --i;
    printf("Undo Move: %d\n", i);

    my_board.undo_move();
    my_board.print_board(PieceColor::WHITE);
    printf("\n");
    std::cin.get();
  }
}