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
  current_state = new_state;
  exit_state = true;
}

void ChessEngine::main_menu_state()
{
  while (!exit_state)
  {
    std::string play_engine;
    std::cout << "Play Against Engine (y = Yes, n = No):";
    std::cin >> play_engine;
    if (play_engine == "exit")
    {
      exit(0);
    }
    else if (play_engine == "y")
    {
      change_state(&ChessEngine::engine_vs_player_state);
    }
    else if (play_engine == "n")
    {
      change_state(&ChessEngine::player_vs_player_state);
    }
    else
    {
      printf("Invalid Input\n");
    }
  }
}

void ChessEngine::player_vs_player_state()
{
  while (!exit_state)
  {
    game_board_state.print_board(game_board_state.color_to_move);
    if (!game_over && is_checkmate())
    {
      printf("Checkmate, %s WINS!\n",
             game_board_state.color_to_move == player_color ? "Engine"
                                                            : "Player");
      game_over = true;
    }
    if (!game_over && is_stalemate())
    {
      printf("Stalemate, It's a draw!\n");
      game_over = true;
    }

    handle_player_turn();
  }
}

void ChessEngine::engine_vs_player_state()
{
  set_up_engine();

  while (!exit_state)
  {
    if (!game_over && is_checkmate())
    {
      printf("Checkmate, %s WINS!\n",
             game_board_state.color_to_move == player_color ? "Engine"
                                                            : "Player");
      game_over = true;
    }
    if (!game_over && is_stalemate())
    {
      printf("Stalemate, It's a draw!\n");
      game_over = true;
    }
    if (!game_over && game_board_state.color_to_move != player_color)
    {
      // Engine's turn.
      if (!search_engine.execute_best_move())
      {
        printf("Break-Point execute_best_move\n");
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

void ChessEngine::set_up_engine()
{
  int engine_depth;
  do
  {
    std::cout << "Please Enter Engine Depth (1-30):";
    std::cin >> engine_depth;
  } while (engine_depth < 1 && engine_depth >= parts::MAX_SEARCH_DEPTH);
  search_engine.max_search_depth = engine_depth;

  char show_performance_char;
  do
  {
    std::cout << "Show Performance (y = Yes, n = No):";
    std::cin >> show_performance_char;
  } while (show_performance_char != 'y' && show_performance_char != 'n');
  search_engine.show_performance = show_performance_char == 'y';

  char user_color;
  do
  {
    std::cout << "Please Enter Player Color (w = White, b = Black):";
    std::cin >> user_color;
  } while (user_color != 'w' && user_color != 'b');

  if (user_color == 'w' &&
      game_board_state.color_to_move == parts::PieceColor::WHITE)
  {
    // Set player colors.
    player_color = parts::PieceColor::WHITE;
    search_engine.engine_color = parts::PieceColor::BLACK;

    // White goes first, player's turn.
    game_board_state.print_board(player_color);
    handle_player_turn();
  }
  else
  {
    player_color = parts::PieceColor::BLACK;
    search_engine.engine_color = parts::PieceColor::WHITE;
  }
}

void ChessEngine::handle_player_turn()
{
  while (!exit_state)
  {
    // Indicate color to move.
    printf("%s's Turn\n",
           game_board_state.color_to_move == parts::PieceColor::WHITE
               ? "White"
               : "Black");

    // Get user input.
    std::string user_input;
    std::cout << "Enter move: ";
    std::cin >> user_input;
    std::cout << '\n';

    if (user_input == "exit")
    {
      exit(0);
    }
    else if (user_input == "undo")
    {
      game_board_state.undo_move();
      if (current_state == &ChessEngine::engine_vs_player_state)
      {
        game_board_state.undo_move();
      }
      game_board_state.print_board(game_board_state.color_to_move);
      if (game_over)
      {
        game_over = false;
      }
      continue;
    }
    else if (user_input == "menu")
    {
      game_board_state.reset_board();
      change_state(&ChessEngine::main_menu_state);
    }
    else if (user_input == "reset")
    {
      game_board_state.reset_board();
      exit_state = true;
    }
    else if (user_input == "play-engine")
    {
      game_board_state.reset_board();
      change_state(&ChessEngine::engine_vs_player_state);
    }
    else if (user_input == "play-player")
    {
      game_board_state.reset_board();
      change_state(&ChessEngine::player_vs_player_state);
    }
    else if (game_over)
    {
      printf("Game Over - Options:\n - menu\n - exit\n - undo\n - reset\n - "
             "play-engine\n - "
             "play-player\n");
    }
    else if (user_input == "help")
    {
      printf(
          "Options:\n - menu\n - exit\n - undo\n - reset\n - play-engine\n - "
          "play-player\n");
    }
    else if (move_interface.input_to_move(
                 parts::move_generator::calculate_possible_moves(
                     game_board_state),
                 user_input))
    {
      // Move was valid, end player's turn.
      break;
    }
  }
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
} // namespace engine