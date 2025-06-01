#include <algorithm>

#include <chess.hpp>

#include "chess_game.hpp"

ChessGame::ChessGame(std::string_view fen)
{
    board = chess::Board(fen);
}

bool ChessGame::is_move_valid(chess::Move move)
{
    if (std::find(moves.begin(), moves.end(), move) != moves.end())
        return true;
    else
        return false;
}

void ChessGame::make_move(chess::Move move)
{
    board.makeMove(move);
}
