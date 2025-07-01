#include "state.hpp"

void GameState::set_manager(GameStateManager* m)
{
    manager = m;
}

void GameState::set_window(std::shared_ptr<Window> w)
{
    window = w;
}

void GameState::set_gpu_program(std::shared_ptr<GpuProgram> g)
{
    gpu_program = g;
}

bool GameState::update_in_background()
{
    return true;
}

bool GameState::draw_in_background()
{
    return false;
}

void GameStateManager::update(float dt)
{
    for (size_t i = 0; i < states.size(); ++i) {
        bool is_top = (i == states.size() - 1);
        if (is_top || states[i]->update_in_background()) {
            states[i]->update(dt);
        }
    }
}

void GameStateManager::draw()
{
    for (size_t i = 0; i < states.size(); ++i) {
        bool is_top = (i == states.size() - 1);
        if (is_top || states[i]->draw_in_background()) {
            states[i]->draw();
        }
    }
}

void GameStateManager::push_state(std::unique_ptr<GameState> state)
{
    state->set_manager(this);
    state->set_window(window);
    state->set_gpu_program(gpu_program);
    states.push_back(std::move(state));
    states.back()->load();
}

void GameStateManager::pop_state()
{
    if (!empty()) {
        states.back()->unload();
        states.pop_back();
    }
}

void GameStateManager::change_state(std::unique_ptr<GameState> state)
{
    pop_state();
    push_state(std::move(state));
}

GameState* GameStateManager::current_state()
{
    return empty() ? nullptr : states.back().get();
}

bool GameStateManager::empty()
{
    return states.empty();
}
