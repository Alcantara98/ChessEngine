#ifndef CHESS_ENGINE
#define CHESS_ENGINE

#include "best_move_finder.h"
#include "board_state.h"
#include "move_interface.h"
#include "position_evaluator.h"

class ChessEngine {
private:
  BoardState board_state;
  BestMoveFinder search_engine;
  MoveInterface move_interface;
  PositionEvaluator position_evaluator;

  /**
   * @brief Main game loop.
   */
  void game_loop();

public:
  /**
   * @brief Default Constructor - Initialises the Chess Engine.
   */
  ChessEngine();

  /**
   * @brief Starts the game.
   */
  void start_game();

  /**
   * @brief Checks if the current player is in checkmate.
   */
  bool is_checkmate();

  /**
   * @brief Checks if the current player is in stalemate.
   */
  bool is_stalemate();
};

#endif