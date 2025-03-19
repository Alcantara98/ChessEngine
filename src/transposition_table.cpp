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
                               int best_move_index)
{
  // Get the entry.
  TranspositionTableEntry &entry = tt_table[hash % max_size];
  uint32_t checksum =
      calculate_checksum(hash, search_depth, eval_score, flag, best_move_index);

  // Update the entry.
  entry.hash = hash;
  entry.search_depth = search_depth;
  entry.eval_score = eval_score;
  entry.flag = flag;
  entry.best_move_index = best_move_index;
  entry.checksum = checksum;
}

auto TranspositionTable::retrieve(uint64_t &hash,
                                  int &search_depth,
                                  int &eval_score,
                                  int &flag,
                                  int &best_move_index) -> bool
{
  TranspositionTableEntry entry = tt_table[hash % max_size];

  if (entry.hash == hash)
  {
    search_depth = entry.search_depth;
    eval_score = entry.eval_score;
    flag = entry.flag;
    best_move_index = entry.best_move_index;

    uint32_t checksum = calculate_checksum(hash, search_depth, eval_score, flag,
                                           best_move_index);
    if (checksum == entry.checksum)
    {
      return true;
    }
  }
  return false;
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
                                            int best_move_index) -> uint32_t
{
  uint32_t checksum = CHECKSUM_SEED;
  checksum ^= hash;

  // Thes evalues may be small, so multiply by a prime number to get a better
  // distribution, and make the checksum more unique.
  checksum ^= depth * CHECKSUM_PRIMES[0];
  checksum ^= eval_score * CHECKSUM_PRIMES[1];
  checksum ^= flag * CHECKSUM_PRIMES[2];
  checksum ^= best_move_index * CHECKSUM_PRIMES[3];

  return checksum;
}
} // namespace engine::parts