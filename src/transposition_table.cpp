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

void TranspositionTable::store(uint64_t &hash,
                               int search_depth,
                               int eval_score,
                               int flag,
                               int best_move_index,
                               bool is_quiescence)
{
  // Get the entry.
  TranspositionTableEntry &entry = tt_table[hash % max_size];
  uint32_t checksum = calculate_checksum(hash, search_depth, eval_score, flag,
                                         best_move_index, is_quiescence);

  // Update the entry.
  entry.hash = hash;
  entry.search_depth = search_depth;
  entry.eval_score = eval_score;
  entry.flag = flag;
  entry.best_move_index = best_move_index;
  entry.is_quiescence = is_quiescence;
  entry.checksum = checksum;
}

auto TranspositionTable::retrieve(uint64_t &hash,
                                  int &search_depth,
                                  int &eval_score,
                                  int &flag,
                                  int &best_move_index,
                                  bool is_quiescence) -> bool
{
  // We need to mod the hash to get the index because the hash has a larger
  // range than the table size. This will cause collisions, and potentially
  // have racy writes to the same entry and cause mixed data from different
  // states.
  // We handle this by using a checksum to verify the data.
  TranspositionTableEntry entry = tt_table[hash % max_size];

  if (entry.hash != hash)
  {
    return false;
  }

  search_depth = entry.search_depth;
  eval_score = entry.eval_score;
  flag = entry.flag;
  best_move_index = entry.best_move_index;
  bool tt_is_quiescence = entry.is_quiescence;
  uint32_t tt_checksum = entry.checksum;

  uint32_t checksum = calculate_checksum(hash, search_depth, eval_score, flag,
                                         best_move_index, tt_is_quiescence);

  if (checksum != tt_checksum)
  {
    return false;
  }

  if (is_quiescence != tt_is_quiescence)
  {
    return false;
  }

  return true;
}

void TranspositionTable::clear()
{
  delete[] tt_table;
  tt_table = new TranspositionTableEntry[max_size];
}

// PRIVATE FUNCTIONS
auto TranspositionTable::calculate_checksum(uint64_t hash,
                                            int depth,
                                            int eval_score,
                                            int flag,
                                            int best_move_index,
                                            bool is_quiescence) -> uint32_t
{
  uint32_t checksum = CHECKSUM_SEED;
  checksum ^= hash;

  // Thes evalues may be small, so multiply by a prime number to get a better
  // distribution, and make the checksum more unique.
  checksum ^= depth * CHECKSUM_PRIMES[0];
  checksum ^= eval_score * CHECKSUM_PRIMES[1];
  checksum ^= flag * CHECKSUM_PRIMES[2];
  checksum ^= best_move_index * CHECKSUM_PRIMES[3];
  checksum ^= static_cast<int>(is_quiescence) * CHECKSUM_PRIMES[4];

  return checksum;
}
} // namespace engine::parts