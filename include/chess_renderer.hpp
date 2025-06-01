#pragma once

#include <chess.hpp>

#include "chess_game.hpp"
#include "gpu.hpp"
#include "object.hpp"

#define SQUARE 0
#define PIECE 1
#define TABLE 2

#define LIGHT 0
#define DARK 1

#define NONE 0
#define SELECTING 1
#define SELECTED 2
#define LAST_MOVE 3
#define AVAILABLE_MOVE 4
#define AVAILABLE_CAPTURE 5
#define CHECK 6

class ChessRenderer {
    public:
        ChessGame& game;

        ObjModel square_model;
        Object* square_objects[64];

        chess::Square selected_square = chess::Square::NO_SQ;
        chess::Square selecting_square = chess::Square::SQ_A1;

        ChessRenderer(ChessGame& game, GpuProgram);

        void draw();

        void set_selected_square(chess::Square square);
        void set_selecting_square(chess::Square square);

        void move_selecting_square(chess::Direction direction);

    private:
        GpuProgram gpu_program;

        GLint model_uniform;
        GLint object_id_uniform;
        GLint color_uniform;
        GLint square_state_uniform;

        void reset_square_state(chess::Square square);
};
