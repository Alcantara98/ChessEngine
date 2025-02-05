#include "move_interface.h"

Move MoveInterface::input_to_move() {
  bool move_is_valid = false;

  while (!move_is_valid) {
    std::string move;
    std::cin >> move;
    std::regex moveRegex(
        R"(^((O-O(?:-O)?)|
        ([KQRBN])
        ([a-h][1-8])
        (x?)
        ([a-h][1-8])
        =?([QRBN])?
        ([+#]?)
        )$)",
        std::regex::extended);

    std::smatch matches;

    if (std::regex_match(move, matches, moveRegex)) {
      if (matches[1].matched) {}

    }
  }
  return Move();
}