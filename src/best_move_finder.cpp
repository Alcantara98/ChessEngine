#include "best_move_finder.h"
#include "transposition_table.h"

// PRIVATE FUNCTIONS
int BestMoveFinder::minimax_alpha_beta_search(int alpha, int beta, int depth,
                                              bool maximise) {
  int tt_value, tt_flag;
  if (transposition_table.retrieve(board_state, depth, tt_value, tt_flag)) {
    if (tt_flag == 0)
      return tt_value;
    if (tt_flag == -1 && tt_value <= alpha)
      return tt_value;
    if (tt_flag == 1 && tt_value >= beta)
      return tt_value;
  }

  if (depth == 0) {
    int eval = position_evaluator.evaluate_position();
    return eval;
  }

  std::vector<Move> possible_moves = calculate_possible_moves();
  int eval;
  if (maximise) {
    int max_eval = -INF;
    for (Move move : possible_moves) {
      board_state.apply_move(move);
      eval = minimax_alpha_beta_search(alpha, beta, depth - 1, false);
      max_eval = std::max(max_eval, eval);
      alpha = std::max(eval, alpha);
      board_state.undo_move();
      if (alpha >= beta) {
        break;
      }
    }
    transposition_table.store(board_state, depth, max_eval,
                              (max_eval >= beta) ? 1 : 0);
    return max_eval;
  } else {
    int min_eval = INF;
    for (Move move : possible_moves) {
      board_state.apply_move(move);
      eval = minimax_alpha_beta_search(alpha, beta, depth - 1, true);
      min_eval = std::min(min_eval, eval);
      beta = std::min(eval, beta);
      board_state.undo_move();
      if (alpha >= beta) {
        break;
      }
    }
    transposition_table.store(board_state, depth, min_eval,
                              (min_eval <= alpha) ? -1 : 0);
    return min_eval;
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
  for (Move move : possible_moves) {
    board_state.apply_move(move);
    move_scores.push_back(
        {move, minimax_alpha_beta_search(-INF, INF, 4, !maximising)});
    board_state.undo_move();
  }

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

  std::vector<std::pair<Move, int>> pruned_move_scores;
  for (int i = 0; i < 4; ++i) {
    Move &move = move_scores[i].first;
    board_state.apply_move(move);
    pruned_move_scores.push_back(
        {move,
         minimax_alpha_beta_search(-INF, INF, max_search_depth, !maximising)});
    board_state.undo_move();
  }

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
  return pruned_move_scores[0].first;
}
