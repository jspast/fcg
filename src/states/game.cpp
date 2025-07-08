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
        std::vector<int> {
            GLFW_MOUSE_BUTTON_LEFT
        },
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

    // Os Objects compartilham uniforms, podendo ter múltiplas instâncias 
    // criadas através de múltiplas matrizes de transformação
    white_pawn   = std::make_shared<Object>(pawn_model,   *gpu_program);
    white_king   = std::make_shared<Object>(king_model,   *gpu_program);
    white_rook   = std::make_shared<Object>(rook_model,   *gpu_program);
    white_knight = std::make_shared<Object>(knight_model, *gpu_program);
    white_queen  = std::make_shared<Object>(queen_model,  *gpu_program);
    white_bishop = std::make_shared<Object>(bishop_model, *gpu_program);
    black_pawn   = std::make_shared<Object>(pawn_model,   *gpu_program);
    black_king   = std::make_shared<Object>(king_model,   *gpu_program);
    black_rook   = std::make_shared<Object>(rook_model,   *gpu_program);
    black_knight = std::make_shared<Object>(knight_model, *gpu_program);
    black_queen  = std::make_shared<Object>(queen_model,  *gpu_program);
    black_bishop = std::make_shared<Object>(bishop_model, *gpu_program);

    sky->set_uniform("object_id", SKY);
    floor->set_uniform("object_id", FLOOR);
    table->set_uniform("object_id", TABLE);
    board->set_uniform("object_id", BOARD);

    white_pawn->set_uniform("object_id", PIECE);
    white_pawn->set_uniform("piece_color", PIECE_WHITE);
    white_king->set_uniform("object_id", PIECE);
    white_king->set_uniform("piece_color", PIECE_WHITE);
    white_rook->set_uniform("object_id", PIECE);
    white_rook->set_uniform("piece_color", PIECE_WHITE);
    white_knight->set_uniform("object_id", PIECE);
    white_knight->set_uniform("piece_color", PIECE_WHITE);
    white_queen->set_uniform("object_id", PIECE);
    white_queen->set_uniform("piece_color", PIECE_WHITE);
    white_bishop->set_uniform("object_id", PIECE);
    white_bishop->set_uniform("piece_color", PIECE_WHITE);
    black_pawn->set_uniform("object_id", PIECE);
    black_pawn->set_uniform("piece_color", PIECE_BLACK);
    black_king->set_uniform("object_id", PIECE);
    black_king->set_uniform("piece_color", PIECE_BLACK);
    black_rook->set_uniform("object_id", PIECE);
    black_rook->set_uniform("piece_color", PIECE_BLACK);
    black_knight->set_uniform("object_id", PIECE);
    black_knight->set_uniform("piece_color", PIECE_BLACK);
    black_queen->set_uniform("object_id", PIECE);
    black_queen->set_uniform("piece_color", PIECE_BLACK);
    black_bishop->set_uniform("object_id", PIECE);
    black_bishop->set_uniform("piece_color", PIECE_BLACK);

    // Definimos as posições dos objetos
    floor->set_transform(0, Matrix_Scale(100.0f, 1.0f, 100.0f));
    board->set_transform(0, Matrix_Translate(0.0f, table->model->aabb.max.y, 0.0f) *
                         Matrix_Scale(1.5f, 1.5f, 1.5f));

    // Definimos as instâncias e as posições iniciais das peças
    float board_left = BOARD_START + SQUARE_SIZE / 2.0;
    float board_top = BOARD_START + SQUARE_SIZE / 2.0;
    float board_bottom = -BOARD_START - SQUARE_SIZE / 2.0;
    
    white_king->set_transform(0, Matrix_Translate(board_left + SQUARE_SIZE*3, 0.0f, board_top));
    white_queen->set_transform(0, Matrix_Translate(board_left + SQUARE_SIZE*4, 0.0f, board_top));
    white_rook->set_transform(0, Matrix_Translate(board_left, 0.0f, board_top));
    white_rook->add_instance(Matrix_Translate(board_left + SQUARE_SIZE*7, 0.0f, board_top));
    white_knight->set_transform(0, Matrix_Translate(board_left + SQUARE_SIZE*1, 0.0f, board_top) * Matrix_Rotate_Y(M_PI));
    white_knight->add_instance(Matrix_Translate(board_left + SQUARE_SIZE*6, 0.0f, board_top) * Matrix_Rotate_Y(M_PI));
    white_bishop->set_transform(0, Matrix_Translate(board_left + SQUARE_SIZE*2, 0.0f, board_top));
    white_bishop->add_instance(Matrix_Translate(board_left + SQUARE_SIZE*5, 0.0f, board_top));
    white_pawn->set_transform(0, Matrix_Translate(board_left, 0.0f, board_top + SQUARE_SIZE));
    for (int i=1; i<8; i++) {
        white_pawn->add_instance(Matrix_Translate(board_left + SQUARE_SIZE*i, 0.0f, board_top + SQUARE_SIZE));
    }

    black_king->set_transform(0, Matrix_Translate(board_left + SQUARE_SIZE*3, 0.0f, board_bottom));
    black_queen->set_transform(0, Matrix_Translate(board_left + SQUARE_SIZE*4, 0.0f, board_bottom));
    black_rook->set_transform(0, Matrix_Translate(board_left, 0.0f, board_bottom));
    black_rook->add_instance(Matrix_Translate(board_left + SQUARE_SIZE*7, 0.0f, board_bottom));
    black_knight->set_transform(0, Matrix_Translate(board_left + SQUARE_SIZE*1, 0.0f, board_bottom));
    black_knight->add_instance(Matrix_Translate(board_left + SQUARE_SIZE*6, 0.0f, board_bottom));
    black_bishop->set_transform(0, Matrix_Translate(board_left + SQUARE_SIZE*2, 0.0f, board_bottom));
    black_bishop->add_instance(Matrix_Translate(board_left + SQUARE_SIZE*5, 0.0f, board_bottom));
    black_pawn->set_transform(0, Matrix_Translate(board_left, 0.0f, board_bottom - SQUARE_SIZE));
    for (int i=1; i<8; i++) {
        black_pawn->add_instance(Matrix_Translate(board_left + SQUARE_SIZE*i, 0.0f, board_bottom - SQUARE_SIZE));
    }

    board->set_uniform("selecting_square", glm::vec2(0, 0));

    aabbs = {
        std::pair(glm::vec4(0.0), floor_model->aabb * 100.0f),
        std::pair(glm::vec4(0.0), table_model->aabb),
        std::pair(glm::vec4(0.0, table_model->aabb.max.y, 0.0, 0.0), board_model->aabb * 1.5f)
    };

    board->add_child(white_pawn);
    board->add_child(white_king);
    board->add_child(white_rook);
    board->add_child(white_knight);
    board->add_child(white_queen);
    board->add_child(white_bishop);
    board->add_child(black_pawn);
    board->add_child(black_king);
    board->add_child(black_rook);
    board->add_child(black_knight);
    board->add_child(black_queen);
    board->add_child(black_bishop);
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

