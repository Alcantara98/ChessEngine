#include "chess_engine.h"

auto main(int argc, char **argv) -> int {
  ChessEngine chess_engine = ChessEngine();

  chess_engine.start_game();
}
