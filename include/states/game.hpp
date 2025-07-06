#pragma once

#include <memory>

#include <chess.hpp>
#include <utility>

#include <glm/vec4.hpp>
#include "object.hpp"
#include "chess_game.hpp"
#include "camera.hpp"
#include "hud.hpp"
#include "input.hpp"
#include "state.hpp"

class GameplayState: public GameState {
    public:
        void load() override;
        void unload() override;

        void update(float delta_t) override;
        void draw() override;

    private:
        std::shared_ptr<FreeCamera>   free_camera;
        std::shared_ptr<LookAtCamera> lookat_camera;
        std::shared_ptr<Camera>       camera;

        std::unique_ptr<InputManager> game_input;
        std::unique_ptr<InputManager> input;

        std::unique_ptr<Hud> hud;

        std::unique_ptr<ChessGame> chess_game;

        std::shared_ptr<ObjModel> sky_model;
        std::shared_ptr<ObjModel> floor_model;
        std::shared_ptr<ObjModel> table_model;
        std::shared_ptr<ObjModel> board_model;

        std::shared_ptr<ObjModel> pawn_model;
        std::shared_ptr<ObjModel> king_model;
        std::shared_ptr<ObjModel> rook_model;
        std::shared_ptr<ObjModel> knight_model;
        std::shared_ptr<ObjModel> queen_model;
        std::shared_ptr<ObjModel> bishop_model;

        std::shared_ptr<Object> sky;
        std::shared_ptr<Object> floor;
        std::shared_ptr<Object> table;
        std::shared_ptr<Object> board;

        std::shared_ptr<Object> pawn;
        std::shared_ptr<Object> king;
        std::shared_ptr<Object> rook;
        std::shared_ptr<Object> knight;
        std::shared_ptr<Object> queen;
        std::shared_ptr<Object> bishop;

        std::vector<std::pair<glm::vec4, AABB>> aabbs;
};
