#pragma once

#include "state.hpp"

enum TEXTURE_QUALITY {
    LOW = 0,
    HIGH = 1,
};

class LoadingState: public GameState {
    public:
        LoadingState(TEXTURE_QUALITY texture_quality);

        void load() override;
        void unload() override;

        void update(float delta_t) override;
        void draw() override;

    private:
        TEXTURE_QUALITY texture_quality;

        bool loading_complete = false;
};
