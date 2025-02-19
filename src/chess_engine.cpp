#include "chess_engine.h"

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

    Move engine_move =
        search_engine.find_best_move(max_search_depth, show_performance);
    board_state.apply_move(engine_move);
    printf("eval: %d\n", position_evaluator.evaluate_position());
    board_state.print_board(player_color);

    if (is_checkmate()) {
      printf("Checkmate, You LOSE!\n");
      break;
    }
    if (is_stalemate()) {
      printf("Stalemate, It's a draw!\n");
      break;
    }
    Move user_move =
        move_interface.input_to_move(search_engine.calculate_possible_moves());
    board_state.apply_move(user_move);
    printf("eval: %d\n", position_evaluator.evaluate_position());
    board_state.print_board(player_color);
  }
}

// PUBLIC FUNCTIONS
ChessEngine::ChessEngine()
    : board_state(BoardState()), search_engine(BestMoveFinder(board_state)),
      move_interface(MoveInterface(board_state)),
      position_evaluator(PositionEvaluator(board_state)) {}

void ChessEngine::start_game() {
  char user_color;
  std::cout << "Please Enter Player Color (w = White, b = Black):";
  std::cin >> user_color;

  int engine_depth;
  std::cout << "Please Enter Engine Depth:";
  std::cin >> engine_depth;

  bool show_performance;
  std::cout << "Show Performance (1 = Yes, 0 = No):";
  std::cin >> show_performance;

  if (user_color == 'w') {
    player_color = PieceColor::WHITE;
    Move user_move =
        move_interface.input_to_move(search_engine.calculate_possible_moves());
    board_state.apply_move(user_move);
    printf("eval: %d\n", position_evaluator.evaluate_position());
    board_state.print_board(player_color);
    search_engine.engine_color = PieceColor::BLACK;
  } else {
    search_engine.engine_color = PieceColor::WHITE;
  }
  game_loop(engine_depth, show_performance);
}

bool ChessEngine::is_checkmate() {
  PieceColor current_color = board_state.move_color;
  // If the king is checked and all possible moves result in a checked king, it
  // is a checkmate.
  if (board_state.king_is_checked(current_color)) {
    std::vector<Move> possible_moves = search_engine.calculate_possible_moves();
    for (Move move : possible_moves) {
      board_state.apply_move(move);
      if (!board_state.king_is_checked(current_color)) {
        board_state.undo_move();
        return false;
      }
      board_state.undo_move();
    }
    return true;
  }
  return false;
}

bool ChessEngine::is_stalemate() {
  PieceColor current_color = board_state.move_color;
  // If the king is not checked and all possible moves result in a checked king,
  // it is a stalemate.
  if (!board_state.king_is_checked(current_color)) {
    std::vector<Move> possible_moves = search_engine.calculate_possible_moves();
    for (Move move : possible_moves) {
      board_state.apply_move(move);
      if (!board_state.king_is_checked(current_color)) {
        board_state.undo_move();
        return false;
      }
      board_state.undo_move();
    }
    return true;
  }
  return false;
}
