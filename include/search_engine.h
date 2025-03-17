#ifndef SEARCH_ENGINE_H
#define SEARCH_ENGINE_H

#include "board_state.h"
#include "move_generator.h"
#include "move_interface.h"
#include "position_evaluator.h"
#include "transposition_table.h"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <future>
#include <thread>

namespace engine::parts
{
/**
 * @brief Class to find the best move for the current board state using various
 * search algorithms and heuristics.
 */
class SearchEngine
{

public:
  // PROPERTIES

  /// @brief Determines which color to maximise for.
  PieceColor engine_color = PieceColor::WHITE;

  /// @brief Max depth to search.
  int max_search_depth;

  /// @brief Max time to search in milliseconds.
  int max_search_time_milliseconds;

  /// @brief Show performance matrix of the search.
  bool show_performance = false;

  /// @brief Show move evaluations.
  bool show_move_evaluations = false;

  /// @brief Flag to run search with aspiration window.
  bool use_aspiration_window = true;

  /// @brief Transposition Table object.
  TranspositionTable transposition_table;

  // CONSTRUCTORS
  /**
   * @brief Default Constructor - takes a chess board state.
   *
   * @param board_state BoardState object representing the current state of the
   * chess board.
   */
  SearchEngine(BoardState &board_state);

  // FUNCTIONS

  /**
   * @brief Finds the best move for the engine and applies it to the board.
   *
   * @return True if a move is found where the king is not in checked, false
   * otherwise.
   */
  auto execute_best_move() -> bool;

  /**
   * @brief Clears the previous move evaluations.
   */
  void clear_previous_move_evals();

  /**
   * @brief Gets the last move evaluation score.
   * @note If no moves have been evaluated, it returns 0.
   *
   * @return Last move evaluation score.
   */
  auto last_move_eval() -> int;

  /**
   * @brief Pops the last move evaluation score.
   */
  void pop_last_move_eval();

private:
  // PROPERTIES

  /// @brief Number of leaf nodes visited.
  std::atomic<int> leaf_nodes_visited = 0;

  /// @brief Number of nodes visited.
  std::atomic<int> nodes_visited = 0;

  /// @brief See BoardState.
  BoardState &game_board_state;

  /// @brief The max depth that the current iterative search will reach.
  /// NOTE: Not to be confused with max_search_depth. This is the depth the
  /// current iteration will reach.
  int max_iterative_search_depth;

  /// @brief The evaluation score of the previous moves.
  std::stack<int> previous_move_evals;

  /// @brief Flag to stop the iterative search.
  bool stop_search_flag = false;

  /// @brief For stopping the search timeout thread if search reaches max depth
  /// first before timelimit is reached.
  std::condition_variable search_timeout_cv;
  std::mutex search_timeout_mutex;

  /// @brief The best score of current iterative search. Reset after each
  /// iteration.
  /// NOTE: This is used to stop other threads from widening the
  /// search window in the aspiration window function unnecessarily. Do not use
  /// this score to determine the best move.
  std::atomic<int> current_iterative_best_move_score = -INF;

  // FUNCTIONS

  /**
   * @brief Evaluates all possible moves and scores them.
   *
   * @param move_scores Vector of moves and their scores.
   */
  void evaluate_possible_moves(std::vector<std::pair<Move, int>> &move_scores);

  /**
   * @brief Encapsulates the iterative deepening search for each move to apply
   * aspiration window heuristic.
   *
   * @details Aspiration window heuristic is used to reduce the search space by
   * only searching moves that are within a certain range of the previous
   * evaluation score (previous evaluation score at the start is 0).
   * If the evaluation score is within the window, the search stops and returns
   * the score.
   * If the evaluation score is outside the window, re-search the move with a
   * larger window.
   * Window size is increased 2x if the search is outside the window, with the
   * last window being the full window (alpha = -INF, beta = INF).
   * This will reduce the search space and make the search faster if the exact
   * eval is within the window we choose. If the eval is not within the window,
   * the search will be slower as we have to re-search with a larger window.
   * Improve window selection!
   *
   * @param board_state BoardState object to search.
   * @param depth Current depth of search.
   *
   * @return Evaluation score from search branch.
   */
  auto run_search_with_aspiration_window(BoardState &board_state,
                                         int depth) -> int;

