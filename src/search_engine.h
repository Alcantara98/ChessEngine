#ifndef SEARCH_ENGINE_H
#define SEARCH_ENGINE_H

#include "attack_check.h"
#include "board_state.h"
#include "move_generator.h"
#include "move_interface.h"
#include "position_evaluator.h"
#include "thread_handler.h"
#include "transposition_table.h"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <future>
#include <thread>

namespace engine::parts
{
/// @brief Array to represent the history heuristic table.
using history_table_type = std::array<
    std::array<std::array<std::array<int, BOARD_HEIGHT>, BOARD_WIDTH>,
               NUM_OF_PIECE_TYPES>,
    NUM_OF_COLORS>;

/**
 * @brief Class to find the best move for the current board state using
 * various search algorithms and heuristics and apply it to the given board.
 */
class SearchEngine
{

public:
  // PROPERTIES

  /// @brief Determines which color to maximise for.
  PieceColor engine_color = PieceColor::BLACK;

  /// @brief Max depth to search.
  int max_search_depth = MAX_SEARCH_DEPTH;

  /// @brief Max time to search in milliseconds.
  int max_search_time_milliseconds = DEFAULT_SEARCH_TIME_MS;

  /// @brief Show performance matrix of the search.
  bool show_performance = true;

  /// @brief Show performance matrix of pondering search.
  bool show_ponder_performance = false;

  /// @brief Show move evaluations.
  bool show_move_evaluations = true;

  /// @brief Flag to run search with aspiration window.
  bool use_aspiration_window = true;

  /// @brief Flag to run search with null move pruning.
  bool engine_is_pondering = false;

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
   */
  void handle_engine_turn();

  /**
   * @brief Stops the engine from searching.
   */
  void stop_engine_turn();

  /**
   * @brief Starts the engine pondering.
   */
  void start_engine_pondering();

  /**
   * @brief Stops the engine pondering.
   */
  void stop_engine_pondering();

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

  /**
   * @brief Checks if the engine is currently searching.
   */
  auto engine_is_searching() -> bool;

  /**
   * @brief Clears the transposition table.
   */
  void clear_transposition_table();

private:
  // PROPERTIES

  /// @brief Keeps track of the best eval of current search iteration.
  /// NOTE: Atomic because it is accessed by multiple search threads.
  std::atomic<int> best_eval_of_search_iteration = -INF;

  /// @brief Number of leaf nodes visited.
  /// NOTE: Atomic because it is accessed by multiple search threads.
  std::atomic<int> leaf_nodes_visited = 0;

  /// @brief Number of nodes visited.
  /// NOTE: Atomic because it is accessed by multiple search threads.
  std::atomic<int> nodes_visited = 0;

  /// @brief Number of quiescence nodes visited.
  /// NOTE: Atomic because it is accessed by multiple search threads.
  std::atomic<int> quiescence_nodes_visited = 0;

  /// @brief See BoardState.
  BoardState &game_board_state;

  /// @brief The max depth that the current iterative search will reach.
  /// NOTE: Not to be confused with max_search_depth. This is the depth the
  /// current iteration will reach.
  std::atomic<int> max_iterative_search_depth;

  /// @brief The evaluation score of the previous moves.
  std::stack<int> previous_move_evals;

  /// @brief Flag to stop the iterative search.
  /// NOTE: Atomic because it is accessed by multiple search threads.
  std::atomic<bool> running_search_flag = false;

  /// @brief Transposition Table object.
  TranspositionTable transposition_table;

  /// @brief Runs and handles the search thread.
  ThreadHandler search_thread_handler = ThreadHandler(
      running_search_flag, [this]() { this->search_and_execute_best_move(); });

  /// @brief Runs and handles the pondering thread.
  ThreadHandler ponder_thread_handler = ThreadHandler(
      running_search_flag,
      [this]() { this->run_iterative_deepening_search_evaluation(); });

  /// @brief History Heuristic Table.
  history_table_type history_table = {};

  // FUNCTIONS

  /**
   * @brief Finds the best move for the engine and applies it to the board.
   *
   * @return True if a move is found where the king is not in checked, false
   * otherwise.
   */
  auto search_and_execute_best_move() -> bool;

