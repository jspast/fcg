#include "chess_renderer.hpp"
#include "chess.hpp"
#include "chess_game.hpp"
#include "gpu.hpp"
#include "object.hpp"

void ChessRenderer::reset_square_state(chess::Square s)
{
    square_objects[s.index()]->set_uniform("square_state", NONE);
}

ChessRenderer::ChessRenderer(ChessGame& game, GpuProgram gpu) :
               game(game), square_model("../../data/models/square.obj")
{
    model_uniform = gpu.get_uniform_location("model");
    object_id_uniform = gpu.get_uniform_location("object_id");
    color_uniform = gpu.get_uniform_location("object_color");
    square_state_uniform = gpu.get_uniform_location("square_state");

    square_model.compute_normals();
    square_model.build_triangles();

    for (int i = 0; i < 64; i++) {
        square_objects[i] = new Object(square_model,gpu_program);
        square_objects[i]->set_transform(Matrix_Translate((int)(i / 8), 0, i % 8));
        square_objects[i]->set_uniform("object_id", SQUARE);
        square_objects[i]->set_uniform("object_color", chess::Square(i).is_light() ? LIGHT : DARK);
        square_objects[i]->set_uniform("square_state", NONE);
    }
}

void ChessRenderer::draw()
{
    if (selected_square != chess::Square::NO_SQ)
        square_objects[selected_square.index()]->set_uniform("square_state", SELECTED);

    if (selecting_square != chess::Square::NO_SQ)
        square_objects[selecting_square.index()]->set_uniform("square_state", SELECTING);

    for (int i = 0; i < 64; i++)
        square_objects[i]->draw();
}

void ChessRenderer::set_selected_square(chess::Square square)
{
    if (selected_square != chess::Square::NO_SQ)
        reset_square_state(selected_square);

    selected_square = square;
}

void ChessRenderer::set_selecting_square(chess::Square square)
{
    if (selecting_square != chess::Square::NO_SQ)
        reset_square_state(selecting_square);

    selecting_square = square;
}

void ChessRenderer::move_selecting_square(chess::Direction direction)
{
    chess::Rank new_rank = selecting_square.rank() + (int)direction / 8;
    chess::File new_file = selecting_square.file() + (int)direction % 8;

    if (chess::Square::is_valid(new_rank, new_file))
        set_selecting_square(chess::Square(new_rank, new_file));
}

