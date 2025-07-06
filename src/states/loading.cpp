#include <memory>
#include <format>

#include "states/loading.hpp"
#include "states/game.hpp"
#include "state.hpp"

#include "textrendering.hpp"

void LoadingState::load()
{
    gpu_program->load_cubemap_from_hdr_files({"../../data/textures/sky/px_high.hdr",
                                              "../../data/textures/sky/nx_high.hdr",
                                              "../../data/textures/sky/py_high.hdr",
                                              "../../data/textures/sky/ny_high.hdr",
                                              "../../data/textures/sky/pz_high.hdr",
                                              "../../data/textures/sky/nz_high.hdr"},
                                             "SkyImage");

    gpu_program->load_textures_async({
        {"../../data/textures/floor/diffuse_high.jpg", "FloorImage"},
        {"../../data/textures/floor/ambient_high.jpg", "FloorAmbient"},
        {"../../data/textures/floor/roughness_high.jpg", "FloorRoughness"},
        {"../../data/textures/floor/normal_high.jpg", "FloorNormal"},

        {"../../data/textures/table/diffuse_high.jpg", "TableImage"},
        {"../../data/textures/table/ambient_high.jpg", "TableAmbient"},
        {"../../data/textures/table/roughness_high.jpg", "TableRoughness"},
        {"../../data/textures/table/normal_high.jpg", "TableNormal"},

        {"../../data/textures/board/diffuse_high.jpg", "BoardImage"},
        {"../../data/textures/board/ambient_high.jpg", "BoardAmbient"},
        {"../../data/textures/board/roughness_high.jpg", "BoardRoughness"},
        {"../../data/textures/board/normal_high.jpg", "BoardNormal"},

        {"../../data/textures/black_pieces/diffuse_high.jpg", "BlackPiecesImage"},
        {"../../data/textures/black_pieces/ambient_high.jpg", "BlackPiecesAmbient"},
        {"../../data/textures/black_pieces/roughness_high.jpg", "BlackPiecesRoughness"},
        {"../../data/textures/black_pieces/normal_high.jpg", "BlackPiecesNormal"},

        {"../../data/textures/white_pieces/diffuse_high.jpg", "WhitePiecesImage"},
        {"../../data/textures/white_pieces/ambient_high.jpg", "WhitePiecesAmbient"},
        {"../../data/textures/white_pieces/roughness_high.jpg", "WhitePiecesRoughness"},
        {"../../data/textures/white_pieces/normal_high.jpg", "WhitePiecesNormal"}
    });
}

void LoadingState::unload() {}

void LoadingState::update(float dt)
{
    if (!loading_complete) {
        loading_complete = gpu_program->upload_pending_textures();

        if (loading_complete)
            manager->change_state(std::make_unique<GameplayState>());
    }
}

void LoadingState::draw()
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float lineheight = TextRendering_LineHeight(window->glfw_window);
    float charwidth = TextRendering_CharWidth(window->glfw_window);

    float loading_progress = float(gpu_program->num_uploaded_textures) * 100.0f / float(gpu_program->num_loaded_textures);

    TextRendering_PrintString(window->glfw_window,
                              std::format("Carregando... {:.2f}%", loading_progress),
                              -10 * charwidth, -0.5 * lineheight);
}