void GameplayState::process_inputs(float delta_t) 
{
    glm::vec4 col;

    // Identifica a casa apontada, enquanto não há uma animação em progresso
    if (!game_input->get_is_enabled() && 
        chess_game->current_state != ChessGame::IngameState::ONGOING_MOVE) {

        glm::vec4 ray = cursor_to_ray(input->get_cursor_position(),
                                      window->get_size(),
                                      camera->get_projection_matrix(),
                                      camera->get_view_matrix());

        col = ray_plane_intersection(camera->get_position(), ray,
            glm::vec4(0, table_model->aabb.max.y + board_model->aabb.max.y * 1.5f, 0, 1),
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

            // Identifica se a casa apontada foi selecionada com o mouse
            if (input->get_is_mouse_button_released(GLFW_MOUSE_BUTTON_LEFT)) {
                chess_game->selected_square = new_square;
                board->set_uniform("selected_square", glm::vec2(file, rank));
            }
        }
    }

    // Alterna entre estado de manipulação da câmera e estado de 
    // seleção de casa através da tecla ESC
    if (input->get_is_key_pressed(GLFW_KEY_ESCAPE) ||
        input->get_is_gamepad_button_pressed(GLFW_JOYSTICK_1, GLFW_GAMEPAD_BUTTON_START))
    {
        game_input->set_is_enabled(!game_input->get_is_enabled());
        window->toggle_cursor();
    }

    // Exibe ou oculta informações de depuração
    if (input->get_is_key_pressed(GLFW_KEY_F3))
        hud->toggle_debug_info();
    
    // Troca o tipo de câmera
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

    // Muda a seleção da casa atual de acordo com a perspectiva
    if (game_input->get_is_key_pressed(GLFW_KEY_DOWN)) {
        chess::Direction direction;
        if (chess_game->board.sideToMove() == chess::Color::WHITE) {
            direction = chess::Direction::SOUTH;
        } else {
            direction = chess::Direction::NORTH;
        }
        chess::Square new_square = chess_game->move_selecting_square(direction);
        if (new_square != chess::Square::NO_SQ)
            board->set_uniform("selecting_square", glm::vec2(new_square.file(), new_square.rank()));
    }
    if (game_input->get_is_key_pressed(GLFW_KEY_UP)) {
        chess::Direction direction;
        if (chess_game->board.sideToMove() == chess::Color::WHITE) {
            direction = chess::Direction::NORTH;
        } else {
            direction = chess::Direction::SOUTH;
        }
        chess::Square new_square = chess_game->move_selecting_square(direction);
        if (new_square != chess::Square::NO_SQ) {
            board->set_uniform("selecting_square", glm::vec2(new_square.file(), new_square.rank()));
        }
    }
    if (game_input->get_is_key_pressed(GLFW_KEY_RIGHT)) {
        chess::Direction direction;
        if (chess_game->board.sideToMove() == chess::Color::WHITE) {
            direction = chess::Direction::EAST;
        } else {
            direction = chess::Direction::WEST;
        }
        chess::Square new_square = chess_game->move_selecting_square(direction);
        if (new_square != chess::Square::NO_SQ) {
            board->set_uniform("selecting_square", glm::vec2(new_square.file(), new_square.rank()));
        }
    }
    if (game_input->get_is_key_pressed(GLFW_KEY_LEFT)) {
        chess::Direction direction;
        if (chess_game->board.sideToMove() == chess::Color::WHITE) {
            direction = chess::Direction::WEST;
        } else {
            direction = chess::Direction::EAST;
        }
        chess::Square new_square = chess_game->move_selecting_square(direction);
        if (new_square != chess::Square::NO_SQ) {
            board->set_uniform("selecting_square", glm::vec2(new_square.file(), new_square.rank()));
        }
    }

    // Muda o tipo de projeção
    if (game_input->get_is_key_pressed(GLFW_KEY_P))
        camera->toggle_perspective_projection(true);

    if (game_input->get_is_key_pressed(GLFW_KEY_O))
        camera->toggle_perspective_projection(false);

    if (game_input->get_is_key_down(GLFW_KEY_W) ||
        game_input->get_is_key_down(GLFW_KEY_A) ||
        game_input->get_is_key_down(GLFW_KEY_S) ||
        game_input->get_is_key_down(GLFW_KEY_D))
    {
        float front_movement = 0.0f;
        float left_movement = 0.0f;

        if (game_input->get_is_key_down(GLFW_KEY_W))
            front_movement += 0.5f * delta_t;

        if (game_input->get_is_key_down(GLFW_KEY_S))
            front_movement -= 0.5f * delta_t;

        if (game_input->get_is_key_down(GLFW_KEY_A))
            left_movement += 0.5f * delta_t;

        if (game_input->get_is_key_down(GLFW_KEY_D))
            left_movement -= 0.5f * delta_t;

        float factor = sphere_aabbs_intersection_with_movement(
            camera->get_position(),
            camera->get_nearplane_distance() * 2.0f,
            free_camera->w_vector * front_movement +
            free_camera->u_vector * left_movement,
            aabbs);

        free_camera->move(front_movement * factor, left_movement * factor);
    }

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

    sky->set_transform(0, Matrix_Translate(camera->get_position().x - 0.5,
                                        camera->get_position().y - 0.5,
                                        camera->get_position().z - 0.5));

    gpu_program->set_uniform("view", camera->get_view_matrix());
    gpu_program->set_uniform("projection", camera->get_projection_matrix());

    hud->update(input->get_cursor_position(), col);
}

