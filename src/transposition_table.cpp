#include "transposition_table.h"

namespace engine::parts
{
// CONSTRUCTORS
TranspositionTable::TranspositionTable(uint64_t max_size) : max_size(max_size)
{
}

// PUBLIC FUNCTIONS
void TranspositionTable::store(uint64_t &hash, int search_depth, int eval_score,
                               int flag, int best_move_index)
{
  // Lock the table.
  std::lock_guard<std::mutex> lock(mutex);
  auto hash_value_iterator = table.find(hash);

  if (hash_value_iterator != table.end())
  {
    // Update existing entry
    hash_value_iterator->second.search_depth = search_depth;
    hash_value_iterator->second.eval_score = eval_score;
    hash_value_iterator->second.flag = flag;
    hash_value_iterator->second.best_move_index = best_move_index;
    lru_list.erase(hash_value_iterator->second.lru_position);
  }
  else
  {
    // Insert new entry
    if (table.size() >= max_size)
    {
      trim();
    }
    table[hash] = {search_depth, eval_score, flag, lru_list.end(),
                   best_move_index};
  }

  // Update LRU list
  lru_list.push_front(hash);
  table[hash].lru_position = lru_list.begin();
}

auto TranspositionTable::retrieve(uint64_t &hash, int &search_depth,
                                  int &eval_score, int &flag,
                                  int &best_move_index) -> bool
{
  // Lock the table.
  std::lock_guard<std::mutex> lock(mutex);
  auto hash_value_iterator = table.find(hash);

  if (hash_value_iterator != table.end())
  {
    eval_score = hash_value_iterator->second.eval_score;
    flag = hash_value_iterator->second.flag;
    search_depth = hash_value_iterator->second.search_depth;
    best_move_index = hash_value_iterator->second.best_move_index;

    // Update LRU list
    lru_list.erase(hash_value_iterator->second.lru_position);
    lru_list.push_front(hash);
    hash_value_iterator->second.lru_position = lru_list.begin();

    return true;
  }
  return false;
}

auto TranspositionTable::get_size() -> int { return table.size(); }

void TranspositionTable::clear()
{
  table.clear();
  lru_list.clear();
}

// PRIVATE FUNCTIONS
void TranspositionTable::trim()
{
  // Remove the least recently used entry
  uint64_t lru_hash = lru_list.back();
  lru_list.pop_back();
  table.erase(lru_hash);
}
} // namespace engine::parts