#include "chess_engine.h"

namespace engine
{
// CONSTRUCTORS

ChessEngine::ChessEngine()
    : search_engine(parts::SearchEngine(game_board_state)),
      move_interface(parts::MoveInterface(game_board_state))
{
}

// PUBLIC FUNCTIONS

void ChessEngine::state_machine()
{
  while (true)
  {
    exit_state = false;
    (this->*current_state)();
  }
}

// PRIVATE FUNCTIONS

void ChessEngine::change_state(void (ChessEngine::*new_state)())
{
  player_color = parts::PieceColor::NONE;
  if (new_state == &ChessEngine::main_menu_state)
  {
    current_state_name = parts::MAIN_MENU_STATE;
  }
  else if (new_state == &ChessEngine::player_vs_player_state)
  {
    current_state_name = parts::PLAYER_VS_PLAYER_STATE;
  }
  else if (new_state == &ChessEngine::engine_vs_player_state)
  {
    current_state_name = parts::ENGINE_VS_PLAYER_STATE;
  }
  if (game_over)
  {
    game_over = false;
  }
  current_state = new_state;
  reset_game();
}

void ChessEngine::main_menu_state()
{
  printf("\n~%s~\n", parts::MAIN_MENU_STATE.c_str());
  std::string user_input;
  while (!exit_state)
  {
    printf("%s", parts::HELP_MESSAGE.c_str());
    std::string user_message = "Play Against Engine (y = Yes, n = No): ";
    user_input = getValidCharInput(user_message, "yn");

    if (user_input == "y")
    {
      change_state(&ChessEngine::engine_vs_player_state);
    }
    else if (user_input == "n")
    {
      change_state(&ChessEngine::player_vs_player_state);
    }
  }
}

void ChessEngine::player_vs_player_state()
{
  printf("\n~%s~\n\n -- Have Fun --\n", parts::PLAYER_VS_PLAYER_STATE.c_str());

  setup_chess_board();

  while (!exit_state)
  {
    game_board_state.print_board(game_board_state.color_to_move);

    check_and_handle_if_game_over();

    handle_player_turn();
  }
}

void ChessEngine::engine_vs_player_state()
{
  printf("\n~%s~\n\n -- Good Luck! --\n",
         parts::ENGINE_VS_PLAYER_STATE.c_str());

  setup_chess_board();
  set_up_engine();

  // Print initial board state.
  printf("Player Color: %s\n",
         player_color == parts::PieceColor::WHITE ? "White" : "Black");
  game_board_state.print_board(player_color);

  while (!exit_state)
  {
    check_and_handle_if_game_over();

    if (!game_over && game_board_state.color_to_move != player_color)
    {
      // Engine's turn.
      search_engine.handle_engine_turn();
      handle_player_during_engine_turn();
      search_engine.stop_engine_turn();
    }
    else
    {
      // Player's turn.
      handle_player_turn();
    }
    game_board_state.print_board(player_color);
  }
}

void ChessEngine::check_and_handle_if_game_over()
{
  if (!game_over && search_engine.is_stalemate())
  {
    printf("\nStalemate, It's a draw!\n");
    game_over = true;
  }
  if (!game_over && search_engine.is_checkmate())
  {
    std::string winner =
        game_board_state.color_to_move == parts::PieceColor::WHITE ? "Black"
                                                                   : "White";
    printf("\nCheckmate, %s WINS!\n", winner.c_str());
    game_over = true;
  }
}

void ChessEngine::setup_chess_board()
{
  std::string user_message = "Would you like to setup a custom board?";
  char custom_board_char = getValidCharInput(user_message, "yn");

  if (custom_board_char == 'y')
  {
    while (!exit_state)
    {
      std::string board_configuration;
      printf("Enter Custom Board Configuration: ");
      std::cin >> board_configuration;

      if (handle_general_commands(board_configuration))
      {
        continue;
      }
      if (handle_state_change_commands(board_configuration))
      {
        continue;
      }
      if (game_board_state.setup_custom_board(board_configuration))
      {
        break;
      }

      printf(
          "Invalid Board Configuration\n\nConfiguration is a string of 65 "
          "characters representing the board state.\nThe first 64 characters "
          "represent the board state from A1 to H8.\nThe last character "
          "represents the color to move.\nThe characters are as follows:\n\n"
          " - R - White Rook\n"
          " - N - White Knight\n"
          " - B - White Bishop\n"
          " - Q - White Queen\n"
          " - K - White King\n"
          " - P - White Pawn\n"
          " - r - Black Rook\n"
          " - n - Black Knight\n"
          " - b - Black Bishop\n"
          " - q - Black Queen\n"
          " - k - Black King\n"
          " - p - Black Pawn\n"
          " - - - Empty Square\n"
          " - w - White to move\n"
          " - b - Black to move\n\n"
          "Example (Default Chess Starting Configuration): "
          "RNBQKBNRPPPPPPPP--------------------------------"
          "pppppppprnbqkbnrw\n");
    }
  }
}

void ChessEngine::set_up_engine()
{
  const std::string update_depth = "update-depth";
  const std::string update_timelimit = "update-timelimit";
  const std::string update_pondering = "update-pondering";
  const std::string update_info = "update-info";
  (void)update_search_engine_parameters(update_depth);
  (void)update_search_engine_parameters(update_timelimit);
  (void)update_search_engine_parameters(update_pondering);
  (void)update_search_engine_parameters(update_info);

  std::string user_message = "Enter Player Color (w = White, b = Black)";
  const char user_color = getValidCharInput(user_message, "wb");

  // Set player and engine colors.
  if (user_color == parts::WHITE_PIECE_CHAR)
  {
    player_color = parts::PieceColor::WHITE;
    search_engine.engine_color = parts::PieceColor::BLACK;
  }
  else
  {
    player_color = parts::PieceColor::BLACK;
    search_engine.engine_color = parts::PieceColor::WHITE;
  }
}

auto ChessEngine::update_search_engine_parameters(const std::string &user_input)
    -> bool
{
  std::string user_message;

  // If the engine is pondering, stop pondering to update parameters.
  if (search_engine.engine_is_pondering &&
      (user_input == "update-depth" || user_input == "update-timelimit" ||
       user_input == "update-window" || user_input == "update-info" ||
       user_input == "update-pondering"))
  {
    search_engine.stop_engine_pondering();
  }

  if (user_input == "update-depth")
  {
    user_message = "Please Enter Engine Depth";
    int search_depth =
        getValidIntInput(user_message, 1, parts::MAX_SEARCH_DEPTH);
    search_engine.max_search_depth = search_depth;
  }
  else if (user_input == "update-timelimit")
  {
    user_message = "Enter Search Time for Each Move in Milliseconds";
    int search_time = getValidIntInput(user_message, 1, parts::MAX_SEARCH_TIME);
    search_engine.max_search_time_milliseconds = search_time;
  }
  else if (user_input == "update-window")
  {
    user_message = "Allow Aspiration Window?";
    char use_window_char = getValidCharInput(user_message, "yn");
    search_engine.use_aspiration_window = use_window_char == 'y';
  }
  else if (user_input == "update-info")
  {
    user_message = "Show Performance?";
    char show_performance_char = getValidCharInput(user_message, "yn");
    search_engine.show_performance = show_performance_char == 'y';

    if (allow_pondering)
    {
      user_message = "Show Pondering Performance?";
      char show_ponder_performance_char = getValidCharInput(user_message, "yn");
      search_engine.show_ponder_performance =
          show_ponder_performance_char == 'y';
    }

    user_message = "Show All Move Evaluations?";
    char show_move_evaluations_char = getValidCharInput(user_message, "yn");
    search_engine.show_move_evaluations = show_move_evaluations_char == 'y';
  }
  else if (user_input == "update-pondering")
  {
    user_message = "Allow Pondering?";
    char allow_pondering_char = getValidCharInput(user_message, "yn");
    allow_pondering = allow_pondering_char == 'y';
  }
  else
  {
    return false;
  }
  return true;
}

void ChessEngine::handle_player_turn()
{
  // Indicate which color's turn it is.
  if (!game_over)
  {
    std::string color_to_move =
        game_board_state.color_to_move == parts::PieceColor::WHITE ? "White"
                                                                   : "Black";
    printf("%s's Turn\n", color_to_move.c_str());
  }

  std::string user_input;
  while (!exit_state)
  {
    if (allow_pondering && !search_engine.engine_is_pondering && !game_over)
    {
      // Start pondering if allowed during player's turn.
      search_engine.start_engine_pondering();
    }
    if (game_over)
    {
      printf("%s", parts::GAME_OVER_HELP_MESSAGE.c_str());
    }
    else
    {
      printf("Enter move: ");
    }
    // Get user input.
    std::cin >> user_input;

    if (handle_state_change_commands(user_input))
    {
      continue;
    }
    if (handle_general_commands(user_input))
    {
      continue;
    }
    if (update_search_engine_parameters(user_input))
    {
      continue;
    }
    if (handle_board_undo_reset_commands(user_input))
    {
      break;
    }
    if (user_input == "print-moves")
    {
      printf("\nMoves Played:\n");
      print_applied_moves();
      printf("\n");
      continue;
    }
    if (move_interface.input_to_move(
            parts::move_generator::calculate_possible_moves(game_board_state),
            user_input))
    {
      // Move was valid and played, end player's turn.
      break;
    }
  }
  if (search_engine.engine_is_pondering)
  {
    // End pondering after player's turn.
    search_engine.stop_engine_pondering();
  }
}

void ChessEngine::handle_player_during_engine_turn()
{
  while (!exit_state && search_engine.engine_is_searching())
  {
    std::string userInput;

    while (search_engine.engine_is_searching())
    {
      // Check if input is available
      if (inputAvailable())
      {
        // Read input
        std::getline(std::cin, userInput);
        break;
      }

      // No input, check again later after a delay of 100ms
      std::this_thread::sleep_for(
          std::chrono::milliseconds(parts::INPUT_DELAY_TIME));
    }

    if (!search_engine.engine_is_searching())
    {
      break;
    }
    if (userInput == "stop-search")
    {
      break;
    }
    if (handle_state_change_commands(userInput))
    {
      break;
    }
    if (handle_general_commands(userInput))
    {
      continue;
    }
    if (handle_board_undo_reset_commands(userInput))
    {
      return;
    }
  }
}

auto ChessEngine::handle_state_change_commands(const std::string &user_input)
    -> bool
{
  if (user_input == "menu")
  {
    change_state(&ChessEngine::main_menu_state);
  }
  else if (user_input == "play-engine")
  {
    change_state(&ChessEngine::engine_vs_player_state);
  }
  else if (user_input == "play-player")
  {
    change_state(&ChessEngine::player_vs_player_state);
  }
  else
  {
    return false;
  }
  return true;
}

auto ChessEngine::handle_board_undo_reset_commands(
    const std::string &user_input) -> bool
{
  if (user_input == "undo" || user_input == "redo" || user_input == "reset")
  {
    // Need to stop engine from searching or pondering before doing any of
    // these commands.
    if (search_engine.engine_is_searching())
    {
      search_engine.stop_engine_turn();
    }
    if (search_engine.engine_is_pondering)
    {
      search_engine.stop_engine_pondering();
    }
  }
  if (user_input == "undo")
  {
    game_board_state.undo_move();
    if (current_state == &ChessEngine::engine_vs_player_state &&
        game_board_state.color_to_move != player_color)
    {
      search_engine.pop_last_move_eval();
      game_board_state.undo_move();
    }
    game_board_state.print_board(game_board_state.color_to_move);
  }
  else if (user_input == "redo" &&
           current_state == &ChessEngine::engine_vs_player_state)
  {
    search_engine.transposition_table.clear();
    game_board_state.undo_move();
    search_engine.pop_last_move_eval();
    if (game_board_state.color_to_move == player_color)
    {
      game_board_state.undo_move();
    }
  }
  else if (user_input == "reset")
  {
    reset_game();
  }
  else
  {
    return false;
  }

  // If it was game over and we undid the last move, it is no longer game
  // over.
  if (game_over)
  {
    game_over = false;
  }
  return true;
}

auto ChessEngine::handle_general_commands(const std::string &user_input) -> bool
{
  if (user_input == "exit")
  {
    printf("\n -- Goodbye G! --\n\n");
    // Stop engine from searching or pondering before exiting to prevent
    // thread issues.
    if (search_engine.engine_is_searching())
    {
      search_engine.stop_engine_turn();
    }
    if (search_engine.engine_is_pondering)
    {
      search_engine.stop_engine_pondering();
    }
    exit(0);
  }
  else if (user_input == "help")
  {
    printf("Current State: %s\n", current_state_name.c_str());
    printf("%s", parts::HELP_MESSAGE.c_str());
  }
  else
  {
    return false;
  }
  return true;
}

auto ChessEngine::getValidIntInput(const std::string &user_message,
                                   int min,
                                   int max) -> int
{
  int int_input;
  std::string user_input;

  while (!exit_state)
  {
    printf("%s (%d-%d): ", user_message.c_str(), min, max);

    // Get user input.
    std::cin >> user_input;

    if (handle_state_change_commands(user_input))
    {
      continue;
    }
    if (handle_general_commands(user_input))
    {
      continue;
    }

    // Use a stringstream to extract the integer.
    std::istringstream stream(user_input);

    // Try to convert the string to an integer.
    if (stream >> int_input && int_input >= min && int_input <= max &&
        stream.eof())
    {
      return int_input;
    }
    printf("Invalid input or out of range. Please try again\n\n");
  }
  return 0;
}

auto ChessEngine::getValidCharInput(const std::string &user_message,
                                    const std::string &valid_chars) -> char
{
  char char_input;
  std::string user_input;

  while (!exit_state)
  {
    printf("%s (%s): ", user_message.c_str(), valid_chars.c_str());

    // Get user input.
    std::cin >> user_input;

    if (handle_state_change_commands(user_input))
    {
      continue;
    }
    if (handle_general_commands(user_input))
    {
      continue;
    }

    // Check if the input is a valid character.
    if (user_input.length() == 1 &&
        valid_chars.find(user_input) != std::string::npos)
    {
      char_input = user_input[0];
      return char_input;
    }
    printf("Invalid input. Please try again\n");
  }
  return ' ';
}

void ChessEngine::print_applied_moves()
{
  std::vector<parts::Move> previous_moves_temp;

  // Save all previous moves temporarily to print them.
  while (!game_board_state.previous_move_stack.empty())
  {
    parts::Move move = game_board_state.previous_move_stack.top();
    previous_moves_temp.push_back(move);
    game_board_state.previous_move_stack.pop();
  }

  // Print first move to last move applied to the board.
  for (int index = previous_moves_temp.size() - 1; index >= 0; --index)
  {
    parts::Move move = previous_moves_temp[index];
    printf("%s\n", parts::MoveInterface::move_to_string(move).c_str());
    // Add move back to previous move stack.
    game_board_state.previous_move_stack.push(move);
  }
}

void ChessEngine::reset_game()
{
  search_engine.clear_previous_move_evals();
  game_board_state.reset_board();
  exit_state = true;
}
} // namespace engine