#include "transposition_table.h"

namespace engine::parts
{
// CONSTRUCTORS

TranspositionTable::TranspositionTable(uint64_t max_size) : max_size(max_size)
{
  tt_table = new TranspositionTableEntry[max_size];
}

TranspositionTable::~TranspositionTable() { delete[] tt_table; }

// PUBLIC FUNCTIONS

void TranspositionTable::store(uint64_t &hash, int search_depth, int eval_score,
                               int flag, int best_move_index)
{
  // Get the entry.
  TranspositionTableEntry &entry = tt_table[hash % max_size];

  // Update the entry.
  entry.hash = hash;
  entry.search_depth = search_depth;
  entry.eval_score = eval_score;
  entry.flag = flag;
  entry.best_move_index = best_move_index;
}

auto TranspositionTable::retrieve(uint64_t &hash, int &search_depth,
                                  int &eval_score, int &flag,
                                  int &best_move_index) -> bool
{
  TranspositionTableEntry entry = tt_table[hash % max_size];

  if (entry.hash == hash)
  {
    search_depth = entry.search_depth;
    eval_score = entry.eval_score;
    flag = entry.flag;
    best_move_index = entry.best_move_index;

    return true;
  }
  return false;
}

void TranspositionTable::clear()
{
  delete[] tt_table;
  tt_table = new TranspositionTableEntry[max_size];
}
} // namespace engine::parts