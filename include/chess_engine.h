#ifndef CHESS_ENGINE
#define CHESS_ENGINE

#include "board_state.h"
#include "best_move_finder.h"
#include "move_interface.h"
#include "position_evaluator.h"


class ChessEngine {
private:
  BoardState board_state;
  BestMoveFinder search_engine;
  MoveInterface move_interface;
  PositionEvaluator position_evaluator;

public:
  ChessEngine();

  void start_game();

  bool is_checkmate();

  bool is_stalemate();
};

#endif