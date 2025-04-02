#ifndef FEN_INTERFACE_H
#define FEN_INTERFACE_H

#include <string>

#include "board_state.h"
#include "piece.h"

namespace engine::parts::fen_interface
{
/**
 * @brief Sets up the given board state from a FEN string.
 *
 * @param board_state The board state to setup.
 * @param fen_configuration The custom board configuration in Forsyth-Edwards
 * Notation (FEN)
 *
 * @details The board configuration is represented using Forsyth-Edwards
 * Notation (FEN). The FEN string consists of six fields separated by spaces:
 *
 * Field 1. Piece placement (from White's perspective). Each rank is described,
 * starting with rank 8 and ending with rank 1. Within each rank, the contents
 * of each square are described from file 'a' to file 'h'. Each rank is
 * separated by a '/'.
 *
 * The piece characters are:
 * K = White King
 * Q = White Queen
 * R = White Rook
 * B = White Bishop
 * N = White Knight
 * P = White Pawn
 * k = Black King
 * q = Black Queen
 * r = Black Rook
 * b = Black Bishop
 * n = Black Knight
 * p = Black Pawn
 *
 * For Empty Squares, a number 1-8 is used to represent the number of
 * consecutive empty squares.
 *
 * Field 2. Active color. 'w' means White to move, 'b' means Black to move.
 *
 * Field 3. Castling availability. If neither side can castle, this is "-".
 * Otherwise, this has one or more letters: 'K' (White can castle kingside),
 *    'Q' (White can castle queenside), 'k' (Black can castle kingside),
 * and/or 'q' (Black can castle queenside).
 *
 * Field 4. En passant target square in algebraic notation. If there's no en
 * passant target square, this is "-".
 *
 * Field 5. Halfmove clock. This is the number of halfmoves since the last pawn
 * advance or capture.
 *
 * Field 6. Fullmove number. The number of the full move. It starts at 1 and is
 * incremented after Black's move.
 *
 * The default chess board configuration in FEN would be given as:
 * 'rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1'
 *
 * @return True if the given board configuration is valid, false otherwise.
 */
auto setup_custom_board(BoardState &board_state,
                        const std::string &fen_configuration) -> bool;

/**
 * @brief Initializes the chess board with the given board_configuration in FEN.
 *
 * @param board_state The board state to setup.
 * @param board_configuration The custom board in FEN.
 */
auto initialize_board(BoardState &board_state,
                      std::string board_configuration) -> bool;

/**
 * @brief Worker function for setup_custom_board. Creates the pieces and
 * places them on the board.
 *
 * @param board_state The board state to setup.
 * @param piece_color The color of the piece to place.
 * @param piece_type The type of the piece to place.
 * @param x_file The x position of the piece to place.
 * @param y_rank The y position of the piece to place.
 */
static void create_pieces(BoardState &board_state,
                          const PieceColor &piece_color,
                          const PieceType &piece_type,
                          const int &x_file,
                          const int &y_rank);

/**
 * @brief Validates the castling rights string.
 *
 * @param board_state The board state to check.
 * @param castling_rights The castling rights string to validate.
 *
 * @note Chess board must already be setup with the custom board before this
 * is called.
 *
 * @return True if the castling rights string is valid, false otherwise.
 */
static auto
validate_castling_rights(BoardState &board_state,
                         const std::string &castling_rights) -> bool;

/**
 * @brief Validates the white king side castle.
 *
 * @param board_state The board state to check.
 */
static auto validate_white_king_side_castle(BoardState &board_state) -> bool;

/**
 * @brief Validates the white queen side castle.
 *
 * @param board_state The board state to check.
 */
static auto validate_white_queen_side_castle(BoardState &board_state) -> bool;

/**
 * @brief Validates the black king side castle.
 *
 * @param board_state The board state to check.
 */
static auto validate_black_king_side_castle(BoardState &board_state) -> bool;

/**
 * @brief Validates the black queen side castle.
 *
 * @param board_state The board state to check.
 */
static auto validate_black_queen_side_castle(BoardState &board_state) -> bool;

/**
 * @brief Validates the en passant target square string.
 *
 * @param board_state The board state to check.
 * @param en_passant_target The en passant target square string to validate.
 *
 * @note Chess board must already be setup with the custom board before this
 * is called.
 *
 * @return True if the en passant target square string is valid, false
 * otherwise.
 */
static auto
validate_en_passant_target(BoardState &board_state,
                           const std::string &en_passant_target) -> bool;

} // namespace engine::parts::fen_interface

#endif // FEN_INTERFACE_H