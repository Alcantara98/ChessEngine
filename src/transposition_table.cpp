#include "transposition_table.h"

void TranspositionTable::store(uint64_t &hash, int depth, int value, int flag,
                               int best_move_index) {
  auto it = table.find(hash);

  if (it != table.end()) {
    // Update existing entry
    it->second.depth = depth;
    it->second.value = value;
    it->second.flag = flag;
    it->second.best_move_index = best_move_index;
    lru_list.erase(it->second.lru_position);
  } else {
    // Insert new entry
    if (table.size() >= max_size) {
      trim();
    }
    table[hash] = {depth, value, flag, lru_list.end(), best_move_index};
  }

  // Update LRU list
  lru_list.push_front(hash);
  table[hash].lru_position = lru_list.begin();
}

bool TranspositionTable::retrieve(uint64_t &hash, int &depth, int &value,
                                  int &flag, int &best_move_index) {
  auto it = table.find(hash);

  if (it != table.end()) {
    value = it->second.value;
    flag = it->second.flag;
    depth = it->second.depth;
    best_move_index = it->second.best_move_index;

    // Update LRU list
    lru_list.erase(it->second.lru_position);
    lru_list.push_front(hash);
    it->second.lru_position = lru_list.begin();

    return true;
  }
  return false;
}

void TranspositionTable::trim() {
  // Remove the least recently used entry
  uint64_t lru_hash = lru_list.back();
  lru_list.pop_back();
  table.erase(lru_hash);
}

int TranspositionTable::get_size() { return table.size(); }

void TranspositionTable::clear() {
  table.clear();
  lru_list.clear();
}