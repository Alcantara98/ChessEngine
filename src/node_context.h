#ifndef NODE_CONTEXT_H
#define NODE_CONTEXT_H

#include "board_state.h"
#include "move.h"

#include <vector>

namespace engine::parts
{

struct NodeContext
{
  // SEARCH TREE PROPERTIES
  BoardState &board_state;
  int alpha;
  int beta;
  int depth;
  bool is_forward_pruning_line;
  bool is_pvs_line;
  int ply;
  int thread_index;

  // EVALUATION PROPERTIES
  int original_alpha;
  int eval;
  int static_eval;
  int max_eval;
  std::vector<Move> possible_moves;
  bool is_quiescence;

  // TRANSPOSITION TABLE PROPERTIES
  int tt_eval;
  int tt_flag;
  int tt_entry_search_depth;
  int tt_best_move_index;
  uint64_t hash;

  // CHECK PROPERTIES
  bool color_to_move_is_in_check;
};

/**
 * @brief Create a node context from another node context.
 *
 * @param board_state BoardState object to search.
 * @param alpha Highest score to be picked by maximizing node.
 * @param beta Lowest score to be picked by minimizing node.
 * @param depth Current depth of search.
 * @param is_forward_pruning_line Flag to indicate if the search line is from
 * a null move, late move reduction, or probability cut line.
 * @param is_pvs_line Flag to indicate if the node is a PVS node.
 * @param ply Current ply of the search.
 * @param thread_index Thread index of the search thread.
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
                 bool is_quiescence = false) -> NodeContext;
} // namespace engine::parts

#endif // NODE_CONTEXT_H
