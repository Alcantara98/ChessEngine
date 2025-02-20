#ifndef TRANSPOSITION_TABLE_H
#define TRANSPOSITION_TABLE_H

#include "board_state.h"
#include <list>
#include <unordered_map>

/**
 * @brief Entry in the transposition table.
 */
struct TranspositionTableEntry {
  // Maximum depth of the search.
  int max_depth;

  // Value of the board state.
  int eval_score;

  // Flag of the value. 0 = exact, 1 = lower bound, 2 = upper bound.
  int flag;

  // Least recently used position in the LRU list.
  std::list<uint64_t>::iterator lru_position;

  // Index of the best move in the board state.
  int best_move_index;
};

class TranspositionTable {
public:
  /**
   * @brief Construct a new Transposition Table object
   */
  TranspositionTable(uint64_t max_size);

  /**
   * @brief Store a new entry in the transposition table.
   * @param board_state Board state to store.
   * @param max_depth Maximum depth of the search.
   * @param eval_score Evaluation score of the board state.
   * @param flag Flag of the value.
   */
  void store(uint64_t &hash, int max_depth, int eval_score, int flag,
             int best_move_index);

  /**
   * @brief Retrieve an entry from the transposition table.
   * @param board_state Board state to retrieve.
   * @param max_depth Maximum depth of the search.
   * @param eval_score Evaluation score of the board state.
   * @param flag Flag of the value.
   * @return true if the entry was found, false otherwise.
   */
  bool retrieve(uint64_t &hash, int &depth, int &eval_score, int &flag,
                int &best_move_index);
  /**
   * @brief Get the size of the transposition table.
   * @return int The size of the transposition table.
   */
  int get_size();

  /**
   * @brief Clear the transposition table.
   */
  void clear();

private:
  // Maximum size of the transposition table.
  uint64_t max_size;

  // Hash table to store entries.
  std::unordered_map<uint64_t, TranspositionTableEntry> table;

  // List of hash values in least recently used order.
  std::list<uint64_t> lru_list;

  /**
   * @brief Remove the least recently used entry from the table.
   */
  void trim();
};

#endif // TRANSPOSITION_TABLE_H