void GameplayState::update_3D_board(chess::Square origin_sq, chess::Square landing_sq, chess::Piece piece) {
    float new_x = -BOARD_START - SQUARE_SIZE / 2.0 - SQUARE_SIZE * landing_sq.file();
    float new_z = BOARD_START + SQUARE_SIZE / 2.0 + SQUARE_SIZE * landing_sq.rank();

    std::cout << piece << std::endl;

    int piece_id = piece_tracker.getPieceID(origin_sq);
    piece_tracker.movePiece(origin_sq, landing_sq);
    if (piece.color() == chess::Color::WHITE) {
        if (piece_id < 8) {
            white_pawn->set_transform(7-piece_id, Matrix_Translate(new_x, 0.0f, new_z));
        } else {
            switch (piece_id) {
                case 8:
                    white_rook->set_transform(1, Matrix_Translate(new_x, 0.0f, new_z));
                    break;
                case 9:
                    white_rook->set_transform(0, Matrix_Translate(new_x, 0.0f, new_z));
                    break;
                case 10:
                    white_knight->set_transform(1, Matrix_Translate(new_x, 0.0f, new_z) * Matrix_Rotate_Y(M_PI));
                    break;
                case 11:
                    white_knight->set_transform(0, Matrix_Translate(new_x, 0.0f, new_z) * Matrix_Rotate_Y(M_PI));
                    break;
                case 12:
                    white_bishop->set_transform(1, Matrix_Translate(new_x, 0.0f, new_z));
                    break;
                case 13:
                    white_bishop->set_transform(0, Matrix_Translate(new_x, 0.0f, new_z));
                    break;
                case 14:
                    white_queen->set_transform(0, Matrix_Translate(new_x, 0.0f, new_z));
                    break;
                case 15:
                    white_king->set_transform(0, Matrix_Translate(new_x, 0.0f, new_z));
                    break;
            }
        }
    } else {
        if (piece_id < 24) {
            black_pawn->set_transform(7-piece_id+16, Matrix_Translate(new_x, 0.0f, new_z));
        } else {
            switch (piece_id) {
                case 24:
                    black_rook->set_transform(1, Matrix_Translate(new_x, 0.0f, new_z));
                    break;
                case 25:
                    black_rook->set_transform(0, Matrix_Translate(new_x, 0.0f, new_z));
                    break;
                case 26:
                    black_knight->set_transform(1, Matrix_Translate(new_x, 0.0f, new_z));
                    break;
                case 27:
                    black_knight->set_transform(0, Matrix_Translate(new_x, 0.0f, new_z));
                    break;
                case 28:
                    black_bishop->set_transform(1, Matrix_Translate(new_x, 0.0f, new_z));
                    break;
                case 29:
                    black_bishop->set_transform(0, Matrix_Translate(new_x, 0.0f, new_z));
                    break;
                case 30:
                    black_queen->set_transform(0, Matrix_Translate(new_x, 0.0f, new_z));
                    break;
                case 31:
                    black_king->set_transform(0, Matrix_Translate(new_x, 0.0f, new_z));
                    break;
            }
        }
    }
}

