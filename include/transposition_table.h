#ifndef TRANSPOSITION_TABLE_H
#define TRANSPOSITION_TABLE_H

#include "board_state.h"
#include <list>
#include <unordered_map>

struct TranspositionTableEntry {
  int depth;
  int value;
  int flag;                                 // 0 = exact, -1 = alpha, 1 = beta
  std::list<size_t>::iterator lru_position; // Position in the LRU list
};

class TranspositionTable {
private:
  // Maximum size of the transposition table.
  size_t max_size;

  // Hash table to store entries.
  std::unordered_map<size_t, TranspositionTableEntry> table;

  // List of hash values in least recently used order.
  std::list<size_t> lru_list;

  void trim();

public:
  /**
   * @brief Construct a new Transposition Table object
   */
  TranspositionTable(size_t max_size) : max_size(max_size) {}

  /**
   * @brief Store a new entry in the transposition table.
   * @param board_state Board state to store.
   * @param depth Depth of the search.
   * @param value Value of the board state.
   * @param flag Flag of the value.
   */
  void store(const BoardState &board_state, int depth, int value, int flag);

  /**
   * @brief Retrieve an entry from the transposition table.
   * @param board_state Board state to retrieve.
   * @param depth Depth of the search.
   * @param value Value of the board state.
   * @param flag Flag of the value.
   * @return true if the entry was found, false otherwise.
   */
  bool retrieve(const BoardState &board_state, int depth, int &value,
                int &flag);
};

#endif // TRANSPOSITION_TABLE_H