#pragma once

#include <string_view>

#include <chess.hpp>

class ChessGame {
    public:
        chess::Board board;
        chess::Movelist moves;

        ChessGame(std::string_view fen = chess::constants::STARTPOS);

        bool is_move_valid(chess::Move move);

        void make_move(chess::Move move);

        chess::Square selected_square = chess::Square::NO_SQ;
        chess::Square selecting_square = chess::Square::SQ_A1;

        void set_selected_square(chess::Square square);
        void set_selecting_square(chess::Square square);

        chess::Square move_selecting_square(chess::Direction direction);
};
