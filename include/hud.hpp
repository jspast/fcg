#pragma once

#include <string>

#include <GLFW/glfw3.h>

#include "camera.hpp"

class Hud {
    public:
        Hud(GLFWwindow *window);

        void toggle_debug_info(bool boolean);
        void toggle_debug_info();

        void update(Camera& camera);

    private:
        bool show_debug_info = false;

        std::string debug_vendor;
        std::string debug_renderer;
        std::string debug_glversion;
        std::string debug_glslversion;

        GLFWwindow* window;

        float fps;
        float frametime;

        void render_debug_info(Camera& camera);

        void update_timings();
};
