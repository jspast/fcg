#include <ostream>
#include <string_view>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>

#include <glad/gl.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "gpu.hpp"

GpuProgram::GpuProgram(std::string_view v_path, std::string_view f_path)
{
    load_shaders_from_files(v_path, f_path);
}

GpuProgram::GpuProgram(const GLchar* const vertex_shader_source,
                       const GLchar* const fragment_shader_source)
{
    load_shaders_from_source(vertex_shader_source, fragment_shader_source);
}

void GpuProgram::reload_shaders()
{
    load_shaders_from_files("../../src/shader_vertex.glsl", "../../src/shader_fragment.glsl");

    glUseProgram(id);
    for (size_t i = 0; i < texture_uniforms.size(); i++)
        glUniform1i(glGetUniformLocation(id, texture_uniforms[i].data()), i);
    glUseProgram(0);

    std::cout << "Shaders recarregados!" << std::endl;
}

// Carrega shaders de arquivos e cria programa de GPU utilizando-os
void GpuProgram::load_shaders_from_files(std::string_view v_path,
                                         std::string_view f_path)
{
    vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
    fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);

    // Carrega um Vertex Shader de um arquivo GLSL
    load_shader_from_file(v_path, vertex_shader_id);

    // Carrega um Fragment Shader de um arquivo GLSL
    load_shader_from_file(f_path, fragment_shader_id);

    // Deletamos o programa de GPU anterior, caso ele exista
    if (id != 0)
        glDeleteProgram(id);

    // Criamos um programa de GPU utilizando os shaders carregados acima
    create_program();
}

// Carrega shaders de arquivos e cria programa de GPU utilizando-os
void GpuProgram::load_shaders_from_source(const GLchar* const vertex_shader_source,
                                          const GLchar* const fragment_shader_source)
{
    vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
    fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);

    // Carrega um Vertex Shader de um arquivo GLSL
    load_shader_from_source(vertex_shader_source, vertex_shader_id);

    // Carrega um Fragment Shader de um arquivo GLSL
    load_shader_from_source(fragment_shader_source, fragment_shader_id);

    // Deletamos o programa de GPU anterior, caso ele exista
    if (id != 0)
        glDeleteProgram(id);

    // Criamos um programa de GPU utilizando os shaders carregados acima
    create_program();
}

// Carrega shader de arquivo para shader_id
void GpuProgram::load_shader_from_file(std::string_view filename, GLuint shader_id)
{
    // Lemos o arquivo de texto indicado pela variável "filename"
    // e colocamos seu conteúdo em memória, apontado pela variável
    // "shader_string".
    std::ifstream file;
    try {
        file.exceptions(std::ifstream::failbit);
        file.open(std::string(filename).c_str());
    } catch (std::exception& e) {
        std::cerr << "GpuProgram: Cannot open file" << filename << std::endl;
        std::exit(EXIT_FAILURE);
    }
    std::stringstream shader;
    shader << file.rdbuf();
    std::string str = shader.str();
    const GLchar* shader_string = str.c_str();
    const GLint   shader_string_length = static_cast<GLint>(str.length());

    std::string log_info = "File: " + std::string(filename) + "\n";

    load_shader_from_source(shader_string, shader_id, shader_string_length, log_info);
}

