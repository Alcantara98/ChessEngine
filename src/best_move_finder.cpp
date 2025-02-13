#include "best_move_finder.h"

// PRIVATE FUNTIONS
int BestMoveFinder::minimax_alpha_beta_search(int alpha, int beta, int depth,
                                              bool maximise) {
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
    return min_eval;
  }
}

// PUBLIC FUNCTIONS
BestMoveFinder::BestMoveFinder(BoardState &board_state)
    : board_state(board_state),
      position_evaluator(PositionEvaluator(board_state)) {}

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
  std::vector<int> move_scores;
  bool maximising = engine_color == PieceColor::WHITE;
  for (Move move : possible_moves) {
    board_state.apply_move(move);
    move_scores.push_back(
        minimax_alpha_beta_search(-INF, INF, max_search_depth, !maximising));
    board_state.undo_move();
  }
  int best_score = move_scores[0];
  int best_index = 0;
  if (engine_color == PieceColor::WHITE) {
    for (int i = 1; i < move_scores.size(); ++i) {
      if (move_scores[i] > best_score) {
        best_score = move_scores[i];
        best_index = i;
      }
    }
  } else {
    for (int i = 1; i < move_scores.size(); ++i) {
      if (move_scores[i] < best_score) {
        best_score = move_scores[i];
        best_index = i;
      }
    }
  }
  return possible_moves[best_index];
}