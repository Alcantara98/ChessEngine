#ifndef UCI_H
#define UCI_H

#include "board_state.h"
#include "cross_plat_functions.h"
#include "search_engine.h"

#include <queue>

namespace engine::uci_engine
{
// UCI ENGINE CONSTANTS
// ESSENTIAL COMMANDS
const std::string UCI_COMMAND = "uci";
const std::string ISREADY_COMMAND = "isready";
const std::string UCINEWGAME_COMMAND = "ucinewgame";
const std::string POSITION_COMMAND = "position";
const std::string GO_COMMAND = "go";
const std::string STOP_COMMAND = "stop";
const std::string QUIT_COMMAND = "quit";

// POSITION COMMAND OPTIONS
const std::string FEN_COMMAND = "fen";
const std::string STARTPOS_COMMAND = "startpos";
const std::string MOVE_COMMAND = "moves";

// GO COMMAND OPTIONS
const std::string WTIME_COMMAND = "wtime";
const std::string BTIME_COMMAND = "btime";
const std::string WINC_COMMAND = "winc";
const std::string BINC_COMMAND = "binc";
const std::string MOVESTOGO_COMMAND = "movestogo";
const std::string MOVETIME_COMMAND = "movetime";
const std::string DEPTH_COMMAND = "depth";
const std::string MATE_COMMAND = "mate";
const std::string INFINITE_COMMAND = "infinite";

// ESSENTIAL COMMAND RESPONSES
const std::string ID_RESPONSE = "id";
const std::string UCIOK_RESPONSE = "uciok";
const std::string READYOK_RESPONSE = "readyok";
const std::string BESTMOVE_RESPONSE = "bestmove";

// SEARCH TIME CONSTANTS
const int FIRST_MOVES_STACK_SIZE = 4;
const int OPENING_MOVE_STACK_SIZE = 10;
const int OPENING_MOVE_STACK_TIME_FACTOR = 35;
const int MIDDLE_GAME_MOVE_STACK_TIME_FACTOR = 30;
const int END_GAME_MOVE_STACK_TIME_FACTOR = 25;

/**
 * @brief Class to handle the UCI interface.
 */
class UCIEngine
{
public:
  // CONSTRUCTORS

  /**
   * @brief Default Constructor - Initializes the UCI.
   */
  UCIEngine();

  // FUNCTIONS

  /**
   * @brief Starts the engine.
   *
   * @details This function starts the input_thread and handle_input_thread.
   */
  void start_engine();

private:
  // PROPERTIES

  /// @brief Exit flag for the engine.
  std::atomic<bool> exit_flag = false;

  /// @brief Thread for handling the input.
  std::thread handle_input_thread;

  /// @brief Board state object.
  engine::parts::BoardState game_board_state;

  /// @brief Best move finder object.
  engine::parts::SearchEngine search_engine;

  /// @brief Queue for storing the input.
  std::queue<std::string> input_queue;

  /// @brief Mutex for the input queue.
  std::mutex input_queue_mutex;

  /// @brief Condition variable for the input queue.
  std::condition_variable input_queue_cv;

  void handle_input();

  /**
   * @brief This function removes all whitespace from the beginning of the
   * string.
   *
   * @param string The string to skip whitespace in.
   */
  auto static skip_whitespace(std::string &string) -> void;

  /**
   * @brief Reads a token from the user input.
   *
   * @details This function skips whitespace and reads a token from the user
   * input. The token is removed from the user input.
   *
   * @param user_input The user input to read the token from.
   *
   * @return The token read from the user input.
   */
  auto static read_token(std::string &user_input) -> std::string;

  // ESSENTIAL COMMAND HANDLERS

  /**
   * @brief Handles the UCI command.
   *
   * @details This function prints the engine name and author.
   */
  void static handle_uci_command();

  /**
   * @brief Handles the ISREADY command.
   *
   * @details This function prints the readyok response.
   */
  void static handle_isready_command();

  /**
   * @brief Handles the UCINEWGAME command.
   *
   * @details This function resets the engine.
   */
  void handle_ucinewgame_command();

  /**
   * @brief Handles the POSITION command.
   *
   * @details This function sets the board state to the given position.
   */
  void handle_position_command(std::string &user_input);

  /**
   * @brief Handles the GO command.
   *
   * @details This function starts the search for the best move.
   */
  void handle_go_command(std::string &user_input);

  /**
   * @brief Handles the STOP command.
   *
   * @details This function stops the search for the best move and starts
   * pondering.
   */
  void handle_stop_command();

  /**
   * @brief Handles the QUIT command.
   *
   * @details This function exits the engine.
   */
  void handle_quit_command();

  // SEARCH FUNCTIONS

  /**
   * @brief Searches for the best move.
   *
   * @details This function searches for the best move with the given parameters
   * and starts pondering when the search is done.
   *
   * @param wtime_ms White time in milliseconds.
   * @para`m btime_ms Black time in milliseconds.
   * @param winc_ms White increment in milliseconds.
   * @param binc_ms Black increment in milliseconds.
   * @param movestogo Moves to go.
   * @param depth Depth.
   * @param movetime_ms Move time in milliseconds.
   * @param infinite Infinite mode.
   */
  void search_for_best_move(int wtime_ms,
                            int btime_ms,
                            int winc_ms,
                            int binc_ms,
                            int movestogo,
                            int depth,
                            int movetime_ms,
                            bool infinite);
};

} // namespace engine::uci_engine

#endif // UCI_H
