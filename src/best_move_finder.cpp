#include "best_move_finder.h"
#include "transposition_table.h"

// PRIVATE FUNCTIONS
int BestMoveFinder::minimax_alpha_beta_search(int alpha, int beta, int depth,
                                              bool maximise) {
  int tt_value, tt_flag, entry_depth;
  int entry_best_move = -1;
  uint64_t hash = board_state.compute_zobrist_hash();
  if (transposition_table.retrieve(hash, entry_depth, tt_value, tt_flag,
                                   entry_best_move)) {
    if (entry_depth == iterative_depth_search) {
      if (tt_flag == 0)
        return tt_value;
      if (tt_flag == -1 && tt_value <= alpha)
        return tt_value;
      if (tt_flag == 1 && tt_value >= beta)
        return tt_value;
    }
  }

  if (depth == 0) {
    int eval = position_evaluator.evaluate_position();
    return eval;
  }

  std::vector<Move> possible_moves = calculate_possible_moves();
  int eval;
  if (maximise) {
    int max_eval = -INF;
    int best_move_index = 0;
    if (entry_best_move >= 0 && entry_best_move < possible_moves.size()) {
      board_state.apply_move(possible_moves[entry_best_move]);
      eval = minimax_alpha_beta_search(alpha, beta, depth - 1, false);
      if (eval > max_eval) {
        max_eval = eval;
        best_move_index = entry_best_move;
      }
      max_eval = std::max(max_eval, eval);
      alpha = std::max(eval, alpha);
      board_state.undo_move();
    }
    for (int index = 0; index < possible_moves.size(); ++index) {
      board_state.apply_move(possible_moves[index]);
      eval = minimax_alpha_beta_search(alpha, beta, depth - 1, false);
      if (eval > max_eval) {
        max_eval = eval;
        best_move_index = index;
      }
      max_eval = std::max(max_eval, eval);
      alpha = std::max(eval, alpha);
      board_state.undo_move();
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
      board_state.apply_move(possible_moves[entry_best_move]);
      eval = minimax_alpha_beta_search(alpha, beta, depth - 1, true);
      if (eval < min_eval) {
        min_eval = eval;
        best_move_index = entry_best_move;
      }
      beta = std::min(eval, beta);
      board_state.undo_move();
    }
    for (int index = 0; index < possible_moves.size(); ++index) {
      board_state.apply_move(possible_moves[index]);
      eval = minimax_alpha_beta_search(alpha, beta, depth - 1, true);
      if (eval < min_eval) {
        min_eval = eval;
        best_move_index = index;
      }
      beta = std::min(eval, beta);
      board_state.undo_move();
      if (alpha >= beta) {
        break;
      }
    }
    transposition_table.store(hash, iterative_depth_search, min_eval,
                              (min_eval <= alpha) ? -1 : 0, best_move_index);
    return min_eval;
  }
}

void BestMoveFinder::sort_moves(
    std::vector<std::pair<Move, int>> &move_scores) {
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

// PUBLIC FUNCTIONS
BestMoveFinder::BestMoveFinder(BoardState &board_state)
    : board_state(board_state),
      position_evaluator(PositionEvaluator(board_state)),
      transposition_table(1000000) {} // Initialize with a max size

std::vector<Move> BestMoveFinder::calculate_possible_moves() {
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

Move BestMoveFinder::find_best_move(int max_search_depth) {
  std::vector<Move> possible_moves = calculate_possible_moves();
  std::vector<std::pair<Move, int>> move_scores;
  bool maximising = engine_color == PieceColor::WHITE;
  iterative_depth_search = 4;
  int first_search_depth = max_search_depth >= 4 ? 4 : max_search_depth;

  // Search to depth 4 to get initial move scores, then sort.
  for (Move move : possible_moves) {
    board_state.apply_move(move);
    move_scores.push_back(
        {move, minimax_alpha_beta_search(-INF, INF, first_search_depth, !maximising)});
    board_state.undo_move();
  }
  sort_moves(move_scores);

  // Prune to 8 moves.
  std::vector<std::pair<Move, int>> pruned_move_scores(move_scores.begin(),
                                                       move_scores.begin() + 8);

  // Search to max_search_depth with iterative deepening.
  for (int iterative_depth = 5; iterative_depth <= max_search_depth;
       ++iterative_depth) {
    iterative_depth_search = iterative_depth;
    for (int move_index = 0; move_index < 8; ++move_index) {
      Move &move = pruned_move_scores[move_index].first;
      board_state.apply_move(move);
      pruned_move_scores[move_index].second =
          minimax_alpha_beta_search(-INF, INF, iterative_depth, !maximising);
      board_state.undo_move();
    }
  }
  sort_moves(pruned_move_scores);

  return pruned_move_scores[0].first;
}
