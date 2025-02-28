#include "chess_engine.h"

// CONSTRUCTORS
ChessEngine::ChessEngine()
    : game_board_state(BoardState()),
      search_engine(SearchEngine(game_board_state)),
      move_interface(MoveInterface(game_board_state)),
      position_evaluator(PositionEvaluator()) {}

// PUBLIC FUNCTIONS
void ChessEngine::start_game() {
  char user_color;
  std::cout << "Please Enter Player Color (w = White, b = Black):";
  std::cin >> user_color;

  int engine_depth;
  std::cout << "Please Enter Engine Depth:";
  std::cin >> engine_depth;

  bool show_performance;
  char show_performance_char;
  std::cout << "Show Performance (y = Yes, n = No):";
  std::cin >> show_performance_char;

  if (show_performance_char == 'y') {
    show_performance = true;
  } else {
    show_performance = false;
  }

  if (user_color == 'w' &&
      game_board_state.color_to_move == PieceColor::WHITE) {
    player_color = PieceColor::WHITE;
    Move user_move = move_interface.input_to_move(
        search_engine.calculate_possible_moves(game_board_state));
    game_board_state.apply_move(user_move);
    printf("eval: %d\n",
           position_evaluator.evaluate_position(game_board_state));
    game_board_state.print_board(player_color);
    search_engine.engine_color = PieceColor::BLACK;
  } else {
    search_engine.engine_color = PieceColor::WHITE;
  }
  game_loop(engine_depth, show_performance);
}

// PRIVATE FUNCTIONS
void ChessEngine::game_loop(int max_search_depth, bool show_performance) {
  while (true) {
    if (is_checkmate()) {
      printf("Checkmate, You WIN!\n");
      break;
    }
    if (is_stalemate()) {
      printf("Stalemate, It's a draw!\n");
      break;
    }

    // Engine's turn.
    if (!search_engine.execute_best_move(max_search_depth, show_performance)) {
      printf("Checkmate, You WIN!\n");
      break;
    }
    game_board_state.print_board(player_color);

    if (is_checkmate()) {
      printf("Checkmate, You LOSE!\n");
      break;
    }
    if (is_stalemate()) {
      printf("Stalemate, It's a draw!\n");
      break;
    }

    // Player's turn.
    Move user_move = move_interface.input_to_move(
        search_engine.calculate_possible_moves(game_board_state));
    game_board_state.apply_move(user_move);
    game_board_state.print_board(player_color);
  }
}

auto ChessEngine::is_checkmate() -> bool {
  PieceColor current_color = game_board_state.color_to_move;
  // If the king is checked and all possible moves result in a checked king,
  // it is a checkmate.
  if (game_board_state.king_is_checked(current_color)) {
    std::vector<Move> possible_moves =
        search_engine.calculate_possible_moves(game_board_state);
    for (Move move : possible_moves) {
      game_board_state.apply_move(move);
      if (!game_board_state.king_is_checked(current_color)) {
        game_board_state.undo_move();
        return false;
      }
      game_board_state.undo_move();
    }
    return true;
  }
  return false;
}

auto ChessEngine::is_stalemate() -> bool {
  PieceColor current_color = game_board_state.color_to_move;
  // If the king is not checked and all possible moves result in a checked
  // king, it is a stalemate.
  if (!game_board_state.king_is_checked(current_color)) {
    std::vector<Move> possible_moves =
        search_engine.calculate_possible_moves(game_board_state);
    for (Move move : possible_moves) {
      game_board_state.apply_move(move);
      if (!game_board_state.king_is_checked(current_color)) {
        game_board_state.undo_move();
        return false;
      }
      game_board_state.undo_move();
    }
    return true;
  }
  return false;
}
