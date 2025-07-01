#pragma once

#include <memory>
#include <vector>

#include "gpu.hpp"
#include "window.hpp"

class GameStateManager;

class GameState {
    protected:
        GameStateManager* manager;
        std::shared_ptr<Window> window;
        std::shared_ptr<GpuProgram> gpu_program;

    public:
        virtual void set_manager(GameStateManager* manager);
        virtual void set_window(std::shared_ptr<Window> window);
        virtual void set_gpu_program(std::shared_ptr<GpuProgram> gpu_program);

        virtual void load() = 0;
        virtual void unload() = 0;

        virtual void update(float delta_t) = 0;
        virtual void draw() = 0;

        virtual bool update_in_background();
        virtual bool draw_in_background();
};

class GameStateManager {
    private:
        std::vector<std::unique_ptr<GameState>> states;
        std::shared_ptr<Window> window;
        std::shared_ptr<GpuProgram> gpu_program;

    public:
        GameStateManager(std::shared_ptr<Window> w,
                         std::shared_ptr<GpuProgram> g):
            window(w), gpu_program(g) {}

        void update(float dt);
        void draw();

        void push_state(std::unique_ptr<GameState> state);
        void pop_state();

        void change_state(std::unique_ptr<GameState> state);
        GameState* current_state();

        bool empty();
};
