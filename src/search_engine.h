#ifndef SEARCH_ENGINE_H
#define SEARCH_ENGINE_H

#include "board_state.h"
#include "node_context.h"
#include "thread_handler.h"
#include "transposition_table.h"

#include <atomic>
#include <chrono>
#include <cstddef>

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
  // CONSTANTS
  // TODO: Make this a runtime configurable constant - number of cores on the
  // machine.
  /// @brief Maximum number of search threads.
  static constexpr int MAX_SEARCH_THREADS = 14;

  /// @brief Aspiration windows for the search.
  static constexpr std::array<int, 3> ASPIRATION_WINDOWS = {
      PAWN_VALUE / 4, PAWN_VALUE, INF / 2};

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
   * @param is_uci Flag to indicate if the engine is in UCI mode.
   */
  SearchEngine(BoardState &board_state, bool is_uci);

  // FUNCTIONS

  /**
   * @brief Starts the engine search.
   *
   * @details This function starts the search thread and the timeout thread.
   */
  void start_engine_search();

  /**
   * @brief Stops the engine from searching.
   */
  void stop_engine_search();

  /**
   * @brief Waits until the engine search is done and returns the best move
   * string.
   *
   * @return The best move string.
   */
  auto wait_for_search_and_get_best_move() -> std::string;

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

  /// @brief Number of leaf nodes visited.
  std::atomic<size_t> leaf_nodes_visited = 0;

  /// @brief Number of nodes visited.
  std::atomic<size_t> nodes_visited = 0;

  /// @brief Number of nodes visited.
  std::atomic<size_t> nodes_visited_all_threads = 0;

  /// @brief Number of quiescence nodes visited.
  std::atomic<size_t> quiescence_nodes_visited = 0;

  /// @brief See BoardState.
  BoardState &game_board_state;

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
      running_search_flag, [this]() { this->run_lazy_smp_search(); });

  /// @brief One History Heuristic Table for each search thread.
  std::array<history_table_type, MAX_SEARCH_THREADS> history_tables{};

  /// @brief Best move found by the search.
  std::string best_move;

  /// @brief Best move score found by the search.
  int best_move_score = 0;

  /// @brief Flag to indicate if the engine is in UCI mode.
  bool is_uci = false;

  /// @brief Best move string found by the search.
  std::string best_move_string;

  /// @brief Number of iterations the best move has been found in.
  int best_move_iteration_count = 0;

  // FUNCTIONS

  /**
   * @brief Finds the best move for the engine and applies it to the board.
   *
   * @return True if a move is found where the king is not in checked, false
   * otherwise.
   */
  auto search_and_execute_best_move() -> bool;

  /**
   * @brief Runs the lazy SMP search.
   *
   * @details The lazy SMP search is a search algorithm that uses multiple
   * threads to search the game tree in parallel.
   *
   * @return Vector of pairs of moves and their scores.
   */
  auto run_lazy_smp_search() -> std::vector<std::pair<Move, int>>;

  /**
   * @brief Runs the iterative deepening search.
   *
   * @param thread_index Thread index of the search thread.
   * @param board_state BoardState object to search.
   *
   * @return Vector of pairs of moves and their scores.
   */
  auto run_iterative_deepening_search(int thread_index, BoardState &board_state)
      -> std::vector<std::pair<Move, int>>;

  /**
   * @brief Runs the search with aspiration window.
   *
   * @param thread_index Thread index of the search thread.
   * @param previous_eval Previous evaluation of the best move.
   * @param depth Current depth of search.
   * @param board_state BoardState object to search.
   *
   * @return Vector of pairs of moves and their scores.
   */
  auto run_search_with_aspiration_window(int thread_index,
                                         int previous_eval,
                                         int depth,
                                         BoardState &board_state)
      -> std::vector<std::pair<Move, int>>;

  /**
   * @brief Runs the negamax alpha beta search for the root node.
   *
   * @param context Node context.
   *
   * @return Vector of pairs of moves and their scores.
   */
  auto root_negamax_alpha_beta_search(NodeContext context)
      -> std::vector<std::pair<Move, int>>;

  /**
   * @brief Recursive function to evaluate the best move for the given board
   * state.
   *
   * @note Exactly the same algorithm as minimax_alpha_beta_search. Minimizing
   * node is essentially the same as a maximizing node, but with the scores and
   * bounds negated.
   *
   * @param context Node context.
   *
   * @return Evaluation score from search branch.
   */
  auto negamax_alpha_beta_search(NodeContext context) -> int;

  /**
   * @brief Sorts the moves based on their scores.
   *
   * @param move_scores Vector of moves and their scores.
   */
  static void sort_moves(std::vector<std::pair<Move, int>> &move_scores);

  /**
   * @brief Searches all possible moves.
   *
   * @param context Node context.
   */
  void run_negamax_procedure(NodeContext &context);

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
   * @param context Node context.
   * @param move_index Index of the move to search.
   * @param quiet_move_index Index of the quiet move in the possible moves
   * vector.
   * @param is_capture_move Flag to indicate if the move is a capture move.
   */
  void run_pvs_search(NodeContext &context,
                      int move_index,
                      int quiet_move_index,
                      bool is_capture_move);

  /**
   * @brief Handles the transposition table entry.
   *
   * @details Checks if the transposition table entry is valid and updates
   * the node context TT properties based on the entry.
   *
   * @param context Node context.
   *
   * @return If the entry is valid, it returns true and the search can be
   * skipped. If the entry is not valid, it returns false and the search
   * continues.
   */
  auto handle_tt_entry(NodeContext &context) -> bool;

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
   * null moves can make the search too shallow and return bad evaluations.
   *
   * @param context Node context.
   *
   * @return True if eval failed high.
   */
  auto do_null_move_search(NodeContext &context) -> bool;

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
   * @param context Node context.
   */
  void store_state_in_transposition_table(NodeContext &context);

  /**
   * @brief Resets and prints the performance matrix.
   *
   * @param iterative_depth Current iterative depth.
   * @param search_start_time Start time of search.
   * @param search_end_time End time of search.
   */
  void reset_and_print_performance_matrix(
      int iterative_depth,
      const std::chrono::time_point<std::chrono::steady_clock>
          &search_start_time,
      const std::chrono::time_point<std::chrono::steady_clock> &search_end_time,
      const std::vector<std::pair<Move, int>> &move_scores);

  /** @brief Quiescence search to lessen horizon effect.
   *
   * @details Our quiescence search will explore all possible capture moves of
   * the given board state. This is to lessen the horizon effect where the
   * engine may think it has a good position, but if explored further, it is
   * actually in a bad position.
   *
   * @param context Node context.
   *
   * @return Evaluation score from quiescence search.
   */
  auto quiescence_search(NodeContext context) -> int;

  /**
   * @brief Runs the quiescence search procedure.
   *
   * @param context Node context.
   */
  void run_quiescence_search_procedure(NodeContext &context);

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
   * @param context Node context.
   * @param move Move to check.
   */
  static auto delta_prune_move(NodeContext &context, const Move &move) -> bool;

  /**
   * @brief Checks if the given move can be futility pruned.
   *
   * @details Futility pruning is a technique used to reduce the number of
   * nodes searched in the minimax algorithm. It is based on the idea that if
   * the evaluation of a quiet move is so low that it is unlikely to be the best
   * move, then we can prune it from the search tree.
   *
   * @note This is similar to Delta pruning. Futility pruning is used during the
   * main search and for non-capture moves. Delta pruning is used during
   * quiescence search and for capture moves.
   *
   * @param context Node context.
   * @param quiet_move_index Index of the quiet move in the possible moves
   * vector.
   * @param move Move to check.
   * @param is_capture_move Flag to indicate if the move is a capture move.
   *
   * @return True if the move can be futility pruned, false otherwise.
   */
  static auto futility_prune_move(NodeContext &context,
                                  int quiet_move_index,
                                  Move &move,
                                  bool is_capture_move) -> bool;

  /**
   * @brief Updates the history table.
   *
   * @param move Move to update history table with.
   * @param eval Evaluation score of the move.
   * @param depth Current depth of search for weighting the move's score.
   * @param move_index Index of the move in the history table.
   * @param alpha Highest score to be picked by maximizing node.
   * @param beta Lowest score to be picked by minimizing node.
   * @param history_table Reference to the history table.
   */
  static void update_history_table(const Move &move,
                                   const int &eval,
                                   const int &depth,
                                   const int &move_index,
                                   const int &alpha,
                                   const int &beta,
                                   history_table_type &history_table);

  /**
   * @brief Decays the history table.
   *
   * @details Decays all entries in the history table by 0.9 after every search.
   * This is to prevent the history table from being too biased towards a
   * certain move. The position changes throughout the game, and the history
   * table should reflect that.
   *
   * @param history_table Reference to the history table.
   */
  static void decay_history_table(history_table_type &history_table);

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
