#ifndef CHESS_ENGINE
#define CHESS_ENGINE

#include "board_state.h"
#include "cross_plat_functions.h"
#include "fen_interface.h"
#include "move_interface.h"
#include "search_engine.h"

#include <functional>
#include <sstream>

namespace engine
{
/**
 * @brief Class that combines all the components of the chess engine.
 */
class ChessEngine
{
public:
  // CONSTRUCTORS

  /**
   * @brief Default Constructor - Initializes the Chess Engine.
   */
  ChessEngine();

  // FUNCTIONS

  /**
   * @brief State machine for the chess engine.
   */
  void state_machine();

private:
  // PROPERTIES

  /// @brief Board state object.
  parts::BoardState game_board_state;

  /// @brief Best move finder object.
  parts::SearchEngine search_engine;

  /// @brief Move interface object.
  parts::MoveInterface move_interface;

  /// @brief Player color.
  parts::PieceColor player_color = parts::PieceColor::NONE;

  /// @brief Flag to exit current state.
  bool exit_state = false;

  /// @brief Flag to check if game is over.
  bool game_over = false;

  /// @brief Flag to allow pondering.
  bool allow_pondering = false;

  /// @brief Current state. Uses main_menu_state by default.
  void (ChessEngine::*current_state)() = &ChessEngine::main_menu_state;

  /// @brief Current state name.
  std::string current_state_name = parts::MAIN_MENU_STATE;

  // FUNCTIONS

  /**
   * @brief Changes the current state.
   *
   * @param new_state The new state to change to (function pointer to a state).
   *
   * @note States are defined as void functions with no parameters.
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
   * @brief Checks if the game is over and handles the end game scenario.
   *
   * @details This function checks for conditions such as checkmate, stalemate,
   * or draw and updates the game state accordingly.
   */
  void check_and_handle_if_game_over();

  /**
   * @brief Setup the chess board.
   *
   * @details Asks the user if they want to set up a custom board or use the
   * default board. If the user chooses a custom board, they will input a
   * FEN string to set up the board.
   *
   * @note See setup_custom_board in fen_interface.h for more details.
   */
  void setup_chess_board();

  /**
   * @brief Setup engine parameters.
   */
  void set_up_engine();

  /**
   * @brief Toggles engine heuristic parameters.
   *
   * @param user_input User input string to indicate which parameter to update.
   *
   * @return True if the user input is a valid parameter, false otherwise.
   */
  auto update_search_engine_parameters(const std::string &user_input) -> bool;

  /**
   * @brief Takes user input and handles the player's turn.
   */
  void handle_player_turn();

  /**
   * @brief Handles the engine's turn.
   */
  void handle_player_during_engine_turn();

  /**
   * @brief Handles state changes based on user input.
   *
   * @param user_input User input string.
   *
   * @return True if the input is a valid state change command, false otherwise.
   */
  auto handle_state_change_commands(const std::string &user_input) -> bool;

  /**
   * @brief Handles board manipulation commands such as undo or reset.
   *
   * @param user_input User input string.
   *
   * @return True if the input is a valid board manipulation command, false
   * otherwise.
   */
  auto handle_board_undo_reset_commands(const std::string &user_input) -> bool;

  /**
   * @brief Handle general commands. etc. menu, help.
   *
   * @param user_input User input command.
   */
  auto handle_general_commands(const std::string &user_input) -> bool;

  /**
   * @brief Gets a valid integer input from the user.
   *
   * @param user_message User message to display to the user.
   * @param min Minimum value.
   * @param max Maximum value.
   *
   * @return Valid integer input.
   */
  auto
  get_valid_int_input(const std::string &user_message, int min, int max) -> int;

  /**
   * @brief Gets a valid character input from the user.
   *
   * @param user_message User message to display to the user.
   * @param valid_chars Valid characters.
   *
   * @return Valid character input.
   */
  auto get_valid_char_input(const std::string &user_message,
                            const std::string &valid_chars) -> char;

  /**
   * @brief Prints all the moves applied to the board in order.
   *
   * @details This function iterates through the move stack and prints each move
   * in the order they were applied.
   */
  void print_applied_moves();

  /**
   * @brief Resets the game.
   */
  void reset_game();
};
} // namespace engine

#endif // CHESS_ENGINE