// Carrega shader de arquivo para shader_id
void GpuProgram::load_shader_from_source(const GLchar* const shader_string,
                                         GLuint shader_id,
                                         int shader_string_length,
                                         std::string log_info)
{
    // Define o código do shader GLSL, contido na string "shader_string"
    if (shader_string_length == -1)
        glShaderSource(shader_id, 1, &shader_string, NULL);
    else
        glShaderSource(shader_id, 1, &shader_string, &shader_string_length);

    // Compila o código do shader GLSL (em tempo de execução)
    glCompileShader(shader_id);

    // Verificamos se ocorreu algum erro ou "warning" durante a compilação
    GLint compiled_ok;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &compiled_ok);

    GLint log_length = 0;
    glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &log_length);

    // Alocamos memória para guardar o log de compilação.
    // A chamada "new" em C++ é equivalente ao "malloc()" do C.
    GLchar* log = new GLchar[log_length];
    glGetShaderInfoLog(shader_id, log_length, &log_length, log);

    // Imprime no terminal qualquer erro ou "warning" de compilação
    if ( log_length != 0 )
    {
        std::string  output;

        if ( !compiled_ok )
        {
            output += "ERROR: OpenGL compilation failed.\n";
            output += log_info;
            output += "== Start of compilation log\n";
            output += log;
            output += "== End of compilation log\n";
        }
        else
        {
            output += "ERROR: OpenGL compilation failed.\n";
            output += log_info;
            output += "== Start of compilation log\n";
            output += log;
            output += "== End of compilation log\n";
        }

        fprintf(stderr, "%s", output.c_str());
    }

    // A chamada "delete" em C++ é equivalente ao "free()" do C
    delete [] log;
}

void GpuProgram::create_program()
{
    // Criamos um identificador (ID) para este programa de GPU
    id = glCreateProgram();

    // Definição dos dois shaders GLSL que devem ser executados pelo programa
    glAttachShader(id, vertex_shader_id);
    glAttachShader(id, fragment_shader_id);

    // Linkagem dos shaders acima ao programa
    glLinkProgram(id);

    // Verificamos se ocorreu algum erro durante a linkagem
    GLint linked_ok = GL_FALSE;
    glGetProgramiv(id, GL_LINK_STATUS, &linked_ok);

    // Imprime no terminal qualquer erro de linkagem
    if ( linked_ok == GL_FALSE )
    {
        GLint log_length = 0;
        glGetProgramiv(id, GL_INFO_LOG_LENGTH, &log_length);

        // Alocamos memória para guardar o log de compilação
        GLchar* log = new GLchar[log_length];

        glGetProgramInfoLog(id, log_length, &log_length, log);

        std::string output;

        output += "ERROR: OpenGL linking of program failed.\n";
        output += "== Start of link log\n";
        output += log;
        output += "\n== End of link log\n";

        // A chamada "delete" em C++ é equivalente ao "free()" do C
        delete [] log;

        fprintf(stderr, "%s", output.c_str());
    }
}

void GpuProgram::set_uniform(std::string_view name, float value)
{
    glUseProgram(id);
    glUniform1f(get_uniform_location(name), value);
    glUseProgram(0);
}

void GpuProgram::set_uniform(std::string_view name, int value)
{
    glUseProgram(id);
    glUniform1i(get_uniform_location(name), value);
    glUseProgram(0);
}

void GpuProgram::set_uniform(std::string_view name, glm::vec2 value)
{
    glUseProgram(id);
    glUniform2fv(get_uniform_location(name), 1, glm::value_ptr(value));
    glUseProgram(0);
}

void GpuProgram::set_uniform(std::string_view name, glm::vec4 value)
{
    glUseProgram(id);
    glUniform4fv(get_uniform_location(name), 1, glm::value_ptr(value));
    glUseProgram(0);
}

void GpuProgram::set_uniform(std::string_view name, glm::mat4 value)
{
    glUseProgram(id);
    glUniformMatrix4fv(get_uniform_location(name), 1, GL_FALSE, glm::value_ptr(value));
    glUseProgram(0);
}

GLint GpuProgram::get_uniform_location(std::string_view name)
{
    return glGetUniformLocation(id, name.data());
}