  /**
   * @brief Recursive function to find the best move using minimax algorithm
   * with alpha beta pruning.
   *
   * @note Exactly the same algorithm as minimax_alpha_beta_search. Minimizing
   * node is essentially the same as a maximizing node, but with the scores and
   * bounds negated.
   *
   * @param board_state BoardState object to search.
   * @param alpha Highest score to be picked by maximizing node.
   * @param beta Lowest score to be picked by minimizing node.
   * @param depth Current depth of search.
   * @param null_move_line Flag to indicate if the search line is from a null
   * move.
   *
   * @return Evaluation score from search branch.
   */
  auto negamax_alpha_beta_search(BoardState &board_state, int alpha, int beta,
                                 int depth, bool null_move_line) -> int;

  /**
   * @brief Sorts the moves based on their scores.
   *
   * @param move_scores Vector of moves and their scores.
   */
  static void sort_moves(std::vector<std::pair<Move, int>> &move_scores);

  /**
   * @brief Searches all possible moves.
   *
   * @param board_state BoardState object to search.
   * @param alpha Highest score to be picked by maximizing node.
   * @param beta Lowest score to be picked by minimizing node.
   * @param max_eval Current maximum evaluation score.
   * @param eval Evaluation score from search branch.
   * @param depth Current depth of search.
   * @param best_move_index Index of best move.
   * @param possible_moves Vector of possible moves.
   * @param null_move_line Flag to indicate if the search line is from a null
   * move.
   */
  void run_negamax_procedure(BoardState &board_state, int &alpha, int &beta,
                             int &max_eval, int &eval, int &depth,
                             int &best_move_index,
                             std::vector<Move> &possible_moves,
                             bool &null_move_line);

  /**
   * @brief Min search procedure for each possible move.
   *
   * @details The theory behind null move search is that if the current player
   * can make a null move (skip their turn) and still have a good position (eval
   * > beta), then the opponent's position is bad and we can cut off without
   * searching any moves (If we actually make a move, their position will be
   * even worse). In return, we decrease the maximum depth of the search by 1
   * making the search faster.
   *
   * @note We currently only allow one null move per search line. Too many null
   * moves can make the search too shallow and return bull shit evaluations.
   *
   * @param board_state BoardState object to search.
   * @param alpha Highest score to be picked by maximizing node.
   * @param beta Lowest score to be picked by minimizing node.
   * @param depth Current depth of search.
   * @param eval Evaluation score to be updated.
   */
  void do_null_move_search(BoardState &board_state, int &alpha, int &beta,
                           int &depth, int &eval);

  /**
   * @brief Stores the state in the transposition table.
   *
   * @param hash Hash of the board state.
   * @param depth Depth of the search.
   * @param max_eval Maximum evaluation score.
   * @param alpha Highest score to be picked by maximizing node.
   * @param beta Lowest score to be picked by minimizing node.
   * @param best_move_index Index of best move.
   */
  void store_state_in_transposition_table(uint64_t &hash, int &depth,
                                          int &max_eval, int &alpha, int &beta,
                                          int &best_move_index);

  /**
   * @brief Resets and prints the performance matrix.
   *
   * @param iterative_depth Current iterative depth.
   * @param search_start_time Start time of search.
   * @param search_end_time End time of search.
   */
  void reset_and_print_performance_matrix(
      int iterative_depth,
      std::chrono::time_point<std::chrono::steady_clock> search_start_time,
      std::chrono::time_point<std::chrono::steady_clock> search_end_time);

  /**
   * @brief Initiates the search timeout.
   *
   * @details The search timeout is used to stop the search if it exceeds the
   * allowed search time. It will do so by setting the stop_search_flag to true.
   */
  void initiate_search_timeout();
};
} // namespace engine::parts

#endif // SEARCH_ENGINE_H