#pragma once

#include <map>
#include <string_view>
#include <string>

#include <glm/mat4x4.hpp>

#include <glad/gl.h>

#define BOARD 0
#define PIECE 1
#define TABLE 2

#define LIGHT 0
#define DARK 1

#define NONE 0
#define SELECTING 1
#define SELECTED 2
#define LAST_MOVE 3
#define AVAILABLE_MOVE 4
#define AVAILABLE_CAPTURE 5
#define CHECK 6

class GpuProgram {
    private:
        std::map<std::string, GLint> uniform_locations;

        GLuint vertex_shader_id;
        GLuint fragment_shader_id;

        GLuint num_loaded_textures = 0;
        std::vector<std::string_view> texture_uniforms;

        static void load_shader_from_file(std::string_view filename, GLuint shader_id);
        static void load_shader_from_source(const GLchar* const shader_string,
                                            GLuint shader_id,
                                            int shader_string_length = -1,
                                            std::string log_info = "");

        void create_program();

    public:
        GLint id = 0;

        GpuProgram(std::string_view vertex_shader_path = "../../src/shader_vertex.glsl",
                   std::string_view fragment_shader_path = "../../src/shader_fragment.glsl");

        GpuProgram(const GLchar* const vertex_shader_source,
                   const GLchar* const fragment_shader_source);

        void load_shaders_from_files(std::string_view vertex_shader_path,
                                     std::string_view fragment_shader_path);
        void load_shaders_from_source(const GLchar* const vertex_shader_source,
                                      const GLchar* const fragment_shader_source);

        void set_uniform(std::string_view name, float value);
        void set_uniform(std::string_view name, int value);
        void set_uniform(std::string_view name, glm::vec2 value);
        void set_uniform(std::string_view name, glm::vec4 value);
        void set_uniform(std::string_view name, glm::mat4 value);

        GLint get_uniform_location(std::string_view name);

        void load_texture_from_file(std::string_view filename,
                                    std::string_view uniform);

        void reload_shaders();
};
