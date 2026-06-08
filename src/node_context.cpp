#include "node_context.h"
#include "engine_constants.h"

namespace engine::parts
{
auto new_context(BoardState &board_state,
                 int alpha,
                 int beta,
                 int depth,
                 bool is_forward_pruning_line,
                 bool is_pvs_line,
                 int ply,
                 int thread_index,
                 bool is_quiescence) -> NodeContext
{
  return NodeContext{
      board_state,
      alpha,
      beta,
      depth,
      is_forward_pruning_line,
      is_pvs_line,
      ply,
      thread_index,
      alpha,
      0,
      0,
      -INF,
      {},
      is_quiescence,
      0,
      0,
      0,
      -1,
      board_state.get_current_state_hash(),
      false,
  };
}
} // namespace engine::parts
