#include "transposition_table.h"

// CONSTRUCTORS
TranspositionTable::TranspositionTable(uint64_t max_size)
    : max_size(max_size) {}

// PUBLIC FUNCTIONS
void TranspositionTable::store(uint64_t &hash, int max_depth, int eval_score,
                               int flag, int best_move_index) {
  auto it = table.find(hash);

  if (it != table.end()) {
    // Update existing entry
    it->second.max_depth = max_depth;
    it->second.eval_score = eval_score;
    it->second.flag = flag;
    it->second.best_move_index = best_move_index;
    lru_list.erase(it->second.lru_position);
  } else {
    // Insert new entry
    if (table.size() >= max_size) {
      trim();
    }
    table[hash] = {max_depth, eval_score, flag, lru_list.end(),
                   best_move_index};
  }

  // Update LRU list
  lru_list.push_front(hash);
  table[hash].lru_position = lru_list.begin();
}

bool TranspositionTable::retrieve(uint64_t &hash, int &max_depth,
                                  int &eval_score, int &flag,
                                  int &best_move_index) {
  auto it = table.find(hash);

  if (it != table.end()) {
    eval_score = it->second.eval_score;
    flag = it->second.flag;
    max_depth = it->second.max_depth;
    best_move_index = it->second.best_move_index;

    // Update LRU list
    lru_list.erase(it->second.lru_position);
    lru_list.push_front(hash);
    it->second.lru_position = lru_list.begin();

    return true;
  }
  return false;
}

int TranspositionTable::get_size() { return table.size(); }

void TranspositionTable::clear() {
  table.clear();
  lru_list.clear();
}

// PRIVATE FUNCTIONS
void TranspositionTable::trim() {
  // Remove the least recently used entry
  uint64_t lru_hash = lru_list.back();
  lru_list.pop_back();
  table.erase(lru_hash);
}
