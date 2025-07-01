#pragma once

#include "state.hpp"

class LoadingState: public GameState {
    public:
        void load() override;
        void unload() override;

        void update(float delta_t) override;
        void draw() override;

    private:
        bool loading_complete = false;
};
