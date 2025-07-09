#include "states/menu.hpp"
#include "states/loading.hpp"

#include <memory>

#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>
#include "hud.hpp"

#include "input.hpp"
#include "textrendering.hpp"

void MenuState::load()
{
    input = std::make_unique<InputManager>(
        window->glfw_window,
        std::vector<int> {},
        std::vector<int> {
            GLFW_MOUSE_BUTTON_1
        },
        std::set<int> {},
        std::set<int> {}
    );

    glm::vec2 play_button_pos(HUD_START + BORDER_MARGIN * 2.0F,
        TextRendering_LineHeight(window->glfw_window) * 2.0f * 0.5f);

    play_button = std::make_unique<Button>(window->glfw_window,
                                           input.get(),
                                           play_button_pos,
                                           "JOGAR",
                                           3.0f);

    glm::vec2 tex_qual_pos(HUD_START + BORDER_MARGIN * 2.0F, HUD_BOTTOM + BORDER_MARGIN * 9.0f);

    texture_quality_button = std::make_unique<Button>(window->glfw_window,
                                                      input.get(),
                                                      tex_qual_pos,
                                                      "Alta",
                                                      2.0f);

    texture_quality = HIGH;
}

void MenuState::unload() {}

void MenuState::update(float delta_t)
{
    if (play_button->is_clicked()) {
        manager->change_state(std::make_unique<LoadingState>(texture_quality));
    }
    else {
        if (play_button->is_selecting())
            play_button->set_scale(3.5f);
        else
            play_button->set_scale(3.0f);

        if (texture_quality_button->is_selecting())
            texture_quality_button->set_scale(2.5f);
        else
            texture_quality_button->set_scale(2.0f);

        if (texture_quality_button->is_clicked()) {
            if (texture_quality == HIGH) {
                texture_quality = LOW;
                texture_quality_button->set_text("Baixa");
            }
            else {
                texture_quality = HIGH;
                texture_quality_button->set_text("Alta");
            }
        }

        input->update();
    }
}

void MenuState::draw()
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    TextRendering_PrintString(window->glfw_window, "FChessG", HUD_START + BORDER_MARGIN * 2.0F, 1.0f - BORDER_MARGIN * 10.0f, 4.0f);

    TextRendering_PrintString(window->glfw_window, "Qualidade de texturas:", HUD_START + BORDER_MARGIN * 2.0F, HUD_BOTTOM + BORDER_MARGIN * 10.0f, 2.0f);

    play_button->draw();
    texture_quality_button->draw();
}
