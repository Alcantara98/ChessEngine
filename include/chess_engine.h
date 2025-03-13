#ifndef CHESS_ENGINE
#define CHESS_ENGINE

#include "board_state.h"
#include "move_interface.h"
#include "search_engine.h"

#include <functional>
#include <sstream>

namespace engine
{
/**
 * @brief Class that combineas all the components of the chess engine.
 */
class ChessEngine
{
public:
  // CONSTRUCTORS

  /**
   * @brief Default Constructor - Initialises the Chess Engine.
   */
  ChessEngine();

  // FUNCTIONS

  /**
   * @brief State machine for the chess engine.
   */
  void state_machine();

private:
  // PROPERTIES

  // Board state object.
  parts::BoardState game_board_state;

  // Best move finder object.
  parts::SearchEngine search_engine;

  // Move interface object.
  parts::MoveInterface move_interface;

  // Player color.
  parts::PieceColor player_color = parts::PieceColor::NONE;

  // Flag to go exit current state.
  bool exit_state = false;

  // Flag to check if game is over.
  bool game_over = false;

  // Current state. Uses main_menu_state by default.
  void (ChessEngine::*current_state)() = &ChessEngine::main_menu_state;

  // Current state name.
  std::string current_state_name = parts::MAIN_MENU_STATE;

  // FUNCTIONS

  /**
   * @brief Changes the current state.
   *
   * @param new_state The new state to change to.
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
   * @brief Checks if the game is over.
   */
  void check_and_handle_if_game_over();

  /**
   * @brief Setup engine parameters.
   */
  void set_up_engine();

  /**
   * @brief Takes user input and handles the player's turn.
   */
  void handle_player_turn();

  /**
   * @brief Handles state changes based on user input.
   *
   * @param user_input User input.
   *
   * @return True if the input is a state change command.
   */
  auto handle_state_change_commands(const std::string &user_input) -> bool;

  /**
   * @brief Handles board manipulation commands.
   *
   * @param user_input User input.
   *
   * @return True if the input is a board manipulation command.
   */
  auto handle_board_undo_reset_commands(const std::string &user_input) -> bool;

  /**
   * @brief Gets a valid integer input from the user.
   *
   * @param user_message User message to display to the user.
   * @param min Minimum value.
   * @param max Maximum value.
   *
   * @return Valid integer input.
   */
  auto getValidIntInput(const std::string &user_message, int min,
                        int max) -> int;

  /**
   * @brief Gets a valid character input from the user.
   *
   * @param user_message User message to display to the user.
   * @param valid_chars Valid characters.
   *
   * @return Valid character input.
   */
  auto getValidCharInput(const std::string &user_message,
                         const std::string &valid_chars) -> char;

  /**
   * @brief Checks if the current player is in checkmate.
   *
   * @note If the king is checked and all possible moves result in a checked
   * king, it is a checkmate.
   */
  auto is_checkmate() -> bool;

  /**
   * @brief Checks if the current player is in stalemate.
   *
   * @note If the king is not checked and all possible moves result in a checked
   * king, it is a stalemate.
   */
  auto is_stalemate() -> bool;

  /**
   * @brief Prints all the moves applied to the board.
   */
  void print_applied_moves();

  /**
   * @brief Resets the game.
   */
  void reset_game();
};
} // namespace engine

#endif