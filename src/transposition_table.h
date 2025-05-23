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

  /// @brief Hash of the board state.
  uint64_t hash = 0;

  /// @brief Maximum depth of the search.
  int search_depth = 0;

  /// @brief Value of the board state.
  int eval_score = 0;

  /// @brief Flag of the value. 0 = exact, 1 = lower bound, 2 = upper bound.
  int flag = 0;

  /// @brief Index of the best move in the board state.
  int best_move_index = 0;

  /// @brief Flag to check if the entry is a quiescence search.
  bool is_quiescence = false;

  /// @brief Checksum of the entry.
  uint32_t checksum = 0;
};

/**
 * @brief Class that creates and manages the transposition table for storing and
 * retrieving board states and their evaluations.
 */
class TranspositionTable
{
public:
  // CONSTRUCTORS

  /**
   * @brief Construct a new Transposition Table object with a specified maximum
   * size.
   *
   * @param max_size Maximum number of entries in the transposition table.
   */
  TranspositionTable(uint64_t max_size);

  /**
   * @brief Destroy the Transposition Table object and free allocated memory.
   */
  ~TranspositionTable();

  // FUNCTIONS

  /**
   * @brief Store a new entry in the transposition table.
   *
   * @param hash Hash of the board state.
   * @param search_depth Depth searched for this position.
   * @param eval_score Evaluation score of the board state.
   * @param flag Flag of the value (0 = exact, 1 = lower bound, 2 = upper
   * bound).
   * @param best_move_index Index of the best move in the board state.
   * @param is_quiescence Flag to check if the entry is a quiescence search
   * (default is false).
   */
  void store(uint64_t &hash,
             int search_depth,
             int eval_score,
             int flag,
             int best_move_index,
             bool is_quiescence = false);

  /**
   * @brief Retrieve an entry from the transposition table.
   *
   * @param hash Hash of the board state.
   * @param search_depth Depth searched for this position (output parameter).
   * @param eval_score Evaluation score of the board state (output parameter).
   * @param flag Flag of the value (output parameter).
   * @param best_move_index Index of the best move in the board state (output
   * parameter).
   * @param is_quiescence Flag to check if the entry is a quiescence search
   * (default is false).
   *
   * @return True if the entry was found, false otherwise.
   */
  auto retrieve(uint64_t &hash,
                int &search_depth,
                int &eval_score,
                int &flag,
                int &best_move_index,
                bool is_quiescence = false) -> bool;

  /**
   * @brief Clear the transposition table.
   *
   * @details Removes all entries from the transposition table and resets its
   * state.
   */
  void clear();

private:
  // PROPERTIES

  /// @brief Size of the transposition table.
  uint64_t max_size;

  /// @brief Transposition table represented as an array.
  TranspositionTableEntry *tt_table;

  // FUNCTIONS

  /**
   * @brief Calculate the checksum of the entry.
   *
   * @param hash Hash of the board state.
   * @param depth Depth searched for this position.
   * @param eval_score Evaluation score of the board state.
   * @param flag Flag of the value.
   * @param best_move_index Index of the best move in the board state.
   * @param is_quiescence Flag to check if the entry is a quiescence search.
   *
   * @return Calculated checksum.
   */
  static auto calculate_checksum(uint64_t hash,
                                 int depth,
                                 int eval_score,
                                 int flag,
                                 int best_move_index,
                                 bool is_quiescence) -> uint32_t;
};
} // namespace engine::parts

#endif // TRANSPOSITION_TABLE_H