#include <cstddef>
#include <iostream>

#include <GLFW/glfw3.h>
#include <ostream>

#include "window.hpp"

#define OPENGL_VERSION_MAJOR 3
#define OPENGL_VERSION_MINOR 3

// Callback for printing GLFW errors
void glfw_error_callback(int error, const char* description)
{
    std::cerr << "ERROR: GLFW: " << description << std::endl;
}

Window::Window(const char* title, int width, int height, bool disable_cursor)
{
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, OPENGL_VERSION_MAJOR);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, OPENGL_VERSION_MINOR);

    #ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    // Use OpenGL core profile, for solely modern functions
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    glfw_window = glfwCreateWindow(width, height, title, NULL, NULL);
    if (!glfw_window)
    {
        glfwTerminate();
        std::exit(EXIT_FAILURE);
    }

    glfwSetWindowUserPointer(glfw_window, this);

    if (glfwRawMouseMotionSupported())
        glfwSetInputMode(glfw_window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

    toggle_cursor(!disable_cursor);

    // Indicamos que as chamadas OpenGL deverão renderizar nesta janela
    glfwMakeContextCurrent(glfw_window);
}

Window::~Window()
{
    glfwDestroyWindow(glfw_window);
}

void Window::set_framebuffer_size_callback(void callback(GLFWwindow *, int, int))
{
    glfwSetFramebufferSizeCallback(glfw_window, callback);
}

void Window::set_key_callback(void callback(GLFWwindow *, int, int, int, int))
{
    glfwSetKeyCallback(glfw_window, callback);
}

void Window::set_mouse_button_callback(void callback(GLFWwindow *, int, int, int))
{
    glfwSetMouseButtonCallback(glfw_window, callback);
}

void Window::set_cursor_pos_callback(void callback(GLFWwindow *, double, double))
{
    glfwSetCursorPosCallback(glfw_window, callback);
}

void Window::set_scroll_callback(void callback(GLFWwindow *, double, double))
{
    glfwSetScrollCallback(glfw_window, callback);
}

void Window::set_joystick_callback(void callback(int, int))
{
    glfwSetJoystickCallback(callback);
}

void Window::maximize()
{
    GLFWmonitor *monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);

    glfwSetWindowMonitor(glfw_window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
}

void Window::resize(int w, int h)
{
    glfwSetWindowMonitor(glfw_window, NULL, 0, 0, w, h, refresh_rate);
}

void Window::toggle_fullscreen()
{
    fullscreen = !fullscreen;

    if (fullscreen) {
        // Store the window size before maximazing to allow restoring later
        glfwGetWindowSize(glfw_window, &windowed_width, &windowed_height);
        maximize();
    }
    else {
        resize(windowed_width, windowed_height);
    }
}

void Window::toggle_fullscreen(bool b)
{
    if (fullscreen != b)
        toggle_fullscreen();
}

bool Window::is_fullscreen()
{
    return fullscreen;
}

void Window::toggle_cursor()
{
    cursor_enabled = !cursor_enabled;

    if (cursor_enabled)
        glfwSetInputMode(glfw_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    else
        glfwSetInputMode(glfw_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void Window::toggle_cursor(bool b)
{
    if (cursor_enabled != b)
        toggle_cursor();
}

bool Window::is_cursor_enabled()
{
    return cursor_enabled;
}
