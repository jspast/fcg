#include <format>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "hud.hpp"
#include "glm/ext/vector_float4.hpp"
#include "textrendering.hpp"

#define TIMINGS_UPDATE_INTERVAL 1.0f

#define BORDER_MARGIN (charwidth)

#define HUD_TOP (1.0f - BORDER_MARGIN)
#define HUD_BOTTOM (-1.0f + BORDER_MARGIN)
#define HUD_START (-1.0f + BORDER_MARGIN)
#define HUD_END (1.0f - BORDER_MARGIN)

Hud::Hud(GLFWwindow *w)
{
    window = w;

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

void Hud::update(Camera& c)
{
    update_timings();

    if (show_debug_info)
        render_debug_info(c);
}

void Hud::render_debug_info(Camera& c)
{
    float lineheight = TextRendering_LineHeight(window);
    float charwidth = TextRendering_CharWidth(window);

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

    glm::vec4 cam_pos = c.get_position();

    TextRendering_PrintString(window, std::format("Camera position: X: {:.2f} Y: {:.2f} Z: {:.2f}",
                                        cam_pos.x, cam_pos.y, cam_pos.z),
                              HUD_START, HUD_TOP - 7*lineheight);

    TextRendering_PrintString(window, c.is_projection_perspective() ? "Perspective" : "Orthographic",
                              HUD_START, HUD_BOTTOM + 2*lineheight/10);
}
