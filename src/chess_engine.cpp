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

  set_up_engine();

  // Print initial board state.
  game_board_state.print_board(player_color);

  while (!exit_state)
  {
    check_and_handle_if_game_over();

    if (!game_over && game_board_state.color_to_move != player_color)
    {
      // Engine's turn.
      if (!search_engine.execute_best_move())
      {
        printf("BREAKPOINT execute_best_move\n");
        exit(0);
      }
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
  if (!game_over && is_checkmate())
  {
    std::string winner =
        game_board_state.color_to_move == parts::PieceColor::WHITE ? "Black"
                                                                   : "White";
    printf("\nCheckmate, %s WINS!\n", winner.c_str());
    game_over = true;
  }
  if (!game_over && is_stalemate())
  {
    printf("\nStalemate, It's a draw!\n");
    game_over = true;
  }
}

void ChessEngine::set_up_engine()
{
  std::string user_message;

  // Get user input for engine settings.
  user_message = "Please Enter Engine Depth (1-30): ";
  int search_depth = getValidIntInput(user_message, 1, parts::MAX_SEARCH_DEPTH);
  search_engine.max_search_depth = search_depth;

  user_message = "Show Performance (y = Yes, n = No): ";
  char show_performance_char = getValidCharInput(user_message, "yn");
  search_engine.show_performance = show_performance_char == 'y';

  user_message = "Show All Move Evaluations (y = Yes, n = No): ";
  char show_move_evaluations_char = getValidCharInput(user_message, "yn");
  search_engine.show_move_evaluations = show_move_evaluations_char == 'y';

  user_message = "Please Enter Player Color (w = White, b = Black):";
  char user_color = getValidCharInput(user_message, "wb");

  // Set player and engine colors.
  if (user_color == 'w' &&
      game_board_state.color_to_move == parts::PieceColor::WHITE)
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
}

auto ChessEngine::handle_state_change_commands(const std::string &user_input)
    -> bool
{
  if (user_input == "exit")
  {
    printf("\n -- Goodbye G! --\n\n");
    exit(0);
  }
  else if (user_input == "menu")
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

auto ChessEngine::handle_board_undo_reset_commands(
    const std::string &user_input) -> bool
{
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

  // If it was game over and we undid the last move, it is no longer game over.
  if (game_over)
  {
    game_over = false;
  }
  return true;
}

auto ChessEngine::getValidIntInput(const std::string &user_message, int min,
                                   int max) -> int
{
  int int_input;
  std::string user_input;

  while (!exit_state)
  {
    printf("%s", user_message.c_str());

    // Get user input.
    std::cin >> user_input;

    if (handle_state_change_commands(user_input))
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
    printf("%s", user_message.c_str());

    // Get user input.
    std::cin >> user_input;

    if (handle_state_change_commands(user_input))
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

auto ChessEngine::is_checkmate() -> bool
{
  parts::PieceColor current_color = game_board_state.color_to_move;
  std::vector<parts::Move> possible_moves =
      parts::move_generator::calculate_possible_moves(game_board_state);

  for (parts::Move move : possible_moves)
  {
    game_board_state.apply_move(move);
    if (!game_board_state.king_is_checked(current_color))
    {
      game_board_state.undo_move();
      return false;
    }
    game_board_state.undo_move();
  }
  return true;
}

auto ChessEngine::is_stalemate() -> bool
{
  parts::PieceColor current_color = game_board_state.color_to_move;
  std::vector<parts::Move> possible_moves =
      parts::move_generator::calculate_possible_moves(game_board_state);

  for (parts::Move move : possible_moves)
  {
    game_board_state.apply_move(move);
    if (!game_board_state.king_is_checked(current_color))
    {
      game_board_state.undo_move();
      return false;
    }
    game_board_state.undo_move();
  }
  return true;
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