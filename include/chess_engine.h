#ifndef CHESS_ENGINE
#define CHESS_ENGINE

#include "board_state.h"
#include "move_interface.h"
#include "position_evaluator.h"
#include "search_engine.h"

#include <functional>

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
   * @brief State machine for the chess engine.
   */
  void state_machine();

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

  // Flag to go exit current state.
  bool exit_state = false;

  // Flag to check if game is over.
  bool game_over = false;

  // Current state. Uses main_menu_state by default.
  void (ChessEngine::*current_state)() = &ChessEngine::main_menu_state;

  /**
   * @brief Changes the current state.
   */
  void change_state(void (ChessEngine::*new_state)());

  /**
   * @brief Starts the game.
   */
  void main_menu_state();

  /**
   * @brief Player vs player game loop.
   */
  void player_vs_player_state();

  /**
   * @brief Engine vs player game loop.
   */
  void engine_vs_player_state();

  /**
   * @brief Setup engine parameters.
   */
  void set_up_engine();

  /**
   * @brief Takes user input and handles the player's turn.
   */
  void handle_player_turn();

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