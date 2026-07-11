#include "logger.h"
#include "uci_engine.h"

auto main(int argc, char **argv) -> int
{
  engine::init_logging("liying_engine_uci");

  engine::uci_engine::UCIEngine chess_engine_uci =
      engine::uci_engine::UCIEngine();

  chess_engine_uci.start_engine();
}
