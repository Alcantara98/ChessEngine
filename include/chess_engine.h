#ifndef CHESS_ENGINE
#define CHESS_ENGINE

#include "best_move_finder.h"
#include "board_state.h"
#include "move_interface.h"
#include "position_evaluator.h"

class ChessEngine {
public:
  /**
   * @brief Default Constructor - Initialises the Chess Engine.
   */
  ChessEngine();

  /**
   * @brief Starts the game.
   */
  void start_game();

private:
  // Board state object.
  BoardState board_state;

  // Best move finder object.
  BestMoveFinder search_engine;

  // Move interface object.
  MoveInterface move_interface;

  // Position evaluator object.
  PositionEvaluator position_evaluator;

  // Player color.
  PieceColor player_color;

  /**
   * @brief Main game loop.
   */
  void game_loop(int max_search_depth, bool show_performance);

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