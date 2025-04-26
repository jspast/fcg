#pragma once

#include <GLFW/glfw3.h>

#define DEFAULT_WIDTH   800
#define DEFAULT_HEIGHT  800

void glfw_error_callback(int error, const char* description);

class Window {
    protected:
        bool fullscreen = GLFW_FALSE;
        bool cursor_enabled = GLFW_FALSE;

        void maximize();

    public:
        Window(const char* title, int width=DEFAULT_WIDTH, int height=DEFAULT_HEIGHT, bool disable_cursor=GLFW_FALSE);
        ~Window();

        GLFWwindow *glfw_window;

        int refresh_rate;

        int windowed_width;
        int windowed_height;

        void resize(int width, int height);

        void toggle_fullscreen();
        void toggle_fullscreen(bool boolean);
        bool is_fullscreen();

        void set_framebuffer_size_callback(void callback(GLFWwindow *, int, int));
        void set_key_callback(void callback(GLFWwindow *, int, int, int, int) );
        void set_mouse_button_callback(void callback(GLFWwindow *, int, int, int));
        void set_cursor_pos_callback(void callback(GLFWwindow *, double, double));
        void set_scroll_callback(void callback(GLFWwindow *, double, double));
        void set_joystick_callback(void callback(int, int));

        void toggle_cursor();
        void toggle_cursor(bool boolean);
        bool is_cursor_enabled();
};
