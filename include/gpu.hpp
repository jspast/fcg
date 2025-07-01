#pragma once

#include <future>
#include <map>
#include <queue>
#include <string_view>
#include <string>

#include <glm/mat4x4.hpp>

#include <glad/gl.h>

#define BOARD 0
#define PIECE 1
#define TABLE 2
#define SKY 3
#define FLOOR 4

#define PIECE_WHITE 0
#define PIECE_BLACK 1

#define PIECE_NONE 0
#define SELECTING 1
#define SELECTED 2
#define LAST_MOVE 3
#define AVAILABLE_MOVE 4
#define AVAILABLE_CAPTURE 5
#define CHECK 6

#define SQUARE_SIZE (0.05789)
#define BOARD_START (-4 * SQUARE_SIZE)
#define G_SQUARE_SIZE (SQUARE_SIZE * 1.5)
#define G_BOARD_START (-4 * G_SQUARE_SIZE)

struct TextureData {
    std::string_view uniform_name;
    unsigned char* data;
    int width = 0;
    int height = 0;
    int channels = 3;
};

class GpuProgram {
    private:
        std::map<std::string, GLint> uniform_locations;

        GLuint vertex_shader_id;
        GLuint fragment_shader_id;

        static void load_shader_from_file(std::string_view filename, GLuint shader_id);
        static void load_shader_from_source(const GLchar* const shader_string,
                                            GLuint shader_id,
                                            int shader_string_length = -1,
                                            std::string log_info = "");

        void create_program();

        std::vector<std::future<TextureData>> tex_futures;
        std::queue<TextureData> tex_queue;

        std::vector<std::string_view> texture_uniforms;

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

        void reload_shaders();

        void set_uniform(std::string_view name, float value);
        void set_uniform(std::string_view name, int value);
        void set_uniform(std::string_view name, glm::vec2 value);
        void set_uniform(std::string_view name, glm::vec4 value);
        void set_uniform(std::string_view name, glm::mat4 value);

        GLint get_uniform_location(std::string_view name);

        void load_cubemap_from_hdr_files(std::vector<std::string_view> filename,
                                         std::string_view uniform);

        // Load textures from a vector of pairs: (filepath, uniform_name)
        // Should be called once, upload is done through upload_pending_textures()
        void load_textures_async(std::vector<std::pair<std::string_view, std::string_view>> textures);

        // Upload textures loaded async, should be called in the main loop
        // Returns true when all textures have been uploaded
        bool upload_pending_textures();

        GLuint num_loaded_textures = 0;
        GLuint num_uploaded_textures = 0;
};
