#include "chess_engine.h"

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

  if (user_color == 'w') {
    Move user_move =
        move_interface.input_to_move(search_engine.calculate_possible_moves());
    board_state.apply_move(user_move);
    printf("eval: %d\n", position_evaluator.evaluate_position());
    board_state.print_board();
    search_engine.engine_color = PieceColor::BLACK;
  }

  while (true) {
    if (is_checkmate()) {
      printf("Checkmate, You WIN!\n");
      break;
    }
    if (is_stalemate()) {
      printf("Stalemate, It's a draw!\n");
      break;
    }

    Move engine_move = search_engine.find_best_move(engine_depth);
    board_state.apply_move(engine_move);
    printf("eval: %d\n", position_evaluator.evaluate_position());
    board_state.print_board();

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
    board_state.print_board();
  }
}

bool ChessEngine::is_checkmate() {
  PieceColor current_color = board_state.move_color;
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
