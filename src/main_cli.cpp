#include "chess_engine.h"
#include "logger.h"

auto main(int argc, char **argv) -> int
{
  engine::init_logging("liying_engine_cli");

  engine::ChessEngine chess_engine = engine::ChessEngine();

  printf("\n -- Welcome to Elby's Engine!!! -- \n");
  chess_engine.state_machine();

  return 0;
}
