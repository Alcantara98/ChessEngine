#ifndef TRANSPOSITION_TABLE_H
#define TRANSPOSITION_TABLE_H

#include "board_state.h"
#include <list>
#include <mutex>
#include <shared_mutex>
#include <unordered_map>

namespace engine::parts
{
/**
 * @brief Entry in the transposition table.
 */
struct TranspositionTableEntry
{
  // PROPERTIES

  // Hash of the board state.
  uint64_t hash;

  // Maximum depth of the search.
  int search_depth;

  // Value of the board state.
  int eval_score;

  // Flag of the value. 0 = exact, 1 = lower bound, 2 = upper bound.
  int flag;

  // Index of the best move in the board state.
  int best_move_index;
};

class TranspositionTable
{
public:
  // CONSTRUCTORS

  /**
   * @brief Construct a new Transposition Table object
   */
  TranspositionTable(uint64_t max_size);

  ~TranspositionTable();

  // FUNCTIONS

  /**
   * @brief Store a new entry in the transposition table.
   *
   * @param hash Hash of the board state.
   * @param search_depth Depth searched for this position.
   * @param eval_score Evaluation score of the board state.
   * @param flag Flag of the value.
   * @param best_move_index Index of the best move in the board state.
   */
  void store(uint64_t &hash, int search_depth, int eval_score, int flag,
             int best_move_index);

  /**
   * @brief Retrieve an entry from the transposition table.
   *
   * @param hash Hash of the board state.
   * @param search_depth Depth searched for this position.
   * @param eval_score Evaluation score of the board state.
   * @param flag Flag of the value.
   * @param best_move_index Index of the best move in the board state.
   *
   * @return true if the entry was found, false otherwise.
   */
  auto retrieve(uint64_t &hash, int &search_depth, int &eval_score, int &flag,
                int &best_move_index) -> bool;

  /**
   * @brief Clear the transposition table.
   */
  void clear();

private:
  // PROPERTIES

  // Size of the transposition table.
  uint64_t max_size;

  // Transposition table represented as an array.
  TranspositionTableEntry *tt_table;
};
} // namespace engine::parts

#endif // TRANSPOSITION_TABLE_H