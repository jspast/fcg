#include <memory>

#include "states/base.hpp"
#include "input.hpp"
#include "states/menu.hpp"

void BaseState::load()
{
    input = std::make_unique<InputManager>(
        window->glfw_window,
        std::vector<int> {
            GLFW_KEY_F11,
            GLFW_KEY_R
        },
        std::vector<int> {},
        std::set<int> {},
        std::set<int> {}
    );

    manager->push_state(std::make_unique<MenuState>());
}

void BaseState::unload() {}

void BaseState::update(float delta_t)
{
    if (input->get_is_key_pressed(GLFW_KEY_F11))
        window->toggle_fullscreen();

    if (input->get_is_key_pressed(GLFW_KEY_R))
        gpu_program->reload_shaders();

    input->update();
}

void BaseState::draw() {}
