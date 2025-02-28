#ifndef CHESS_ENGINE
#define CHESS_ENGINE

#include "board_state.h"
#include "move_interface.h"
#include "position_evaluator.h"
#include "search_engine.h"

namespace engine
{
/**
 * @brief Class that combineas all the components of the chess engine.
 */
class ChessEngine
{
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
  parts::BoardState game_board_state;

  // Best move finder object.
  parts::SearchEngine search_engine;

  // Move interface object.
  parts::MoveInterface move_interface;

  // Position evaluator object.
  parts::PositionEvaluator position_evaluator;

  // Player color.
  parts::PieceColor player_color;

  /**
   * @brief Main game loop.
   */
  void game_loop(int max_search_depth, bool show_performance);

  /**
   * @brief Checks if the current player is in checkmate.
   */
  auto is_checkmate() -> bool;

  /**
   * @brief Checks if the current player is in stalemate.
   */
  auto is_stalemate() -> bool;
};
} // namespace engine

#endif