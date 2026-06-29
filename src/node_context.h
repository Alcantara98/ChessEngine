#ifndef NODE_CONTEXT_H
#define NODE_CONTEXT_H

#include "board_state.h"

namespace engine::parts
{

struct NodeContext
{
  // REQUIRED
  BoardState &board_state;
  int alpha;
  int beta;
  int depth;
  bool is_forward_pruning_line;
  bool is_pvs_line;
  int ply;
  int thread_index;
  int original_alpha;
  bool previous_state_in_check;
  int iteration_depth;
  bool is_quiescence;
  uint64_t hash;

  // DEFAULTS
  int eval = 0;
  int static_eval = 0;
  int max_eval = 0;
  int tt_eval = 0;
  int tt_flag = 0;
  int tt_entry_search_depth = 0;
  int tt_best_move_index = -1;
  bool king_in_check = false; // TODO: Move this to board state.
};

/**
 * @brief Create a node context from another node context.
 *
 * @param board_state BoardState object to search.
 * @param alpha Highest score to be picked by maximizing node.
 * @param beta Lowest score to be picked by minimizing node.
 * @param depth Current depth of search.
 * @param is_forward_pruning_line Flag to indicate if the search line is from
 * a null move, late move reduction.
 * @param is_pvs_line Flag to indicate if the node is a PVS node.
 * @param ply Current ply of the search.
 * @param thread_index Thread index of the search thread.
 * @param previous_state_in_check Whether previous state was in check.
 * @param iteration_depth Current iteration depth of search.
 * @param is_quiescence Flag whether node is a quiesence node.
 *
 * @return The created node context.
 */
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
                 bool is_quiescence = false) -> NodeContext;
} // namespace engine::parts

#endif // NODE_CONTEXT_H
