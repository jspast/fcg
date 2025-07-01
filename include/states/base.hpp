#pragma once

#include <memory>

#include "state.hpp"
#include "input.hpp"

class BaseState: public GameState {
    public:
        void load() override;
        void unload() override;

        void update(float delta_t) override;
        void draw() override;

    private:
        std::unique_ptr<InputManager> input;
};
