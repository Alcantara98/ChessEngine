#include "search_engine.h"
#include "transposition_table.h"

#include <chrono>

// CONSTRUCTORS
SearchEngine::SearchEngine(BoardState &board_state)
    : board_state(board_state),
      position_evaluator(PositionEvaluator(board_state)),
      transposition_table(10000000) {} // Initialize with a max size

// PUBLIC FUNCTIONS
auto SearchEngine::calculate_possible_moves() -> std::vector<Move> {
  std::vector<Move> possible_moves;
  for (int y = 0; y < 8; ++y) {
    for (int x = 0; x < 8; ++x) {
      Piece *current_piece = board_state.chess_board[x][y];
      PieceType &piece_type = current_piece->type;

      if (current_piece->color == board_state.move_color) {
        switch (piece_type) {
        case PieceType::PAWN:
          MoveGenerator::generate_pawn_move(board_state, x, y, possible_moves);
          break;
        case PieceType::ROOK:
          MoveGenerator::generate_rook_move(board_state, x, y, possible_moves);
          break;
        case PieceType::KNIGHT:
          MoveGenerator::generate_knight_move(board_state, x, y,
                                              possible_moves);
          break;
        case PieceType::BISHOP:
          MoveGenerator::generate_bishop_move(board_state, x, y,
                                              possible_moves);
          break;
        case PieceType::QUEEN:
          MoveGenerator::generate_queen_move(board_state, x, y, possible_moves);
          break;
        case PieceType::KING:
          MoveGenerator::generate_king_move(board_state, x, y, possible_moves);
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

auto SearchEngine::find_best_move(int max_search_depth, bool show_performance)
    -> Move {
  std::vector<Move> possible_moves = calculate_possible_moves();
  std::vector<std::pair<Move, int>> move_scores;
  bool maximising = engine_color == PieceColor::WHITE;

  // Search to max_search_depth with iterative deepening.
  for (int iterative_depth = 1; iterative_depth <= max_search_depth;
       ++iterative_depth) {
    move_scores.clear();
    iterative_depth_search = iterative_depth;
    auto start_time = std::chrono::high_resolution_clock::now();
    for (auto &move : possible_moves) {
      board_state.apply_move(move);
      int eval = minimax_alpha_beta_search(-INF, INF, iterative_depth - 1,
                                           !maximising);
      move_scores.emplace_back(move, eval);
      board_state.undo_move();
    }
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
                        end_time - start_time)
                        .count();
    if (show_performance) {
      printf("Depth: %d, Time: %lldms\n", iterative_depth, duration);
      printf("Nodes Visited %d\n", nodes_visited);
      printf("Leaf Nodes Visited %d\n", leaf_nodes_visited);
      printf("TT Size: %d\n", transposition_table.get_size());
      printf("Nodes per second: %d kN/s\n",
             static_cast<int>(nodes_visited / (duration / 1000.0) / 1000));
    }
    nodes_visited = 0;
    leaf_nodes_visited = 0;
  }
  transposition_table.clear();
  sort_moves(move_scores);

  return move_scores[0].first;
}

// PRIVATE FUNCTIONS
auto SearchEngine::minimax_alpha_beta_search(int alpha, int beta, int depth,
                                             bool maximise) -> int {
  nodes_visited++;
  int tt_value, tt_flag, entry_depth;
  int entry_best_move = -1;
  uint64_t hash = board_state.compute_zobrist_hash();
  if (transposition_table.retrieve(hash, entry_depth, tt_value, tt_flag,
                                   entry_best_move)) {
    if (entry_depth == iterative_depth_search && tt_flag == 0) {
      return tt_value;
    }
    if (entry_depth == iterative_depth_search) {
      if ((tt_flag == -1 && tt_value <= alpha) ||
          (tt_flag == 1 && tt_value >= beta)) {
        return tt_value;
      }
    }
  }

  if (depth == 0) {
    int eval = position_evaluator.evaluate_position();
    leaf_nodes_visited++;
    return eval;
  }

  std::vector<Move> possible_moves = calculate_possible_moves();
  int eval;
  if (maximise) {
    int max_eval = -INF;
    int best_move_index = 0;
    if (entry_best_move >= 0 && entry_best_move < possible_moves.size()) {
      max_search(alpha, beta, max_eval, eval, depth, best_move_index,
                 entry_best_move, possible_moves);
    }
    for (int index = 0; index < possible_moves.size(); ++index) {
      max_search(alpha, beta, max_eval, eval, depth, best_move_index, index,
                 possible_moves);
      if (alpha >= beta) {
        break;
      }
    }
    transposition_table.store(hash, iterative_depth_search, max_eval,
                              (max_eval >= beta) ? 1 : 0, best_move_index);
    return max_eval;
  } else {
    int min_eval = INF;
    int best_move_index = 0;
    if (entry_best_move >= 0 && entry_best_move < possible_moves.size()) {
      min_search(alpha, beta, min_eval, eval, depth, best_move_index,
                 entry_best_move, possible_moves);
    }
    for (int index = 0; index < possible_moves.size(); ++index) {
      min_search(alpha, beta, min_eval, eval, depth, best_move_index, index,
                 possible_moves);
      if (alpha >= beta) {
        break;
      }
    }
    transposition_table.store(hash, iterative_depth_search, min_eval,
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

void SearchEngine::max_search(int &alpha, int &beta, int &max_eval, int &eval,
                              int &depth, int &best_move_index, int &move_index,
                              std::vector<Move> &possible_moves) {
  board_state.apply_move(possible_moves[move_index]);
  eval = minimax_alpha_beta_search(alpha, beta, depth - 1, false);
  if (eval > max_eval) {
    max_eval = eval;
    best_move_index = move_index;
  }
  max_eval = std::max(max_eval, eval);
  alpha = std::max(eval, alpha);
  board_state.undo_move();
}

void SearchEngine::min_search(int &alpha, int &beta, int &min_eval, int &eval,
                              int &depth, int &best_move_index, int &move_index,
                              std::vector<Move> &possible_moves) {
  board_state.apply_move(possible_moves[move_index]);
  eval = minimax_alpha_beta_search(alpha, beta, depth - 1, true);
  if (eval < min_eval) {
    min_eval = eval;
    best_move_index = move_index;
  }
  beta = std::min(eval, beta);
  board_state.undo_move();
}