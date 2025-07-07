#include <iostream>

#include <GLFW/glfw3.h>

#include "window.hpp"

#define OPENGL_VERSION_MAJOR 3
#define OPENGL_VERSION_MINOR 3

// Callback for printing GLFW errors
void glfw_error_callback(int error, const char* description)
{
    std::cerr << "ERROR: GLFW: " << description << std::endl;
}

Window::Window(const char* title, int width, int height)
{
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, OPENGL_VERSION_MAJOR);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, OPENGL_VERSION_MINOR);

    #ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    // Use OpenGL core profile, for solely modern functions
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    glfw_window = glfwCreateWindow(width, height, title, NULL, NULL);
    if (!glfw_window)
    {
        glfwTerminate();
        std::exit(EXIT_FAILURE);
    }

    glfwSetWindowUserPointer(glfw_window, this);

    if (glfwRawMouseMotionSupported())
        glfwSetInputMode(glfw_window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

    // Indicamos que as chamadas OpenGL deverão renderizar nesta janela
    glfwMakeContextCurrent(glfw_window);
}

Window::~Window()
{
    glfwDestroyWindow(glfw_window);
}

void Window::set_user_pointer(void *p)
{
    glfwSetWindowUserPointer(glfw_window, p);
}

void Window::set_framebuffer_size_callback(void callback(GLFWwindow *, int, int))
{
    glfwSetFramebufferSizeCallback(glfw_window, callback);
}

void Window::maximize()
{
    GLFWmonitor *monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);

    glfwSetWindowMonitor(glfw_window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
}

void Window::resize(int w, int h, int x, int y)
{
    glfwSetWindowMonitor(glfw_window, NULL, x, y, w, h, refresh_rate);
}

glm::vec2 Window::get_size()
{
    glm::vec<2, int> size;

    glfwGetWindowSize(glfw_window, &size.x, &size.y);

    return size;
}

void Window::toggle_fullscreen()
{
    toggle_fullscreen(!fullscreen);
}

void Window::toggle_fullscreen(bool b)
{
    fullscreen = b;

    if (b) {
        // Store the window size before maximazing to allow restoring later
        glfwGetWindowSize(glfw_window, &windowed_width, &windowed_height);
        glfwGetWindowPos(glfw_window, &windowed_x, &windowed_y);
        maximize();
    }
    else {
        resize(windowed_width, windowed_height, windowed_x, windowed_y);
    }
}

bool Window::is_fullscreen()
{
    return fullscreen;
}

void Window::toggle_cursor()
{
    toggle_cursor(!cursor_enabled);
}

void Window::toggle_cursor(bool b)
{
    cursor_enabled = b;

    if (b)
        glfwSetInputMode(glfw_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    else
        glfwSetInputMode(glfw_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

bool Window::is_cursor_enabled()
{
    return cursor_enabled;
}
