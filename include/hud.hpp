#pragma once

#include <memory>
#include <string>

#include <GLFW/glfw3.h>

#include "camera.hpp"

class Hud {
    public:
        Hud(GLFWwindow *window, std::shared_ptr<Camera> *camera);

        void toggle_debug_info(bool boolean);
        void toggle_debug_info();

        void update(glm::vec2 cursor, glm::vec4 cursor_intersection);
        void draw();

    private:
        bool show_debug_info = false;

        std::string debug_vendor;
        std::string debug_renderer;
        std::string debug_glversion;
        std::string debug_glslversion;

        GLFWwindow* window;

        float fps;
        float frametime;

        glm::vec2 cursor_pos;
        glm::vec4 cursor_intersection;

        std::shared_ptr<Camera> *camera;

        void render_debug_info();

        void update_timings();
};
