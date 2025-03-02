#include "search_engine.h"

#include <chrono>

// CONSTRUCTORS
SearchEngine::SearchEngine(BoardState &board_state)
    : game_board_state(board_state), position_evaluator(PositionEvaluator()),
      transposition_table(10000000) {} // Initialize with a max size

// PUBLIC FUNCTIONS
auto SearchEngine::calculate_possible_moves(BoardState &board_state)
    -> std::vector<Move> {
  std::vector<Move> possible_moves;
  for (int y_coordinate = 0; y_coordinate < 8; ++y_coordinate) {
    for (int x_coordinate = 0; x_coordinate < 8; ++x_coordinate) {
      Piece *current_piece =
          board_state.chess_board[x_coordinate][y_coordinate];
      PieceType &piece_type = current_piece->piece_type;

      if (current_piece->piece_color == board_state.color_to_move) {
        switch (piece_type) {
        case PieceType::PAWN:
          MoveGenerator::generate_pawn_move(board_state, x_coordinate,
                                            y_coordinate, possible_moves);
          break;
        case PieceType::ROOK:
          MoveGenerator::generate_rook_move(board_state, x_coordinate,
                                            y_coordinate, possible_moves);
          break;
        case PieceType::KNIGHT:
          MoveGenerator::generate_knight_move(board_state, x_coordinate,
                                              y_coordinate, possible_moves);
          break;
        case PieceType::BISHOP:
          MoveGenerator::generate_bishop_move(board_state, x_coordinate,
                                              y_coordinate, possible_moves);
          break;
        case PieceType::QUEEN:
          MoveGenerator::generate_queen_move(board_state, x_coordinate,
                                             y_coordinate, possible_moves);
          break;
        case PieceType::KING:
          MoveGenerator::generate_king_move(board_state, x_coordinate,
                                            y_coordinate, possible_moves);
          break;
        default:
          // Empty square.
          break;
        }
      }
    }
  }
  return std::move(possible_moves);
}

auto SearchEngine::execute_best_move(int max_search_depth,
                                     bool show_performance) -> bool {
  std::vector<Move> possible_moves = calculate_possible_moves(game_board_state);
  std::vector<std::pair<Move, int>> move_scores;
  bool maximising = engine_color == PieceColor::WHITE;

  // Search to max_search_depth with iterative deepening.
  for (int iterative_depth = 1; iterative_depth <= max_search_depth;
       ++iterative_depth) {
    max_iterative_search_depth = iterative_depth;
    move_scores.clear();

    std::vector<std::thread> search_threads;
    std::vector<std::promise<int>> promises(possible_moves.size());
    std::vector<std::future<int>> futures;
    std::vector<BoardState> thread_board_states(possible_moves.size(),
                                                BoardState(game_board_state));

    auto search_start_time = std::chrono::high_resolution_clock::now();
    // Search each move in a separate thread.
    for (int move_index = 0; move_index < possible_moves.size(); ++move_index) {
      thread_board_states[move_index].apply_move(possible_moves[move_index]);
      futures.push_back(promises[move_index].get_future());

      search_threads.emplace_back([this, &promises, move_index, maximising,
                                   iterative_depth, &thread_board_states] {
        int eval =
            minimax_alpha_beta_search(thread_board_states[move_index], -INF,
                                      INF, iterative_depth - 1, !maximising);
        promises[move_index].set_value(eval);
      });
    }

    // Wait for all threads to finish.
    for (auto &thread : search_threads) {
      thread.join();
    }

    // Get results from threads.
    for (int i = 0; i < possible_moves.size(); ++i) {
      move_scores.emplace_back(possible_moves[i], futures[i].get());
    }

    auto search_end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
                        search_end_time - search_start_time)
                        .count();

    // Print performance metrics.
    if (show_performance) {
      printf("Depth: %d, Time: %lldms\n", iterative_depth, duration);
      printf("Nodes Visited %d\n", nodes_visited.load());
      printf("Leaf Nodes Visited %d\n", leaf_nodes_visited.load());
      printf("TT Size: %d\n", transposition_table.get_size());
      printf("Nodes per second: %d kN/s\n\n",
             static_cast<int>(nodes_visited / (duration / 1000.0) / 1000));
    }

    // Reset performance metrics.
    nodes_visited = 0;
    leaf_nodes_visited = 0;
  }

  transposition_table.clear();
  sort_moves(move_scores);

  // Check if there are any valid moves and apply the best move.
  bool found_valid_move = false;
  for (std::pair<Move, int> move_score : move_scores) {
    game_board_state.apply_move(move_score.first);
    if (!game_board_state.king_is_checked(game_board_state.color_to_move)) {
      found_valid_move = true;
      break;
    }
    game_board_state.undo_move();
  }

  return found_valid_move;
}