  /**
   * @brief Evaluates all possible moves and scores them.
   *
   * @note This function is used to evaluate all possible moves and their scores
   * using iterative deepening search. The search is done in parallel using
   * multiple threads for each possible move.
   *
   * @details Iterative deepening search helps by searching at lower depths
   * first and saving the best move it has found so far in the transposition
   * table. This allows the next deeper search iteration to search a likely best
   * move first, causing more alpha beta pruning to occur.
   *
   * @return Vector of pairs of moves and their scores.
   */
  auto run_iterative_deepening_search_evaluation()
      -> std::vector<std::pair<Move, int>>;

  /**
   * @brief Prunes the root moves that are not in the top 50% of the search so
   * far.
   *
   * @details This is done to reduce the search space and focus on the most
   * promising moves. The moves are sorted based on their scores and the top 50%
   * of the moves are kept for further searching. The rest of the moves are
   * removed from the search.
   *
   * @param moves_to_search Map of moves to search.
   * @param move_scores Vector of moves and their scores.
   * @param current_depth Current depth of iterative search.
   */
  static void prune_root_moves(std::map<int, bool> &moves_to_search,
                               std::vector<std::pair<Move, int>> &move_scores,
                               int &current_depth);

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
   * @brief Recursive function to evaluate the best move for the given board
   * state.
   *
   * @note Exactly the same algorithm as minimax_alpha_beta_search. Minimizing
   * node is essentially the same as a maximizing node, but with the scores and
   * bounds negated.
   *
   * @param board_state BoardState object to search.
   * @param alpha Highest score to be picked by maximizing node.
   * @param beta Lowest score to be picked by minimizing node.
   * @param depth Current depth of search.
   * @param is_null_move_line Flag to indicate if the search line is from a null
   * move.
   * @param is_lmr_line Flag to indicate if the search line is from a late move
   * reduction line.
   * @param is_pvs_line Flag to indicate if the node is a PVS node.
   *
   * @return Evaluation score from search branch.
   */
  auto negamax_alpha_beta_search(BoardState &board_state,
                                 int alpha,
                                 int beta,
                                 int depth,
                                 bool is_null_move_line,
                                 bool is_lmr_line,
                                 bool is_pvs_line) -> int;

  /**
   * @brief Handles the leaf node of the search tree.
   *
   * @note Handles the evaluation of the leaf node of the search tree. This is
   * where the search tree ends and the evaluation of the board state is
   * performed.
   *
   * @param board_state BoardState object to search.
   * @param alpha Highest score to be picked by maximizing node.
   * @param beta Lowest score to be picked by minimizing node.
   *
   * @return Evaluation score of the leaf node.
   */
  auto evaluate_leaf_node(BoardState &board_state, int alpha, int beta) -> int;

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
   * @param is_null_move_line Flag to indicate if the search line is from a null
   * move.
   * @param is_lmr_line Flag to indicate if the search line is from a late move
   * reduction line.
   * @param is_pvs_line Flag to indicate if the node is a PVS node.
   */
  void run_negamax_procedure(BoardState &board_state,
                             int &alpha,
                             int &beta,
                             int &max_eval,
                             int &eval,
                             int &depth,
                             int &best_move_index,
                             std::vector<Move> &possible_moves,
                             bool &is_null_move_line,
                             bool &is_lmr_line,
                             bool &is_pvs_line);