void GameplayState::update_chess_game(float delta_t) {
    if (chess_game->current_state == ChessGame::IngameState::SELECTING_SQUARES &&
        chess_game->selected_square != chess::Square::NO_SQ) {
        chess::Piece selected_piece = chess_game->board.at(chess_game->selected_square);

        if (chess_game->current_piece_to_move == chess::Piece::NONE) {
            if (selected_piece != chess::Piece::NONE &&
                selected_piece.color() == chess_game->board.sideToMove()) {
                    chess_game->set_origin_square(chess_game->selected_square);
                    chess_game->set_piece_to_move(selected_piece);
            }
        } else {
            if (selected_piece == chess::Piece::NONE ||
                selected_piece.color() != chess_game->board.sideToMove()) {
                chess::Move move = chess::Move::make(chess_game->origin_square, chess_game->selected_square);
                if (chess_game->is_move_valid(move)) {
                    std::cout << chess_game->current_piece_to_move << std::endl;
                    chess_game->make_move(move);
                    std::cout << chess_game->board << std::endl;
                    chess_game->current_state = ChessGame::IngameState::ONGOING_MOVE;
                    update_3D_board(chess_game->origin_square, chess_game->selected_square, chess_game->current_piece_to_move);
                    chess::movegen::legalmoves(chess_game->moves, chess_game->board);
                    chess_game->set_origin_square(chess::Square::NO_SQ); 
                    chess_game->set_selected_square(chess::Square::NO_SQ); 
                    chess_game->set_piece_to_move(chess::Piece::NONE); 
                }
            } else if (selected_piece.color() == chess_game->board.sideToMove()) {
                chess_game->set_origin_square(chess_game->selected_square);
                chess_game->set_piece_to_move(selected_piece);
            }
        }
    } else {
        // Aqui vem a lógica da animação
        // Peça movendo (com curva Bézier) + peça tombando + rotação do tabuleiro
        chess_game->current_state = ChessGame::IngameState::SELECTING_SQUARES;
    }
}

void GameplayState::update(float delta_t)
{
    // PASSO 1: atualizações sob demanda
    process_inputs(delta_t);

    // PASSO 2: atualização da lógica do jogo segundo os inputs
    update_chess_game(delta_t);
    

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