void GpuProgram::load_cubemap_from_hdr_files(std::vector<std::string_view> filename,
                                             std::string_view uniform)
{
    printf("Carregando texturas de cubemap...");

    // Primeiro fazemos a leitura da imagem do disco
    stbi_set_flip_vertically_on_load(false);

    // Agora criamos objetos na GPU com OpenGL para armazenar a textura
    GLuint texture_id;
    GLuint textureunit = num_uploaded_textures;
    glGenTextures(1, &texture_id);
    glActiveTexture(GL_TEXTURE0 + textureunit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);

    for (int i = 0; i < 6; i++)
    {
        int width, height, channels;
        float *data = stbi_loadf(filename[i].data(), &width, &height, &channels, 3);

        if ( data == NULL )
        {
            fprintf(stderr, "ERROR: Cannot open image file \"%s\".\n", filename[i].data());
            std::exit(EXIT_FAILURE);
        }

        printf("OK (%dx%d), ", width, height);

        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data);

        stbi_image_free(data);
    }

    printf(".\n");

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glUseProgram(id);
    glUniform1i(glGetUniformLocation(id, uniform.data()), num_uploaded_textures);
    glUseProgram(0);

    texture_uniforms.push_back(uniform);

    num_loaded_textures++;
    num_uploaded_textures++;
}

void GpuProgram::load_textures_async(std::vector<std::pair<std::string_view, std::string_view>> textures)
{
    stbi_set_flip_vertically_on_load(true);

    for (const auto& [filepath, uniform] : textures) {

        num_loaded_textures++;

        tex_futures.emplace_back(std::async(std::launch::async, [filepath, uniform]() {

            int w, h, c;
            unsigned char* data = stbi_load(filepath.data(), &w, &h, &c, 0);

            if (!data)
                throw std::runtime_error( "ERROR: Cannot open image file \"" + std::string(filepath) + "\".");

            std::cout << "Carregando imagem \"" << filepath << "\" ... OK (" << w << "x" << h << ")." << std::endl;

            TextureData result;
            result.uniform_name = uniform;
            result.width = w;
            result.height = h;
            result.channels = c;
            result.data = data;
            return result;
        }));
    }
}

bool GpuProgram::upload_pending_textures()
{
    for (auto it = tex_futures.begin(); it != tex_futures.end(); ) {
        if (it->wait_for(std::chrono::milliseconds(0)) == std::future_status::ready) {
            tex_queue.push(it->get());
            it = tex_futures.erase(it);
        } else {
            ++it;
        }
    }

    // Upload ready textures to GPU
    while (!tex_queue.empty()) {
        const auto& tex = tex_queue.front();

        // Agora criamos objetos na GPU com OpenGL para armazenar a textura
        GLuint texture_id;
        GLuint sampler_id;
        glGenTextures(1, &texture_id);
        glGenSamplers(1, &sampler_id);

        // Veja slides 95-96 do documento Aula_20_Mapeamento_de_Texturas.pdf
        glSamplerParameteri(sampler_id, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glSamplerParameteri(sampler_id, GL_TEXTURE_WRAP_T, GL_REPEAT);

        // Parâmetros de amostragem da textura.
        glSamplerParameteri(sampler_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glSamplerParameteri(sampler_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glSamplerParameterf(sampler_id, GL_TEXTURE_MAX_ANISOTROPY_EXT, 8.0f);

        // Agora enviamos a imagem lida do disco para a GPU
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
        glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
        glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);

        GLuint textureunit = num_uploaded_textures;
        glActiveTexture(GL_TEXTURE0 + textureunit);
        glBindTexture(GL_TEXTURE_2D, texture_id);
        glTexImage2D(GL_TEXTURE_2D, 0,
                     (tex.channels > 1) ? GL_SRGB8 : GL_R8,
                     tex.width, tex.height, 0,
                     (tex.channels > 1) ? GL_RGB : GL_RED,
                     GL_UNSIGNED_BYTE, tex.data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindSampler(textureunit, sampler_id);

        stbi_image_free(tex.data);

        glUseProgram(id);
        glUniform1i(glGetUniformLocation(id,
                                         tex.uniform_name.data()),
                                         num_uploaded_textures);
        glUseProgram(0);

        texture_uniforms.push_back(tex.uniform_name);

        num_uploaded_textures++;

        tex_queue.pop();
    }

    return tex_futures.empty() && tex_queue.empty();
}
