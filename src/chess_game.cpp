#include <algorithm>

#include <chess.hpp>

#include "chess_game.hpp"

ChessGame::ChessGame(std::string_view fen)
{
    board = chess::Board(fen);
    chess::movegen::legalmoves(moves, board);
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

void ChessGame::set_selecting_square(chess::Square square)
{
    selecting_square = square;
}

void ChessGame::set_selected_square(chess::Square square)
{
    selected_square = square;
}

void ChessGame::set_piece_to_move(chess::Piece piece)
{
    current_piece_to_move = piece;
}

void ChessGame::set_origin_square(chess::Square square)
{
    origin_square = square;
}

void ChessGame::set_next_move(chess::Move move)
{
    ongoing_move = move;
}

chess::Square ChessGame::move_selecting_square(chess::Direction direction)
{
    if (selecting_square == chess::Square::NO_SQ) {
        set_selecting_square(chess::Square::SQ_E4);
        return chess::Square::SQ_E4;;
    }

    chess::Rank new_rank = selecting_square.rank() + (int)direction / 8;
    chess::File new_file = selecting_square.file() + (int)direction % 8;

    if (chess::Square::is_valid(new_rank, new_file)){
        chess::Square new_square(new_rank, new_file);
        set_selecting_square(new_square);
        return new_square;
    }
    else {
        return selecting_square;
    }
}

bool ChessGame::is_game_over()
{
    chess::GameResult result = board.isGameOver().second;

    switch (result) {
        case chess::GameResult::WIN:
            winner = board.sideToMove();
            break;
        case chess::GameResult::LOSE:
            winner = !board.sideToMove();
            break;
        case chess::GameResult::DRAW:
            winner = chess::Color::NONE;
            break;
        default:
            return false;
    }

    return true;
}
