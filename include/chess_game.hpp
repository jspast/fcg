#pragma once

#include <string_view>

#include <chess.hpp>

class ChessGame {
    public:
        chess::Board board;
        chess::Movelist moves;

        enum class IngameState { 
            SELECTING_SQUARES,
            ONGOING_MOVE
        };

        IngameState current_state = IngameState::SELECTING_SQUARES;

        ChessGame(std::string_view fen = chess::constants::STARTPOS);

        bool is_move_valid(chess::Move move);
        void make_move(chess::Move move);

        chess::Square selecting_square = chess::Square::SQ_A1;
        chess::Square selected_square = chess::Square::NO_SQ;
        chess::Piece current_piece_to_move = chess::Piece::NONE;
        chess::Square origin_square = chess::Square::NO_SQ;

        chess::Move ongoing_move = chess::Move::NULL_MOVE;

        void set_selecting_square(chess::Square square);
        void set_selected_square(chess::Square square);
        void set_piece_to_move(chess::Piece piece);
        void set_origin_square(chess::Square square);

        void set_next_move(chess::Move move);

        chess::Square move_selecting_square(chess::Direction direction);
};