  /**
   * @brief Runs the Principal Variation Search (PVS) algorithm.
   *
   * @details In chess, the Principal Variation is the sequence of moves that
   * the engine believes is the best sequence of moves for both players.
   *
   * @details The PVS algorithm is a variation of the negamax algorithm that
   * reduces the number of nodes searched by first running a full search on the
   * PVS node (best move) of the current board state. We then search the rest of
   * the moves with a null window search around alpha. If the null window search
   * fails high, we do a full search on the move. If it fails low, we skip the
   * full search of the move.
   *
   * @details We also do Late Move Reduction (LMR) to reduce the number of
   * nodes. LMR is a heuristic that reduces the depth of the search for moves
   * that are not likely to be good moves. We currently sort the moves using
   * MVV-LVV and History Heuristic. This means that the worst moves are likely
   * to be at the end of the list. We reduce the depth of the search for moves
   * that are 'late' in the list of moves.
   *
   * @param board_state BoardState object to search.
   * @param move_index Index of the move to search.
   * @param late_move_threshold Threshold to determine if a move is late.
   * @param eval Evaluation score from search branch.
   * @param alpha Highest score to be picked by maximizing node.
   * @param beta Lowest score to be picked by minimizing node.
   * @param depth Current depth of search.
   * @param is_null_move_line Flag to indicate if the search line is from a null
   * move.
   * @param is_lmr_line Flag to indicate if the search line is from a late move
   * reduction line.
   * @param is_pvs_line Flag to indicate if the node is a PVS node.
   */
  void run_pvs_search(BoardState &board_state,
                      int &move_index,
                      int &late_move_threshold,
                      int &eval,
                      int &alpha,
                      int &beta,
                      int &depth,
                      bool &is_null_move_line,
                      bool &is_lmr_line,
                      bool &is_pvs_line);

  /**
   * @brief Runs the PVS scout search algorithm.
   *
   * @details Search the best move first with a null window. If the eval is
   * above alpha, set this as the best_eval_of_search_iteration. Null window
   * should make the search quick, and if we get a high eval, we can do a lot of
   * pruning in the aspiration window function.
   */
  void run_pvs_scout_search();

  /**
   * @brief Min search procedure for each possible move.
   *
   * @details The theory behind null move search is that if the current
   * player can make a null move (skip their turn) and still have a good
   * position (eval > beta), then the opponent's position is bad and we can
   * cut off without searching any moves (If we actually make a move, their
   * position will be even worse). In return, we decrease the maximum depth
   * of the search by 1 making the search faster.
   *
   * @note We currently only allow one null move per search line. Too many
   * null moves can make the search too shallow and return bull shit
   * evaluations.
   *
   * @param board_state BoardState object to search.
   * @param alpha Highest score to be picked by maximizing node.
   * @param beta Lowest score to be picked by minimizing node.
   * @param depth Current depth of search.
   * @param eval Evaluation score to be updated.
   * @param is_lmr_line Flag to indicate if the search line is from a late move
   * reduction line.
   *
   * @return True if eval failed high.
   */
  auto do_null_move_search(BoardState &board_state,
                           int &alpha,
                           int &beta,
                           int &depth,
                           int &eval,
                           bool &is_lmr_line) -> bool;

  /**
   * @brief Handles necessary eval adjustments to prevent stalemates and loops.
   *
   * @note If node is part of a checkmate sequence, we adjust the
   * score by 1 so that the engine can follow the sequence that leads to a
   * checkmate. This is needed otherwise all boardstates that are part of a
   * checkmate sequence will save the same eval score in the transposition
   * table. If that is the case, the engine may just go back and forth between
   * two boardstates. By doing this adjustment, the closer the boardstate is to
   * the checkmate state, the higher the evaluation.
   *
   * @note For a node to be part of an checkmate sequence, its max_eval must be
   * larger than INF_MINUS_1000 or less than -INF_MINUS_1000 points.
   *
   * @note An eval of less than -INF_MINUS_1000 indicates that every move by the
   * current player eventually leads to the capture of their king. This
   * situation is caused by either checkmate or stalemate. Performing
   * explicit checks for checkmate/stalemate during the negamax search would be
   * too costly, so the engine relies on detecting king capture first, and then
   * checks for checkmate/stalemate in this function.
   *
   * @param eval Evaluation score to be adjusted.
   * @param board_state BoardState object to search.
   */
  static void handle_eval_adjustments(int &eval, BoardState &board_state);

  /**
   * @brief Stores the state in the transposition table.
   *
   * @param hash Hash of the board state.
   * @param depth Depth of the search.
   * @param max_eval Maximum evaluation score.
   * @param alpha Highest score to be picked by maximizing node.
   * @param beta Lowest score to be picked by minimizing node.
   * @param best_move_index Index of best move.
   * @param is_quiescence Flag to check if the entry is a quiescence search.
   */
  void store_state_in_transposition_table(uint64_t &hash,
                                          int &depth,
                                          int &max_eval,
                                          int &alpha,
                                          int &beta,
                                          int &best_move_index,
                                          bool is_quiescence = false);

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

