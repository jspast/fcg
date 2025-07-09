#pragma once

#include <memory>
#include <utility>
#include <unordered_map>

#include <glm/vec4.hpp>

#include <chess.hpp>

#include "object.hpp"
#include "chess_game.hpp"
#include "camera.hpp"
#include "hud.hpp"
#include "input.hpp"
#include "state.hpp"
#include "animation.hpp"

class PieceTracker {
    private:
        std::unordered_map<std::size_t, int> pieceIDs;

    public:
        PieceTracker() {
            init();
        }

        void init() {
            // As 8 primeiras peças são os peões brancos
            pieceIDs[squareHash(chess::Square::SQ_A2)] = 0;
            pieceIDs[squareHash(chess::Square::SQ_B2)] = 1;
            pieceIDs[squareHash(chess::Square::SQ_C2)] = 2;
            pieceIDs[squareHash(chess::Square::SQ_D2)] = 3;
            pieceIDs[squareHash(chess::Square::SQ_E2)] = 4;
            pieceIDs[squareHash(chess::Square::SQ_F2)] = 5;
            pieceIDs[squareHash(chess::Square::SQ_G2)] = 6;
            pieceIDs[squareHash(chess::Square::SQ_H2)] = 7;

            // As próximas duas são as torres brancas
            pieceIDs[squareHash(chess::Square::SQ_A1)] = 8;
            pieceIDs[squareHash(chess::Square::SQ_H1)] = 9;

            // As próximas duas são os cavalos brancos
            pieceIDs[squareHash(chess::Square::SQ_B1)] = 10;
            pieceIDs[squareHash(chess::Square::SQ_G1)] = 11;

            // As próximas duas são os bispos brancos
            pieceIDs[squareHash(chess::Square::SQ_C1)] = 12;
            pieceIDs[squareHash(chess::Square::SQ_F1)] = 13;

            // Por fim, a rainha e o rei brancos
            pieceIDs[squareHash(chess::Square::SQ_D1)] = 14;    // Rainha
            pieceIDs[squareHash(chess::Square::SQ_E1)] = 15;    // Rei

            // A mesma lógica, mas para as peças pretas
            // Peões pretos
            pieceIDs[squareHash(chess::Square::SQ_A7)] = 16;
            pieceIDs[squareHash(chess::Square::SQ_B7)] = 17;
            pieceIDs[squareHash(chess::Square::SQ_C7)] = 18;
            pieceIDs[squareHash(chess::Square::SQ_D7)] = 19;
            pieceIDs[squareHash(chess::Square::SQ_E7)] = 20;
            pieceIDs[squareHash(chess::Square::SQ_F7)] = 21;
            pieceIDs[squareHash(chess::Square::SQ_G7)] = 22;
            pieceIDs[squareHash(chess::Square::SQ_H7)] = 23;
            // Torres pretas
            pieceIDs[squareHash(chess::Square::SQ_A8)] = 24;
            pieceIDs[squareHash(chess::Square::SQ_H8)] = 25;
            // Cavalos preto
            pieceIDs[squareHash(chess::Square::SQ_B8)] = 26;
            pieceIDs[squareHash(chess::Square::SQ_G8)] = 27;
            // Bispos pretos
            pieceIDs[squareHash(chess::Square::SQ_C8)] = 28;
            pieceIDs[squareHash(chess::Square::SQ_F8)] = 29;
            // Bispos pretos
            pieceIDs[squareHash(chess::Square::SQ_D8)] = 30;
            pieceIDs[squareHash(chess::Square::SQ_E8)] = 31;
        }

        std::size_t squareHash(const chess::Square sq) const {
            return static_cast<std::size_t>(sq.index());
        }

        int getPieceID(chess::Square sq) const {
            auto it = pieceIDs.find(squareHash(sq));
            return (it != pieceIDs.end()) ? it->second : -1;
        }

        void movePiece(chess::Square from, chess::Square to) {
            int id = pieceIDs[squareHash(from)];
            pieceIDs.erase(squareHash(from));
            pieceIDs[squareHash(to)] = id;
        }
};

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

        std::shared_ptr<Object> white_pawn;
        std::shared_ptr<Object> white_king;
        std::shared_ptr<Object> white_rook;
        std::shared_ptr<Object> white_knight;
        std::shared_ptr<Object> white_queen;
        std::shared_ptr<Object> white_bishop;
        std::shared_ptr<Object> black_pawn;
        std::shared_ptr<Object> black_king;
        std::shared_ptr<Object> black_rook;
        std::shared_ptr<Object> black_knight;
        std::shared_ptr<Object> black_queen;
        std::shared_ptr<Object> black_bishop;

        PieceTracker piece_tracker;
        std::pair<std::shared_ptr<Object>, int> piece_to_object_instance(chess::Square sq, chess::Piece piece);

        std::vector<std::pair<glm::vec4, AABB>> aabbs;

        AnimationCubicBezier piece_animation;
        AnimationCamera camera_animation;

        void process_inputs(float delta_t);
        void update_chess_game(float delta_t);
        void update_3D_piece(chess::Move move, chess::Piece piece, float new_x, float new_y, float new_z);
};
