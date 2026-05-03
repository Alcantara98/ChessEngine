#ifndef UCI_H
#define UCI_H

#include "board_state.h"
#include "cross_plat_functions.h"
#include "fen_interface.h"
#include "move_interface.h"
#include "search_engine.h"

#include <functional>
#include <sstream>

namespace uci
{
/**
 * @brief Class to handle the UCI interface.
 */
class UCI
{
public:
  // CONSTRUCTORS

  /**
   * @brief Default Constructor - Initializes the UCI.
   */
  UCI();
};

} // namespace uci

#endif // UCI_H