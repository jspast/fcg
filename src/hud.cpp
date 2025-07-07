#include <format>
#include <memory>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <glm/vec4.hpp>
#include <glm/vec2.hpp>

#include "input.hpp"
#include "textrendering.hpp"
#include "hud.hpp"

#define TIMINGS_UPDATE_INTERVAL 1.0f

Button::Button(GLFWwindow *w, InputManager *i, glm::vec2 pos, std::string t, float s)
{
    window = w;
    input = i;
    pos_start.x = pos.x;
    pos_end.y = pos.y;
    text = t;

    set_scale(s);
}

void Button::set_text(std::string t)
{
    text = t;
}

void Button::set_scale(float s)
{
    if (scale == s)
        return;

    scale = s;

    float lineheight = TextRendering_LineHeight(window) * scale;
    float charwidth = TextRendering_CharWidth(window) * scale;

    pos_start.y = pos_end.y - lineheight;
    pos_end.x = pos_start.x + charwidth * text.length();
}

bool Button::is_selecting()
{
    int height, width;
    glfwGetWindowSize(window, &width, &height);

    glm::vec2 cursor_ndc = glm::vec2((input->get_cursor_position().x / int(width/2)) - 1,
                                     -(input->get_cursor_position().y / int(height/2)) + 1);

    if (pos_start.x < cursor_ndc.x && pos_end.x > cursor_ndc.x &&
        pos_start.y < cursor_ndc.y && pos_end.y > cursor_ndc.y)
        return true;

    return false;
}

bool Button::is_clicked()
{
    if (!is_selecting())
        return false;

    return input->get_is_mouse_button_pressed(GLFW_MOUSE_BUTTON_1);
}

void Button::draw()
{
    TextRendering_PrintString(window, text, pos_start.x, pos_start.y, scale);
}

Hud::Hud(GLFWwindow *w, std::shared_ptr<Camera> *c)
{
    window = w;
    camera = c;

    debug_vendor = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
    debug_renderer = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
    debug_glversion = reinterpret_cast<const char*>(glGetString(GL_VERSION));
    debug_glslversion = reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION));
}

void Hud::toggle_debug_info(bool b)
{
    show_debug_info = b;
}

void Hud::toggle_debug_info()
{
    toggle_debug_info(!show_debug_info);
}

void Hud::update_timings()
{
    static float old_seconds = (float)glfwGetTime();
    static int ellapsed_frames = 0;

    ellapsed_frames += 1;

    // Recuperamos o número de segundos que passou desde a execução do programa
    float seconds = (float)glfwGetTime();

    // Número de segundos desde o último cálculo do fps
    float ellapsed_seconds = seconds - old_seconds;

    if (ellapsed_seconds > TIMINGS_UPDATE_INTERVAL)
    {
        fps = ellapsed_frames / ellapsed_seconds;
        frametime = 1000 * ellapsed_seconds / ellapsed_frames;

        old_seconds = seconds;
        ellapsed_frames = 0;
    }
}

void Hud::update(glm::vec2 cur, glm::vec4 cur_i)
{
    update_timings();

    cursor_pos = cur;
    cursor_intersection = cur_i;
}

void Hud::draw()
{
    glDisable(GL_DEPTH_TEST);

    if (show_debug_info)
        render_debug_info();

    glEnable(GL_DEPTH_TEST);
}

void Hud::render_debug_info()
{
    float lineheight = TextRendering_LineHeight(window);

    TextRendering_PrintString(window, std::format("GPU: {}, {}\n",
                                        debug_vendor, debug_renderer),
                              HUD_START, HUD_TOP - lineheight);
    TextRendering_PrintString(window, std::format("OpenGL {}, GLSL {}\n",
                                        debug_glversion, debug_glslversion),
                              HUD_START, HUD_TOP - 2*lineheight);

    TextRendering_PrintString(window, std::format("{:.2f} FPS", fps),
                              HUD_START, HUD_TOP - 4*lineheight, 1.25f);
    TextRendering_PrintString(window, std::format("Frametime: {:.2f} ms", frametime),
                              HUD_START, HUD_TOP - 5*lineheight);

    glm::vec4 cam_pos = camera->get()->get_position();

    TextRendering_PrintString(window, std::format("Camera position: X: {:.2f} Y: {:.2f} Z: {:.2f}",
                                        cam_pos.x, cam_pos.y, cam_pos.z),
                              HUD_START, HUD_TOP - 7*lineheight);

    TextRendering_PrintString(window, std::format("Cursor position: X: {:.2f} Y: {:.2f}",
                                        cursor_pos.x, cursor_pos.y),
                              HUD_START, HUD_TOP - 9*lineheight);

    TextRendering_PrintString(window, std::format("Cursor-Board intersection position: X: {:.2f} Y: {:.2f} Z: {:.2f}",
                                        cursor_intersection.x, cursor_intersection.y, cursor_intersection.z),
                              HUD_START, HUD_TOP - 10*lineheight);

    TextRendering_PrintString(window, camera->get()->is_projection_perspective() ? "Perspective" : "Orthographic",
                              HUD_START, HUD_BOTTOM + 2*lineheight/10);
}
