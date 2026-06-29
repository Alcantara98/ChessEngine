#include "node_context.h"

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
                 bool previous_state_in_check,
                 int iteration_depth,
                 bool is_quiescence) -> NodeContext
{
  return NodeContext{board_state,
                     alpha,
                     beta,
                     depth,
                     is_forward_pruning_line,
                     is_pvs_line,
                     ply,
                     thread_index,
                     alpha,
                     previous_state_in_check,
                     iteration_depth,
                     is_quiescence,
                     board_state.get_current_state_hash()};
}
} // namespace engine::parts
