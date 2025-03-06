#include "chess_engine.h"

auto main(int argc, char **argv) -> int
{
  engine::ChessEngine chess_engine = engine::ChessEngine();

  chess_engine.state_machine();
}
