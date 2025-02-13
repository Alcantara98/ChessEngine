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
    Move engine_move = search_engine.find_best_move(engine_depth);
    board_state.apply_move(engine_move);
    printf("eval: %d\n", position_evaluator.evaluate_position());
    board_state.print_board();

    Move user_move =
        move_interface.input_to_move(search_engine.calculate_possible_moves());
    board_state.apply_move(user_move);
    printf("eval: %d\n", position_evaluator.evaluate_position());
    board_state.print_board();
  }
}
