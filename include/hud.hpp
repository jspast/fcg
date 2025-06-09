#pragma once

#include <string>

#include <GLFW/glfw3.h>

#include "camera.hpp"

class Hud {
    public:
        Hud(GLFWwindow *window);

        void toggle_debug_info(bool boolean);
        void toggle_debug_info();

        void update(Camera& camera, glm::vec2 cursor, glm::vec4 cursor_intersection);

    private:
        bool show_debug_info = false;

        std::string debug_vendor;
        std::string debug_renderer;
        std::string debug_glversion;
        std::string debug_glslversion;

        GLFWwindow* window;

        float fps;
        float frametime;

        void render_debug_info(Camera& camera, glm::vec2 cursor, glm::vec4 cursor_intersection);

        void update_timings();
};