  /** @brief Quiescence search to lessen horizon effect.
   *
   * @details Our quiescence search will explore all possible capture moves of
   * the given board state. This is to lessen the horizon effect where the
   * engine may think it has a good position, but if explored further, it is
   * actually in a bad position.
   *
   * @param alpha Highest score to be picked by maximizing node.
   * @param beta Lowest score to be picked by minimizing node.
   * @param board_state BoardState object to search.
   *
   * @return Evaluation score from quiescence search.
   */
  auto quiescence_search(int alpha, int beta, BoardState &board_state) -> int;

  /**
   * @brief Runs the quiescence search procedure.
   *
   * @param board_state BoardState object to search.
   * @param alpha Highest score to be picked by maximizing node.
   * @param beta Lowest score to be picked by minimizing node.
   * @param best_eval Current best evaluation score.
   * @param best_move_index Index of the best move.
   * @param current_eval Current evaluation score.
   * @param possible_moves Vector of possible moves.
   */
  void run_quiescence_search_procedure(BoardState &board_state,
                                       int &alpha,
                                       int &beta,
                                       int &best_eval,
                                       int &best_move_index,
                                       int &current_eval,
                                       std::vector<Move> &possible_moves);

  /**
   * @brief Checks if the given move can be delta pruned.
   *
   * @note If the current_eval is so low that the score gained from
   * capturing the piece in the move + a queen or rook piece value (900 and 500)
   * will not bring it back up to alpha, then it is not worth
   * searching the move.
   *
   * @note Don't do it in the end game since it is more likely
   * that bad moves are the best a player could do.
   *
   * @param board_state BoardState object to search.
   * @param move Move to check.
   * @param current_eval Current evaluation score.
   * @param alpha Highest score to be picked by maximizing node.
   */
  static auto delta_prune_move(const BoardState &board_state,
                               const Move &move,
                               const int &current_eval,
                               const int &alpha) -> bool;

  /**
   * @brief Checks if the given move can be futility pruned.
   *
   * @details Futility pruning is a technique used to reduce the number of
   * nodes searched in the minimax algorithm. It is based on the idea that if
   * the evaluation of a move is so low that it is unlikely to be the best move,
   * then we can prune it from the search tree.
   *
   * @note This is similar to Delta pruning. Futility pruning is used during the
   * main search and for non-capture moves. Delta pruning is used during
   * quiescence search and for capture moves.
   *
   * @param board_state BoardState object to search.
   * @param move Move to check.
   * @param alpha Highest score to be picked by maximizing node.
   * @param depth Current depth of search.
   *
   * @return True if the move can be futility pruned, false otherwise.
   */
  auto futility_prune_move(BoardState &board_state,
                           const Move &move,
                           const int &alpha,
                           const int &depth) -> bool;

  /**
   * @brief Updates the history table.
   *
   * @param move Move to update history table with.
   * @param eval Evaluation score of the move.
   * @param depth Current depth of search for weighting the move's score.
   * @param move_index Index of the move in the history table.
   * @param alpha Highest score to be picked by maximizing node.
   * @param beta Lowest score to be picked by minimizing node.
   */
  void udpate_history_table(const Move &move,
                            const int &eval,
                            const int &depth,
                            const int &move_index,
                            const int &alpha,
                            const int &beta);

  /**
   * @brief Decays the history table.
   *
   * @details Decays all entries in the history table by 0.9 after every search.
   * This is to prevent the history table from being too biased towards a
   * certain move. The position changes throughout the game, and the history
   * table should reflect that.
   */
  void decay_history_table();

  /**
   * @brief Puts the best move at the front of the possible moves vector.
   *
   * @param possible_moves Reference to the vector of possible moves.
   * @param best_move_index Reference to the index of the best move.
   */
  static void put_best_move_at_front(std::vector<Move> &possible_moves,
                                     int &best_move_index);
};
} // namespace engine::parts

#endif // SEARCH_ENGINE_H