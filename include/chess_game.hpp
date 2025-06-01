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
};
