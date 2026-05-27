#include "uci_engine.h"
#include "fen_interface.h"
#include "move_interface.h"

#include <iostream>

namespace engine::uci_engine
{
// CONSTRUCTORS

UCIEngine::UCIEngine() : search_engine(game_board_state, true) {}

// PUBLIC FUNCTIONS

void UCIEngine::start_engine()
{
  exit_flag = false;
  handle_input_thread = std::thread([this]() { handle_input(); });

  std::setvbuf(stdout, nullptr, _IONBF, 0);
  std::string user_input;
  while (!exit_flag)
  {
    std::getline(std::cin, user_input);

    {
      std::lock_guard<std::mutex> lock(input_queue_mutex);
      input_queue.push(user_input);
    }

    std::string token = read_token(user_input);
    if (token == QUIT_COMMAND)
    {
      handle_quit_command();
    }
    else if (token == STOP_COMMAND)
    {
      handle_stop_command();
    }

    input_queue_cv.notify_one();
  }

  handle_input_thread.join();
  exit(0);
}

// PRIVATE FUNCTIONS

void UCIEngine::handle_input()
{
  while (!exit_flag)
  {
    std::string user_input;
    {
      std::unique_lock<std::mutex> lock(input_queue_mutex);
      input_queue_cv.wait(lock, [this]() { return !input_queue.empty(); });
      user_input = input_queue.front();
      input_queue.pop();
    }

    std::string token = read_token(user_input);
    if (token == UCI_COMMAND)
    {
      handle_uci_command();
    }
    else if (token == ISREADY_COMMAND)
    {
      handle_isready_command();
    }
    else if (token == UCINEWGAME_COMMAND)
    {
      handle_ucinewgame_command();
    }
    else if (token == POSITION_COMMAND)
    {
      handle_position_command(user_input);
    }
    else if (token == GO_COMMAND)
    {
      handle_go_command(user_input);
    }
    else if (token == STOP_COMMAND)
    {
      handle_stop_command();
    }
    else if (token == QUIT_COMMAND)
    {
      handle_quit_command();
    }
  }
}

auto UCIEngine::skip_whitespace(std::string &string) -> void
{
  string.erase(0, string.find_first_not_of(" \t\n\r\f\v"));
}

auto UCIEngine::read_token(std::string &user_input) -> std::string
{
  skip_whitespace(user_input);

  if (user_input.empty())
  {
    return "";
  }
  size_t index = user_input.find_first_of(" \t\r\n");

  if (index == std::string::npos)
  {
    // Indicates the end of the string.
    index = user_input.size();
  }

  std::string token = user_input.substr(0, index);
  user_input = user_input.substr(index);
  return token;
}

void UCIEngine::handle_uci_command()
{
  printf("id name Elby-Engine\n");
  printf("id author Elbert Alcantara\n");
  printf("uciok\n");
}

void UCIEngine::handle_isready_command() { printf("readyok\n"); }

void UCIEngine::handle_ucinewgame_command()
{
  // NOTE: GUI sends this when the next position is from a different game than
  // the last position.
  // TODO: Potentially reset some search engine properties here.
}

void UCIEngine::handle_position_command(std::string &user_input)
{
  search_engine.stop_engine_search();
  search_engine.stop_engine_pondering();
  game_board_state.reset_board();

  std::string token = read_token(user_input);
  if (token == FEN_COMMAND)
  {
    std::string fen_configuration;
    for (int index = 0; index < parts::fen_interface::FEN_FIELD_COUNT; index++)
    {
      fen_configuration += read_token(user_input);
      if (index < parts::fen_interface::FEN_FIELD_COUNT - 1)
      {
        fen_configuration += " ";
      }
    }
    parts::fen_interface::setup_custom_board(game_board_state,
                                             fen_configuration);
  }

  token = read_token(user_input);
  if (token == MOVE_COMMAND)
  {
    while (!user_input.empty())
    {
      std::string move_string = read_token(user_input);
      parts::Move move =
          parts::Move(-1, -1, -1, -1, nullptr, nullptr, parts::PieceType::EMPTY,
                      false, false, false, -1, -1);

      if (parts::move_interface::string_to_move(move, move_string,
                                                game_board_state) &&
          parts::move_interface::validate_move(move, game_board_state))
      {
        game_board_state.apply_move(move);
      }
    }
  }
  // Set engine color to who's move it is now
  search_engine.engine_color = game_board_state.color_to_move;
}

void UCIEngine::handle_go_command(std::string &user_input)
{
  search_engine.stop_engine_search();
  search_engine.stop_engine_pondering();

  int wtime_ms = -1;
  int btime_ms = -1;
  int winc_ms = -1;
  int binc_ms = -1;
  int movetime_ms = -1;
  int movestogo = -1;
  int depth = -1;
  bool infinite = false;

  while (!user_input.empty())
  {
    std::string token = read_token(user_input);

    if (token == WTIME_COMMAND)
    {
      std::string time_string = read_token(user_input);
      wtime_ms = std::stoi(time_string);
    }
    else if (token == BTIME_COMMAND)
    {
      std::string time_string = read_token(user_input);
      btime_ms = std::stoi(time_string);
    }
    else if (token == WINC_COMMAND)
    {
      std::string time_string = read_token(user_input);
      winc_ms = std::stoi(time_string);
    }
    else if (token == BINC_COMMAND)
    {
      std::string time_string = read_token(user_input);
      binc_ms = std::stoi(time_string);
    }
    else if (token == MOVESTOGO_COMMAND)
    {
      std::string movestogo_string = read_token(user_input);
      movestogo = std::stoi(movestogo_string);
    }
    else if (token == DEPTH_COMMAND)
    {
      std::string depth_string = read_token(user_input);
      depth = std::stoi(depth_string);
    }
    else if (token == MOVETIME_COMMAND)
    {
      std::string movetime_string = read_token(user_input);
      movetime_ms = std::stoi(movetime_string);
    }
    else if (token == INFINITE_COMMAND)
    {
      infinite = true;
    }
  }

  search_for_best_move(wtime_ms, btime_ms, winc_ms, binc_ms, movestogo, depth,
                       movetime_ms, infinite);
}

void UCIEngine::handle_stop_command()
{
  search_engine.stop_engine_search();
  search_engine.start_engine_pondering();
}

void UCIEngine::handle_quit_command()
{
  exit_flag = true;
  search_engine.stop_engine_search();
  search_engine.stop_engine_pondering();
}

// SEARCH FUNCTIONS

void UCIEngine::search_for_best_move(int wtime_ms,
                                     int btime_ms,
                                     int winc_ms,
                                     int binc_ms,
                                     int movestogo,
                                     int depth,
                                     int movetime_ms,
                                     bool infinite)
{
  // Deterine Engine Clock depending on engine color
  int engine_clock = 0;
  int engine_increment = 0;

  // Set default search time and depth
  search_engine.max_search_time_milliseconds = parts::DEFAULT_SEARCH_TIME_MS;
  search_engine.max_search_depth = parts::MAX_SEARCH_DEPTH;

  if (search_engine.engine_color == parts::PieceColor::WHITE)
  {
    engine_clock = wtime_ms;
    engine_increment = winc_ms;
  }
  else
  {
    engine_clock = btime_ms;
    engine_increment = binc_ms;
  }

  if (depth > 0)
  {
    search_engine.max_search_depth = depth;
  }
  if (engine_clock > 0)
  {
    // First two moves
    if (game_board_state.previous_move_stack.size() < 2)
    {
      //  2 seconds or wtime_ms / 30, whichever is smaller
      search_engine.max_search_time_milliseconds =
          std::min(engine_clock / OPENING_MOVE_STACK_TIME_FACTOR,
                   parts::DEFAULT_SEARCH_TIME_MS);
    }
    // Opening
    else if (game_board_state.previous_move_stack.size() <
             OPENING_MOVE_STACK_SIZE)
    {
      search_engine.max_search_time_milliseconds =
          (engine_clock / OPENING_MOVE_STACK_TIME_FACTOR);
    }
    // End game
    else if (game_board_state.is_end_game)
    {
      search_engine.max_search_time_milliseconds =
          (engine_clock / END_GAME_MOVE_STACK_TIME_FACTOR);
    }
    // Middle game
    else
    {
      search_engine.max_search_time_milliseconds =
          (engine_clock / MIDDLE_GAME_MOVE_STACK_TIME_FACTOR);
    }
  }
  if (winc_ms > 0)
  {
    // NOTE: -1 is to account for the time it takes for the move to actually be
    // played in the server.
    search_engine.max_search_time_milliseconds += engine_increment - 1;
  }
  if (movetime_ms > 0)
  {
    search_engine.max_search_time_milliseconds = movetime_ms;
  }
  if (infinite)
  {
    search_engine.max_search_time_milliseconds = parts::INF;
    search_engine.max_search_depth = parts::MAX_SEARCH_DEPTH;
  }

  search_engine.start_engine_search();
  std::string best_move = search_engine.wait_for_search_and_get_best_move();
  printf("bestmove %s\n", best_move.c_str());
  search_engine.start_engine_pondering();
}
} // namespace engine::uci_engine