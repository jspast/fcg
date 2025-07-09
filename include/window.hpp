#pragma once

#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>

#define DEFAULT_WIDTH   800
#define DEFAULT_HEIGHT  800

void glfw_error_callback(int error, const char* description);

class Window {
    protected:
        bool fullscreen = GLFW_FALSE;
        bool cursor_enabled = GLFW_TRUE;

        int refresh_rate;

        int windowed_width;
        int windowed_height;

        int windowed_x;
        int windowed_y;

        void maximize();

    public:
        Window(const char* title, int width=DEFAULT_WIDTH, int height=DEFAULT_HEIGHT);

        GLFWwindow *glfw_window;

        void resize(int width, int height, int x, int y);

        glm::vec2 get_size();

        void toggle_fullscreen();
        void toggle_fullscreen(bool boolean);
        bool is_fullscreen();

        void set_user_pointer(void *user_pointer);

        void set_framebuffer_size_callback(void callback(GLFWwindow *, int, int));

        void toggle_cursor();
        void toggle_cursor(bool boolean);
        bool is_cursor_enabled();
};
