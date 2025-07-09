#pragma once

#include <memory>
#include <string_view>

#include <GLFW/glfw3.h>

#include "camera.hpp"
#include "input.hpp"

#define BORDER_MARGIN (0.025)

#define HUD_TOP (1.0f - BORDER_MARGIN)
#define HUD_BOTTOM (-1.0f + BORDER_MARGIN)
#define HUD_START (-1.0f + BORDER_MARGIN)
#define HUD_END (1.0f - BORDER_MARGIN)

class Button {
    public:
        Button(GLFWwindow *window,
               InputManager *input,
               glm::vec2 pos,
               std::string text,
               float scale=1.0f);

        void draw();

        bool is_selecting();
        bool is_clicked();

        void set_text(std::string text);
        void set_scale(float scale);

    private:
        GLFWwindow *window;
        InputManager *input;

        glm::vec2 pos_start;
        glm::vec2 pos_end;

        std::string text;

        float scale;
};

class Hud {
    public:
        Hud(GLFWwindow *window, std::shared_ptr<Camera> *camera);

        void toggle_debug_info(bool boolean);
        void toggle_debug_info();

        void update(glm::vec2 cursor, glm::vec4 cursor_intersection);
        void draw();

    private:
        bool show_debug_info = false;

        std::string_view debug_vendor;
        std::string_view debug_renderer;
        std::string_view debug_glversion;
        std::string_view debug_glslversion;

        GLFWwindow* window;

        float fps;
        float frametime;

        glm::vec2 cursor_pos;
        glm::vec4 cursor_intersection;

        std::shared_ptr<Camera> *camera;

        void render_debug_info();

        void update_timings();
};
