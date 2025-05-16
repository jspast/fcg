#pragma once

#include <map>
#include <set>
#include <vector>
#include <array>

#include "GLFW/glfw3.h"
#include "glm/ext/vector_float2.hpp"

// #define MAX_GAMEPADS (GLFW_JOYSTICK_LAST + 1)
#define MAX_GAMEPADS 2

class InputManager {
    public:
        InputManager(GLFWwindow *window,
                     std::vector<int> managed_keys = {},
                     std::vector<int> managed_mouse_buttons = {},
                     std::set<int> managed_gamepad_buttons = {},
                     std::set<int> managed_gamepad_axes = {});

        ~InputManager();

        bool get_is_key_down(int key);
        bool get_is_mouse_button_down(int button);
        bool get_is_key_pressed(int key);
        bool get_is_mouse_button_pressed(int button);

        glm::vec2 get_cursor_position();
        glm::vec2 get_cursor_movement();
        glm::vec2 get_scroll_offset();

        bool get_is_gamepad_button_down(int joystick, int button);
        bool get_is_gamepad_button_pressed(int joystick, int button);
        float get_gamepad_axis_value(int joystick, int axis);

        void set_left_stick_treshold(int joystick, float treshold);
        void set_right_stick_treshold(int joystick, float treshold);
        void set_left_trigger_treshold(int joystick, float treshold);
        void set_right_trigger_treshold(int joystick, float treshold);

        bool get_is_enabled();
        void set_is_enabled(bool boolean);

        // Should be called every frame after processing input
        void update();

    private:
        bool is_enabled = true;

        std::map<int, bool> keys;
        std::map<int, bool> last_keys;

        std::map<int, bool> mouse_buttons;
        std::map<int, bool> last_mouse_buttons;

        std::array<bool, MAX_GAMEPADS> gamepad_state_is_updated;
        std::array<GLFWgamepadstate, MAX_GAMEPADS> gamepad_state;
        std::array<GLFWgamepadstate, MAX_GAMEPADS> last_gamepad_state;

        glm::vec2 cursor_position = glm::vec2(0.0f, 0.0f);
        glm::vec2 last_cursor_position = glm::vec2(0.0f, 0.0f);
        glm::vec2 scroll_offset = glm::vec2(0.0f, 0.0f);

        std::set<int> managed_gamepad_buttons;
        std::set<int> managed_gamepad_axes;

        std::array<float, MAX_GAMEPADS> left_stick_treshold;
        std::array<float, MAX_GAMEPADS> right_stick_treshold;
        std::array<float, MAX_GAMEPADS> left_trigger_treshold;
        std::array<float, MAX_GAMEPADS> right_trigger_treshold;

        static std::vector<InputManager*> instances;

        void update_gamepad_state(int joystick);

        void set_key_down(int key, bool is_down);
        void set_mouse_button_down(int button, bool is_down);
        void set_cursor_position(double x_pos, double y_pos);
        void set_scroll_offset(double x_offset, double y_offset);

        static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
        static void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);
        static void cursor_position_callback(GLFWwindow *window, double x_pos, double y_pos);
        static void scroll_callback(GLFWwindow *window, double x_offset, double y_offset);
};
