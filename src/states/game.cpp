#include <memory>
#include <set>
#include <vector>

#include <chess.hpp>

#include "chess_game.hpp"
#include "states/game.hpp"
#include "camera.hpp"
#include "input.hpp"
#include "object.hpp"
#include "gpu.hpp"
#include "collisions.hpp"

void GameplayState::load()
{
    lookat_camera = std::make_shared<LookAtCamera>();
    free_camera = std::make_shared<FreeCamera>();
    camera = lookat_camera;
    window->set_user_pointer(camera.get());

    lookat_camera->set_target_position(0.0f, 1.0f, 0.0f);
    lookat_camera->set_distance(2.0f);

    chess_game = std::make_unique<ChessGame>();

    game_input = std::make_unique<InputManager>(
        window->glfw_window,
        std::vector<int> {
            GLFW_KEY_W,
            GLFW_KEY_A,
            GLFW_KEY_S,
            GLFW_KEY_D,
            GLFW_KEY_L,
            GLFW_KEY_F,
            GLFW_KEY_P,
            GLFW_KEY_O,
            GLFW_KEY_UP,
            GLFW_KEY_LEFT,
            GLFW_KEY_DOWN,
            GLFW_KEY_RIGHT,
            GLFW_KEY_ENTER
        },
        std::vector<int> {},
        std::set<int> {},
        std::set<int> {
            GLFW_GAMEPAD_AXIS_LEFT_X,
            GLFW_GAMEPAD_AXIS_LEFT_Y,
            GLFW_GAMEPAD_AXIS_RIGHT_X,
            GLFW_GAMEPAD_AXIS_RIGHT_Y,
            GLFW_GAMEPAD_AXIS_LEFT_TRIGGER,
            GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER
        }
    );

    input = std::make_unique<InputManager>(
        window->glfw_window,
        std::vector<int> {
            GLFW_KEY_F3,
            GLFW_KEY_ESCAPE
        },
        std::vector<int> {},
        std::set<int> {
            GLFW_GAMEPAD_BUTTON_START
        },
        std::set<int> {}
    );


    hud = std::make_unique<Hud>(window->glfw_window, &camera);

    sky_model    = std::make_shared<ObjModel>("../../data/models/cube.obj");
    floor_model  = std::make_shared<ObjModel>("../../data/models/plane.obj");
    table_model  = std::make_shared<ObjModel>("../../data/models/table.obj");
    board_model  = std::make_shared<ObjModel>("../../data/models/board.obj");

    pawn_model   = std::make_shared<ObjModel>("../../data/models/pawn.obj");
    king_model   = std::make_shared<ObjModel>("../../data/models/king.obj");
    rook_model   = std::make_shared<ObjModel>("../../data/models/rook.obj");
    knight_model = std::make_shared<ObjModel>("../../data/models/knight.obj");
    queen_model  = std::make_shared<ObjModel>("../../data/models/queen.obj");
    bishop_model = std::make_shared<ObjModel>("../../data/models/bishop.obj");

    sky    = std::make_shared<Object>(sky_model,    *gpu_program);
    floor  = std::make_shared<Object>(floor_model,  *gpu_program);
    table  = std::make_shared<Object>(table_model,  *gpu_program);
    board  = std::make_shared<Object>(board_model,  *gpu_program);

    pawn   = std::make_shared<Object>(pawn_model,   *gpu_program);
    king   = std::make_shared<Object>(king_model,   *gpu_program);
    rook   = std::make_shared<Object>(rook_model,   *gpu_program);
    knight = std::make_shared<Object>(knight_model, *gpu_program);
    queen  = std::make_shared<Object>(queen_model,  *gpu_program);
    bishop = std::make_shared<Object>(bishop_model, *gpu_program);

    sky->set_uniform("object_id", SKY);
    floor->set_uniform("object_id", FLOOR);
    table->set_uniform("object_id", TABLE);
    board->set_uniform("object_id", BOARD);

    pawn->set_uniform("object_id", PIECE);
    pawn->set_uniform("piece_color", PIECE_BLACK);
    king->set_uniform("object_id", PIECE);
    king->set_uniform("piece_color", PIECE_WHITE);

    floor->set_transform(Matrix_Scale(100.0f, 1.0f, 100.0f));
    board->set_transform(Matrix_Translate(0.0f, table->model->aabb.max_y, 0.0f) *
                         Matrix_Scale(1.5f, 1.5f, 1.5f));

    pawn->set_transform(Matrix_Translate(BOARD_START + SQUARE_SIZE / 2.0, 0.0f, BOARD_START + SQUARE_SIZE / 2.0));
    king->set_transform(Matrix_Translate(-BOARD_START - SQUARE_SIZE / 2.0, 0.0f, -BOARD_START - SQUARE_SIZE / 2.0));

    board->set_uniform("selecting_square", glm::vec2(0, 0));

    pawn->set_instances(8);

    board->add_child(pawn);
    board->add_child(king);
    table->add_child(board);

    // Enable Z-buffer
    glEnable(GL_DEPTH_TEST);

    // Enable backface culling
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    // Enable transparent objects
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void GameplayState::unload() {}

void GameplayState::update(float delta_t)
{
    glm::vec4 col;

    if (!game_input->get_is_enabled()) {

        glm::vec4 ray = cursor_to_ray(input->get_cursor_position(),
                                      window->get_size(),
                                      camera->get_projection_matrix(),
                                      camera->get_view_matrix());

        col = ray_plane_intersection(camera->get_position(), ray,
            glm::vec4(0, table_model->aabb.max_y + board_model->aabb.max_y * 1.5f, 0, 1),
            glm::vec4(0, 1, 0, 0));

        if (col.x > G_BOARD_START &&
            col.x < G_BOARD_START + 8 * G_SQUARE_SIZE &&
            col.z > G_BOARD_START &&
            col.z < G_BOARD_START + 8 * G_SQUARE_SIZE) {

            chess::File file = 8 - (col.x - G_BOARD_START) / G_SQUARE_SIZE;
            chess::Rank rank = (col.z - G_BOARD_START) / G_SQUARE_SIZE;

            chess::Square new_square(file, rank);

            chess_game->selecting_square = new_square;
            board->set_uniform("selecting_square", glm::vec2(file, rank));
        }
    }

    if (input->get_is_key_pressed(GLFW_KEY_ESCAPE) ||
        input->get_is_gamepad_button_pressed(GLFW_JOYSTICK_1, GLFW_GAMEPAD_BUTTON_START))
    {
        game_input->set_is_enabled(!game_input->get_is_enabled());
        window->toggle_cursor();
    }

    if (input->get_is_key_pressed(GLFW_KEY_F3))
        hud->toggle_debug_info();

    if (game_input->get_is_key_pressed(GLFW_KEY_L)) {
        lookat_camera = build_lookat_camera(camera, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), 2.0f);
        camera = lookat_camera;
        window->set_user_pointer(camera.get());
    }

    if (game_input->get_is_key_pressed(GLFW_KEY_F)) {
        free_camera = build_free_camera(camera);
        camera = free_camera;
        window->set_user_pointer(camera.get());
    }

    if (game_input->get_is_key_pressed(GLFW_KEY_UP)) {
        chess::Square new_square = chess_game->move_selecting_square(chess::Direction::NORTH);
        if (new_square != chess::Square::NO_SQ)
            board->set_uniform("selecting_square", glm::vec2(new_square.file(), new_square.rank()));
    }

    if (game_input->get_is_key_pressed(GLFW_KEY_DOWN)) {
        chess::Square new_square = chess_game->move_selecting_square(chess::Direction::SOUTH);
        if (new_square != chess::Square::NO_SQ) {
            board->set_uniform("selecting_square", glm::vec2(new_square.file(), new_square.rank()));
        }
    }

    if (game_input->get_is_key_pressed(GLFW_KEY_LEFT)) {
        chess::Square new_square = chess_game->move_selecting_square(chess::Direction::WEST);
        if (new_square != chess::Square::NO_SQ) {
            board->set_uniform("selecting_square", glm::vec2(new_square.file(), new_square.rank()));
        }
    }

    if (game_input->get_is_key_pressed(GLFW_KEY_RIGHT)) {
        chess::Square new_square = chess_game->move_selecting_square(chess::Direction::EAST);
        if (new_square != chess::Square::NO_SQ) {
            board->set_uniform("selecting_square", glm::vec2(new_square.file(), new_square.rank()));
        }
    }

    if (game_input->get_is_key_pressed(GLFW_KEY_P))
        camera->toggle_perspective_projection(true);

    if (game_input->get_is_key_pressed(GLFW_KEY_O))
        camera->toggle_perspective_projection(false);

    if (game_input->get_is_key_down(GLFW_KEY_W))
        free_camera->move(0.5f * delta_t, 0.0f);

    if (game_input->get_is_key_down(GLFW_KEY_S))
        free_camera->move(-0.5f * delta_t, 0.0f);

    if (game_input->get_is_key_down(GLFW_KEY_A))
        free_camera->move(0.0f, 0.5f * delta_t);

    if (game_input->get_is_key_down(GLFW_KEY_D))
        free_camera->move(0.0f, -0.5f * delta_t);

    // Atualizamos parâmetros da câmera com os deslocamentos
    glm::vec2 cursor_movement = game_input->get_cursor_movement();
    camera->adjust_angles(-0.001f * cursor_movement.x, 0.001f * cursor_movement.y);

    glm::vec2 scroll = game_input->get_scroll_offset();
    camera->adjust_orthographic_zoom(0.1 * scroll.y);
    lookat_camera->adjust_distance(-0.1 * scroll.y);

    free_camera->move(-delta_t * game_input->get_gamepad_axis_value(GLFW_JOYSTICK_1,
                                                                    GLFW_GAMEPAD_AXIS_LEFT_Y),
                        -delta_t * game_input->get_gamepad_axis_value(GLFW_JOYSTICK_1,
                                                                      GLFW_GAMEPAD_AXIS_LEFT_X));

    camera->adjust_angles(-delta_t * game_input->get_gamepad_axis_value(GLFW_JOYSTICK_1,
                                                                        GLFW_GAMEPAD_AXIS_RIGHT_X),
                            delta_t * game_input->get_gamepad_axis_value(GLFW_JOYSTICK_1,
                                                                         GLFW_GAMEPAD_AXIS_RIGHT_Y));

    lookat_camera->adjust_distance(delta_t * (game_input->get_gamepad_axis_value(GLFW_JOYSTICK_1,
                                                                                 GLFW_GAMEPAD_AXIS_LEFT_TRIGGER) -
                                                game_input->get_gamepad_axis_value(GLFW_JOYSTICK_1,
                                                                                   GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER)));

    game_input->update();
    input->update();

    sky->set_transform(Matrix_Translate(camera->get_position().x - 0.5,
                                        camera->get_position().y - 0.5,
                                        camera->get_position().z - 0.5));

    gpu_program->set_uniform("view", camera->get_view_matrix());
    gpu_program->set_uniform("projection", camera->get_projection_matrix());

    hud->update(input->get_cursor_position(), col);
}

void GameplayState::draw()
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    sky->draw();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    floor->draw();

    table->draw();

    hud->draw();
}
