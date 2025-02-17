#ifndef TRANSPOSITION_TABLE_H
#define TRANSPOSITION_TABLE_H

#include "board_state.h"
#include <list>
#include <unordered_map>

struct TranspositionTableEntry {
  int depth;
  int value;
  int flag;                                   // 0 = exact, -1 = alpha, 1 = beta
  std::list<uint64_t>::iterator lru_position; // Position in the LRU list
  int best_move_index;
};

class TranspositionTable {
private:
  // Maximum size of the transposition table.
  uint64_t max_size;

  // Hash table to store entries.
  std::unordered_map<uint64_t, TranspositionTableEntry> table;

  // List of hash values in least recently used order.
  std::list<uint64_t> lru_list;

  void trim();

public:
  /**
   * @brief Construct a new Transposition Table object
   */
  TranspositionTable(uint64_t max_size) : max_size(max_size) {}

  /**
   * @brief Store a new entry in the transposition table.
   * @param board_state Board state to store.
   * @param depth Depth of the search.
   * @param value Value of the board state.
   * @param flag Flag of the value.
   */
  void store(uint64_t &hash, int depth, int value, int flag,
             int best_move_index);

  /**
   * @brief Retrieve an entry from the transposition table.
   * @param board_state Board state to retrieve.
   * @param depth Depth of the search.
   * @param value Value of the board state.
   * @param flag Flag of the value.
   * @return true if the entry was found, false otherwise.
   */
  bool retrieve(uint64_t &hash, int &depth, int &value,
                int &flag, int &best_move_index);
};

#endif // TRANSPOSITION_TABLE_H