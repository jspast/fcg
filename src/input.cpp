#include <algorithm>
#include <array>
#include <cmath>

#include "input.hpp"

#include "GLFW/glfw3.h"
#include "glm/ext/vector_float2.hpp"

#define GAMEPAD_DEFAULT_STICK_TRESHOLD 0.1
#define GAMEPAD_DEFAULT_TRIGGER_TRESHOLD 0.01

std::vector<InputManager*> InputManager::instances;

InputManager::InputManager(GLFWwindow *window,
                           std::vector<int> managed_keys,
                           std::vector<int> managed_mouse_buttons,
                           std::set<int> managed_gamepad_buttons,
                           std::set<int> managed_gamepad_axes)
{
    for (int key : managed_keys) {
        keys[key] = false;
        last_keys[key] = false;
    }

    for (int button : managed_mouse_buttons) {
        mouse_buttons[button] = false;
        last_mouse_buttons[button] = false;
    }

    this->managed_gamepad_buttons = managed_gamepad_buttons;
    this->managed_gamepad_axes = managed_gamepad_axes;

    left_stick_treshold.fill(GAMEPAD_DEFAULT_STICK_TRESHOLD);
    right_stick_treshold.fill(GAMEPAD_DEFAULT_STICK_TRESHOLD);
    left_trigger_treshold.fill(GAMEPAD_DEFAULT_TRIGGER_TRESHOLD);
    right_trigger_treshold.fill(GAMEPAD_DEFAULT_TRIGGER_TRESHOLD);

    InputManager::instances.push_back(this);

    glfwSetKeyCallback(window, InputManager::key_callback);
    glfwSetMouseButtonCallback(window, InputManager::mouse_button_callback);
    glfwSetCursorPosCallback(window, InputManager::cursor_position_callback);
    glfwSetScrollCallback(window, InputManager::scroll_callback);
}

InputManager::~InputManager()
{
    instances.erase(std::remove(instances.begin(), instances.end(), this), instances.end());
}

bool InputManager::get_is_enabled()
{
    return is_enabled;
}

void InputManager::set_is_enabled(bool b)
{
    is_enabled = b;
}

bool InputManager::get_is_key_down(int key)
{
    if (is_enabled) {
        std::map<int,bool>::iterator it = keys.find(key);
        if (it != keys.end())
            return keys[key];
    }

    return false;
}

bool InputManager::get_is_key_pressed(int key)
{
    return get_is_key_down(key) &&
           keys[key] != last_keys[key];
}

bool InputManager::get_is_mouse_button_down(int button)
{
    if (is_enabled) {
        std::map<int,bool>::iterator it = mouse_buttons.find(button);
        if (it != mouse_buttons.end())
            return mouse_buttons[button];
    }

    return false;
}

bool InputManager::get_is_mouse_button_pressed(int button)
{
    return get_is_mouse_button_down(button) &&
           mouse_buttons[button] != last_mouse_buttons[button];
}

bool InputManager::get_is_mouse_button_released(int button)
{
    return !get_is_mouse_button_down(button) &&
           mouse_buttons[button] != last_mouse_buttons[button];
}

void InputManager::update()
{
    gamepad_state_is_updated.fill(false);
    last_keys = keys;
    last_mouse_buttons = mouse_buttons;
    last_cursor_position = cursor_position;
}

void InputManager::update_gamepad_state(int joystick)
{
    gamepad_state_is_updated[joystick] = true;
    last_gamepad_state = gamepad_state;
    glfwGetGamepadState(joystick, &gamepad_state[joystick]);
}

bool InputManager::get_is_gamepad_button_down(int joystick, int button)
{
    if (is_enabled &&
        glfwJoystickIsGamepad(joystick) &&
        managed_gamepad_buttons.count(button)) {

        if (!gamepad_state_is_updated[joystick])
            update_gamepad_state(joystick);

        return gamepad_state[joystick].buttons[button] == GLFW_PRESS;
    }

    return false;
}

bool InputManager::get_is_gamepad_button_pressed(int joystick, int button)
{
    return get_is_gamepad_button_down(joystick, button) &&
           gamepad_state[joystick].buttons[button] != last_gamepad_state[joystick].buttons[button];
}

inline float apply_treshold(float value, float treshold)
{
    return (std::fabs(value) < treshold) ? 0.0f : value;
}

float InputManager::get_gamepad_axis_value(int joystick, int axis)
{
    if (is_enabled && glfwJoystickIsGamepad(joystick) && managed_gamepad_axes.count(axis)) {

        if (!gamepad_state_is_updated[joystick])
            update_gamepad_state(joystick);

        float treshold;

        switch (axis / 2) {
            case 0:
                treshold = left_stick_treshold[joystick];
                break;
            case 1:
                treshold = right_stick_treshold[joystick];
                break;
            case 2:
                if (axis % 2)
                    treshold = right_trigger_treshold[joystick];
                else
                    treshold = left_trigger_treshold[joystick];
                break;
        }

        return apply_treshold(gamepad_state[joystick].axes[axis], treshold);
    }

    return 0.0f;
}

void InputManager::set_left_stick_treshold(int joystick, float treshold)
{
    left_stick_treshold[joystick] = treshold;
}

void InputManager::set_right_stick_treshold(int joystick, float treshold)
{
    right_stick_treshold[joystick] = treshold;
}

void InputManager::set_left_trigger_treshold(int joystick, float treshold)
{
    left_trigger_treshold[joystick] = treshold;
}

void InputManager::set_right_trigger_treshold(int joystick, float treshold)
{
    right_trigger_treshold[joystick] = treshold;
}

glm::vec2 InputManager::get_cursor_position()
{
    if (is_enabled)
        return cursor_position;

    return glm::vec2(0.0f, 0.0f);
}

glm::vec2 InputManager::get_cursor_movement()
{
    if (is_enabled)
        return cursor_position - last_cursor_position;

    return glm::vec2(0.0f, 0.0f);
}

glm::vec2 InputManager::get_scroll_offset()
{
    glm::vec2 result = glm::vec2(0.0f, 0.0f);

    if (is_enabled)
        result = scroll_offset;

    scroll_offset = glm::vec2(0.0f, 0.0f);

    return result;
}

void InputManager::set_key_down(int key, bool is_down)
{
    std::map<int,bool>::iterator it = keys.find(key);
    if (it != keys.end())
        keys[key] = is_down;
}

void InputManager::set_mouse_button_down(int button, bool is_down)
{
    std::map<int,bool>::iterator it = mouse_buttons.find(button);
    if (it != mouse_buttons.end())
        mouse_buttons[button] = is_down;
}

void InputManager::set_cursor_position(double x_pos, double y_pos)
{
    cursor_position = glm::vec2(x_pos, y_pos);
}

void InputManager::set_scroll_offset(double x_offset, double y_offset)
{
    scroll_offset = glm::vec2(x_offset, y_offset);
}

// Callback functions inspired by: https://stackoverflow.com/a/55582951
void InputManager::key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    for (InputManager *input_manager : instances)
        input_manager->set_key_down(key, action != GLFW_RELEASE);
}

void InputManager::mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
{
    for (InputManager *input_manager : instances)
        input_manager->set_mouse_button_down(button, action != GLFW_RELEASE);
}

void InputManager::cursor_position_callback(GLFWwindow *window, double x_pos, double y_pos)
{
    for (InputManager *input_manager : instances)
        input_manager->set_cursor_position(x_pos, y_pos);
}

void InputManager::scroll_callback(GLFWwindow *window, double x_offset, double y_offset)
{
    for (InputManager *input_manager : instances)
         input_manager->set_scroll_offset(x_offset, y_offset);
}