// PRIVATE FUNCTIONS
auto SearchEngine::minimax_alpha_beta_search(
    BoardState &board_state, int alpha, int beta, int depth, bool maximise,
    bool previous_move_is_null) -> int {
  nodes_visited.fetch_add(1, std::memory_order_relaxed);

  int tt_value, tt_flag, entry_search_depth;
  int entry_best_move = -1;
  uint64_t hash = board_state.compute_zobrist_hash();
  // Check transposition table if position has been searched before.
  if (transposition_table.retrieve(hash, entry_search_depth, tt_value, tt_flag,
                                   entry_best_move)) {
    // Check if tt_value can be used.
    if (depth <= entry_search_depth) {
      if ((tt_flag == -1 && tt_value <= alpha) ||
          (tt_flag == 1 && tt_value >= beta) || tt_flag == 0) {
        return tt_value;
      }
    }
  }

  // Evaluate leaf nodes.
  if (depth <= 0) {
    int eval = position_evaluator.evaluate_position(board_state);
    leaf_nodes_visited.fetch_add(1, std::memory_order_relaxed);
    return eval;
  }

  // Try a null move.
  if (!previous_move_is_null && (max_iterative_search_depth - depth) >= 3) {
    // If previous move is a null move, skip this to prevent double null moves.
    // This will prevent the search from being too shallow.
    board_state.apply_null_move();
    int eval = minimax_alpha_beta_search(board_state, alpha, beta, depth - 3,
                                         !maximise, true);
    board_state.undo_null_move();
    if ((maximise && eval >= beta) || (!maximise && eval <= alpha)) {
      return eval;
    }
  }

  std::vector<Move> possible_moves = calculate_possible_moves(board_state);
  int eval;
  // Start minimax search.
  if (maximise) {
    int max_eval = -INF;
    int best_move_index = 0;
    if (entry_best_move >= 0 && entry_best_move < possible_moves.size()) {
      max_search(board_state, alpha, beta, max_eval, eval, depth,
                 best_move_index, entry_best_move, possible_moves);
    }
    for (int index = 0; index < possible_moves.size(); ++index) {
      max_search(board_state, alpha, beta, max_eval, eval, depth,
                 best_move_index, index, possible_moves);
      if (alpha >= beta) {
        break;
      }
    }
    transposition_table.store(hash, depth, max_eval, (max_eval >= beta) ? 1 : 0,
                              best_move_index);
    return max_eval;
  } else {
    int min_eval = INF;
    int best_move_index = 0;
    if (entry_best_move >= 0 && entry_best_move < possible_moves.size()) {
      min_search(board_state, alpha, beta, min_eval, eval, depth,
                 best_move_index, entry_best_move, possible_moves);
    }
    for (int index = 0; index < possible_moves.size(); ++index) {
      min_search(board_state, alpha, beta, min_eval, eval, depth,
                 best_move_index, index, possible_moves);
      if (alpha >= beta) {
        break;
      }
    }
    transposition_table.store(hash, depth, min_eval,
                              (min_eval <= alpha) ? -1 : 0, best_move_index);
    return min_eval;
  }
}

void SearchEngine::sort_moves(std::vector<std::pair<Move, int>> &move_scores) {
  if (engine_color == PieceColor::WHITE) {
    // Sort by descending order.
    sort(move_scores.begin(), move_scores.end(),
         [](const std::pair<Move, int> &a, const std::pair<Move, int> &b) {
           return a.second > b.second;
         });
  } else {
    // Sort by ascending order.
    sort(move_scores.begin(), move_scores.end(),
         [](const std::pair<Move, int> &a, const std::pair<Move, int> &b) {
           return a.second < b.second;
         });
  }
}

void SearchEngine::max_search(BoardState &board_state, int &alpha, int &beta,
                              int &max_eval, int &eval, int &depth,
                              int &best_move_index, int &move_index,
                              std::vector<Move> &possible_moves) {
  board_state.apply_move(possible_moves[move_index]);
  eval = minimax_alpha_beta_search(board_state, alpha, beta, depth - 1, false);
  if (eval > max_eval) {
    max_eval = eval;
    best_move_index = move_index;
  }
  max_eval = std::max(max_eval, eval);
  alpha = std::max(eval, alpha);
  board_state.undo_move();
}

void SearchEngine::min_search(BoardState &board_state, int &alpha, int &beta,
                              int &min_eval, int &eval, int &depth,
                              int &best_move_index, int &move_index,
                              std::vector<Move> &possible_moves) {
  board_state.apply_move(possible_moves[move_index]);
  eval = minimax_alpha_beta_search(board_state, alpha, beta, depth - 1, true);
  if (eval < min_eval) {
    min_eval = eval;
    best_move_index = move_index;
  }
  beta = std::min(eval, beta);
  board_state.undo_move();
}