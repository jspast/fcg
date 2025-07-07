#pragma once

#include "state.hpp"
#include "input.hpp"
#include "hud.hpp"
#include "states/loading.hpp"

class MenuState: public GameState {
    public:
        void load() override;
        void unload() override;

        void update(float delta_t) override;
        void draw() override;

    private:
        std::unique_ptr<InputManager> input;

        std::unique_ptr<Button> play_button;
        std::unique_ptr<Button> texture_quality_button;

        TEXTURE_QUALITY texture_quality;
};
