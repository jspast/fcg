//     Universidade Federal do Rio Grande do Sul
//             Instituto de Informática
//       Departamento de Informática Aplicada
//
//    INF01047 Fundamentos de Computação Gráfica
//               Prof. Eduardo Gastal
//
//                   LABORATÓRIO 2
//

// Arquivos "headers" padrões de C podem ser incluídos em um
// programa C++, sendo necessário somente adicionar o caractere
// "c" antes de seu nome, e remover o sufixo ".h". Exemplo:
//    #include <stdio.h> // Em C
//  vira
//    #include <cstdio> // Em C++
//
#define _USE_MATH_DEFINES
#include <cmath>
#include <cstdio>
#include <cstdlib>

// Headers abaixo são específicos de C++
#include <map>
#include <new>
#include <string>
#include <limits>
#include <fstream>
#include <sstream>

#include "object.hpp"

// Headers das bibliotecas OpenGL
#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>   // Criação de contexto OpenGL 3.3
#include <GLFW/glfw3.h>  // Criação de janelas do sistema operacional

// Headers da biblioteca GLM: criação de matrizes e vetores.
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

// Headers locais, definidos na pasta "include/"
#include "utils.h"
#include "matrices.hpp"
#include "camera.hpp"
#include "hud.hpp"
#include "window.hpp"
#include "input.hpp"
#include "textrendering.hpp"

// Declaração de várias funções utilizadas em main().  Essas estão definidas
// logo após a definição de main() neste arquivo.
void LoadShadersFromFiles(); // Carrega os shaders de vértice e fragmento, criando um programa de GPU
GLuint LoadShader_Vertex(const char* filename);   // Carrega um vertex shader
GLuint LoadShader_Fragment(const char* filename); // Carrega um fragment shader
void LoadShader(const char* filename, GLuint shader_id); // Função utilizada pelas duas acima
GLuint CreateGpuProgram(GLuint vertex_shader_id, GLuint fragment_shader_id); // Cria um programa de GPU

void FramebufferSizeCallback(GLFWwindow* window, int width, int height);

void print_system_info();

// Variáveis que definem um programa de GPU (shaders). Veja função LoadShadersFromFiles().
GLuint g_GpuProgramID = 0;

int main()
{
    FreeCamera free_camera;
    LookAtCamera lookat_camera;
    Camera *camera = &free_camera;

    glfwSetErrorCallback(glfw_error_callback);

    int success = glfwInit();
    if (!success)
        std::exit(EXIT_FAILURE);

    Window *window = new Window("INF01047 - Trabalho Final");

    // Definimos a função de callback que será chamada sempre que a janela for
    // redimensionada, por consequência alterando o tamanho do "framebuffer"
    // (região de memória onde são armazenados os pixels da imagem).
    window->set_user_pointer(camera);
    window->set_framebuffer_size_callback(FramebufferSizeCallback);

    InputManager base_input(window->glfw_window,
                            {GLFW_KEY_F3,
                            GLFW_KEY_F11,
                            GLFW_KEY_ESCAPE},
                            {},
                            {GLFW_GAMEPAD_BUTTON_START},
                            {});
    InputManager game_input(window->glfw_window,
                            {GLFW_KEY_W,
                            GLFW_KEY_A,
                            GLFW_KEY_S,
                            GLFW_KEY_D,
                            GLFW_KEY_L,
                            GLFW_KEY_F,
                            GLFW_KEY_P,
                            GLFW_KEY_O},
                            {},
                            {},
                            {GLFW_GAMEPAD_AXIS_LEFT_X,
                            GLFW_GAMEPAD_AXIS_LEFT_Y,
                            GLFW_GAMEPAD_AXIS_RIGHT_X,
                            GLFW_GAMEPAD_AXIS_RIGHT_Y,
                            GLFW_GAMEPAD_AXIS_LEFT_TRIGGER,
                            GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER});

    // Carregamento de todas funções definidas por OpenGL 3.3, utilizando a
    // biblioteca GLAD.
    gladLoadGL(glfwGetProcAddress);

    Hud hud(window->glfw_window);

    print_system_info();

    // Carregamos os shaders de vértices e de fragmentos que serão utilizados
    // para renderização. Veja slides 180-200 do documento Aula_03_Rendering_Pipeline_Grafico.pdf.
    //
    LoadShadersFromFiles();

    ObjModel bunny_model("../../data/models/bunny.obj");
    // ObjModel bunny_model("../../data/models/pieces/black/pawn.obj");
    bunny_model.object_id = 1;
    bunny_model.compute_normals();
    bunny_model.build_triangles();

    Object* bunnies[32];

    for (int i = 0; i < 8; i++) {
        bunnies[i] = new Object(bunny_model, Matrix_Translate(i * 2, 0, 0));
    }
    for (int i = 0; i < 8; i++) {
        bunnies[i + 8] = new Object(bunny_model, Matrix_Translate(i * 2, 0, 2));
    }
    for (int i = 0; i < 8; i++) {
        bunnies[i + 16] = new Object(bunny_model, Matrix_Translate(i * 2, 0, 12));
    }
    for (int i = 0; i < 8; i++) {
        bunnies[i + 24] = new Object(bunny_model, Matrix_Translate(i * 2, 0, 14));
    }

    // Inicializamos o código para renderização de texto.
    TextRendering_Init();

    // Buscamos o endereço das variáveis definidas dentro do Vertex Shader.
    // Utilizaremos estas variáveis para enviar dados para a placa de vídeo
    // (GPU)! Veja arquivo "shader_vertex.glsl".
    GLint model_uniform           = glGetUniformLocation(g_GpuProgramID, "model"); // Variável da matriz "model"
    GLint view_uniform            = glGetUniformLocation(g_GpuProgramID, "view"); // Variável da matriz "view" em shader_vertex.glsl
    GLint projection_uniform      = glGetUniformLocation(g_GpuProgramID, "projection"); // Variável da matriz "projection" em shader_vertex.glsl
    GLint object_id_uniform       = glGetUniformLocation(g_GpuProgramID, "object_id"); // Variável "object_id" em shader_fragment.glsl

    // Habilitamos o Z-buffer. Veja slides 104-116 do documento Aula_09_Projecoes.pdf.
    glEnable(GL_DEPTH_TEST);

    // Habilitamos o backface culling
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    camera->set_position(2.5f, 2.5f, 2.5f);
    camera->set_angles(-M_PI * 0.75, M_PI / 6);

    float delta_t;
    float current_time;
    float prev_time = (float)glfwGetTime();

    // Ficamos em um loop infinito, renderizando, até que o usuário feche a janela
    while (!glfwWindowShouldClose(window->glfw_window))
    {
        // Aqui executamos as operações de renderização

        // Definimos a cor do "fundo" do framebuffer como branco.  Tal cor é
        // definida como coeficientes RGBA: Red, Green, Blue, Alpha; isto é:
        // Vermelho, Verde, Azul, Alpha (valor de transparência).
        // Conversaremos sobre sistemas de cores nas aulas de Modelos de Iluminação.
        //
        //           R     G     B     A
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

        // "Pintamos" todos os pixels do framebuffer com a cor definida acima,
        // e também resetamos todos os pixels do Z-buffer (depth buffer).
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Pedimos para a GPU utilizar o programa de GPU criado acima (contendo
        // os shaders de vértice e fragmentos).
        glUseProgram(g_GpuProgramID);

        // Atualiza delta de tempo
        current_time = (float)glfwGetTime();
        delta_t = current_time - prev_time;
        prev_time = current_time;

        if (base_input.get_is_key_pressed(GLFW_KEY_F3))
            hud.toggle_debug_info();

        if (base_input.get_is_key_pressed(GLFW_KEY_F11))
            window->toggle_fullscreen();

        if (base_input.get_is_key_pressed(GLFW_KEY_ESCAPE) ||
            base_input.get_is_gamepad_button_pressed(GLFW_JOYSTICK_1, GLFW_GAMEPAD_BUTTON_START))
        {
            game_input.set_is_enabled(!game_input.get_is_enabled());
            window->toggle_cursor();
        }

        if (game_input.get_is_key_pressed(GLFW_KEY_L)) {
            lookat_camera = build_lookat_camera(camera, 3.0f);
            camera = &lookat_camera;
        }

        if (game_input.get_is_key_pressed(GLFW_KEY_F)) {
            free_camera = build_free_camera(camera);
            camera = &free_camera;
        }

        if (game_input.get_is_key_pressed(GLFW_KEY_P))
            camera->toggle_perspective_projection(true);

        if (game_input.get_is_key_pressed(GLFW_KEY_O))
            camera->toggle_perspective_projection(false);

        if (game_input.get_is_key_down(GLFW_KEY_W))
            free_camera.move(1.0f * delta_t, 0.0f);

        if (game_input.get_is_key_down(GLFW_KEY_S))
            free_camera.move(-1.0f * delta_t, 0.0f);

        if (game_input.get_is_key_down(GLFW_KEY_A))
            free_camera.move(0.0f, 1.0f * delta_t);

        if (game_input.get_is_key_down(GLFW_KEY_D))
            free_camera.move(0.0f, -1.0f * delta_t);

        // Atualizamos parâmetros da câmera com os deslocamentos
        glm::vec2 cursor_movement = game_input.get_cursor_movement();
        camera->adjust_angles(-0.001f * cursor_movement.x, 0.001f * cursor_movement.y);

        glm::vec2 scroll = game_input.get_scroll_offset();
        camera->adjust_orthographic_zoom(0.1 * scroll.y);
        lookat_camera.adjust_distance(-0.5 * scroll.y);

        free_camera.move(-delta_t * game_input.get_gamepad_axis_value(GLFW_JOYSTICK_1,
                                                                      GLFW_GAMEPAD_AXIS_LEFT_Y),
                         -delta_t * game_input.get_gamepad_axis_value(GLFW_JOYSTICK_1,
                                                                      GLFW_GAMEPAD_AXIS_LEFT_X));

        camera->adjust_angles(-delta_t * game_input.get_gamepad_axis_value(GLFW_JOYSTICK_1,
                                                                         GLFW_GAMEPAD_AXIS_RIGHT_X),
                              delta_t * game_input.get_gamepad_axis_value(GLFW_JOYSTICK_1,
                                                                        GLFW_GAMEPAD_AXIS_RIGHT_Y));

        lookat_camera.adjust_distance(delta_t * (game_input.get_gamepad_axis_value(GLFW_JOYSTICK_1,
                                                                                   GLFW_GAMEPAD_AXIS_LEFT_TRIGGER) -
                                                 game_input.get_gamepad_axis_value(GLFW_JOYSTICK_1,
                                                                                   GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER)));
        base_input.update();
        game_input.update();

        // Computamos a matriz "View" utilizando os parâmetros da câmera para
        // definir o sistema de coordenadas da câmera.  Veja slides 2-14, 184-190 e 236-242 do documento Aula_08_Sistemas_de_Coordenadas.pdf.
        glm::mat4 view = camera->get_view_matrix();
        glm::mat4 projection = camera->get_projection_matrix();

        // Enviamos as matrizes "view" e "projection" para a placa de vídeo
        // (GPU). Veja o arquivo "shader_vertex.glsl", onde estas são
        // efetivamente aplicadas em todos os pontos.
        glUniformMatrix4fv(view_uniform       , 1 , GL_FALSE , glm::value_ptr(view));
        glUniformMatrix4fv(projection_uniform , 1 , GL_FALSE , glm::value_ptr(projection));

        for (int i = 0; i < 32; i++)
            bunnies[i]->draw(model_uniform, object_id_uniform);

        hud.update(camera->is_projection_perspective());

        // O framebuffer onde OpenGL executa as operações de renderização não
        // é o mesmo que está sendo mostrado para o usuário, caso contrário
        // seria possível ver artefatos conhecidos como "screen tearing". A
        // chamada abaixo faz a troca dos buffers, mostrando para o usuário
        // tudo que foi renderizado pelas funções acima.
        // Veja o link: https://en.wikipedia.org/w/index.php?title=Multiple_buffering&oldid=793452829#Double_buffering_in_computer_graphics
        glfwSwapBuffers(window->glfw_window);

        // Verificamos com o sistema operacional se houve alguma interação do
        // usuário (teclado, mouse, ...). Caso positivo, as funções de callback
        // definidas anteriormente usando glfwSet*Callback() serão chamadas
        // pela biblioteca GLFW.
        glfwPollEvents();
    }

    delete window;

    // Finalizamos o uso dos recursos do sistema operacional
    glfwTerminate();

    // Fim do programa
    return 0;
}

// Carrega um Vertex Shader de um arquivo GLSL. Veja definição de LoadShader() abaixo.
GLuint LoadShader_Vertex(const char* filename)
{
    // Criamos um identificador (ID) para este shader, informando que o mesmo
    // será aplicado nos vértices.
    GLuint vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);

    // Carregamos e compilamos o shader
    LoadShader(filename, vertex_shader_id);

    // Retorna o ID gerado acima
    return vertex_shader_id;
}

// Carrega um Fragment Shader de um arquivo GLSL . Veja definição de LoadShader() abaixo.
GLuint LoadShader_Fragment(const char* filename)
{
    // Criamos um identificador (ID) para este shader, informando que o mesmo
    // será aplicado nos fragmentos.
    GLuint fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);

    // Carregamos e compilamos o shader
    LoadShader(filename, fragment_shader_id);

    // Retorna o ID gerado acima
    return fragment_shader_id;
}

// Função auxilar, utilizada pelas duas funções acima. Carrega código de GPU de
// um arquivo GLSL e faz sua compilação.
void LoadShader(const char* filename, GLuint shader_id)
{
    // Lemos o arquivo de texto indicado pela variável "filename"
    // e colocamos seu conteúdo em memória, apontado pela variável
    // "shader_string".
    std::ifstream file;
    try {
        file.exceptions(std::ifstream::failbit);
        file.open(filename);
    } catch ( std::exception& e ) {
        fprintf(stderr, "ERROR: Cannot open file \"%s\".\n", filename);
        std::exit(EXIT_FAILURE);
    }
    std::stringstream shader;
    shader << file.rdbuf();
    std::string str = shader.str();
    const GLchar* shader_string = str.c_str();
    const GLint   shader_string_length = static_cast<GLint>( str.length() );

    // Define o código do shader GLSL, contido na string "shader_string"
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
            output += "ERROR: OpenGL compilation of \"";
            output += filename;
            output += "\" failed.\n";
            output += "== Start of compilation log\n";
            output += log;
            output += "== End of compilation log\n";
        }
        else
        {
            output += "WARNING: OpenGL compilation of \"";
            output += filename;
            output += "\".\n";
            output += "== Start of compilation log\n";
            output += log;
            output += "== End of compilation log\n";
        }

        fprintf(stderr, "%s", output.c_str());
    }

    // A chamada "delete" em C++ é equivalente ao "free()" do C
    delete [] log;
}

// Função que carrega os shaders de vértices e de fragmentos que serão
// utilizados para renderização. Veja slides 180-200 do documento Aula_03_Rendering_Pipeline_Grafico.pdf.
//
void LoadShadersFromFiles()
{
    // Note que o caminho para os arquivos "shader_vertex.glsl" e
    // "shader_fragment.glsl" estão fixados, sendo que assumimos a existência
    // da seguinte estrutura no sistema de arquivos:
    //
    //    + FCG_Lab_01/
    //    |
    //    +--+ bin/
    //    |  |
    //    |  +--+ Release/  (ou Debug/ ou Linux/)
    //    |     |
    //    |     o-- main.exe
    //    |
    //    +--+ src/
    //       |
    //       o-- shader_vertex.glsl
    //       |
    //       o-- shader_fragment.glsl
    //
    GLuint vertex_shader_id = LoadShader_Vertex("../../src/shader_vertex.glsl");
    GLuint fragment_shader_id = LoadShader_Fragment("../../src/shader_fragment.glsl");

    // Deletamos o programa de GPU anterior, caso ele exista.
    if ( g_GpuProgramID != 0 )
        glDeleteProgram(g_GpuProgramID);

    // Criamos um programa de GPU utilizando os shaders carregados acima.
    g_GpuProgramID = CreateGpuProgram(vertex_shader_id, fragment_shader_id);
}

// Esta função cria um programa de GPU, o qual contém obrigatoriamente um
// Vertex Shader e um Fragment Shader.
GLuint CreateGpuProgram(GLuint vertex_shader_id, GLuint fragment_shader_id)
{
    // Criamos um identificador (ID) para este programa de GPU
    GLuint program_id = glCreateProgram();

    // Definição dos dois shaders GLSL que devem ser executados pelo programa
    glAttachShader(program_id, vertex_shader_id);
    glAttachShader(program_id, fragment_shader_id);

    // Linkagem dos shaders acima ao programa
    glLinkProgram(program_id);

    // Verificamos se ocorreu algum erro durante a linkagem
    GLint linked_ok = GL_FALSE;
    glGetProgramiv(program_id, GL_LINK_STATUS, &linked_ok);

    // Imprime no terminal qualquer erro de linkagem
    if ( linked_ok == GL_FALSE )
    {
        GLint log_length = 0;
        glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &log_length);

        // Alocamos memória para guardar o log de compilação.
        // A chamada "new" em C++ é equivalente ao "malloc()" do C.
        GLchar* log = new GLchar[log_length];

        glGetProgramInfoLog(program_id, log_length, &log_length, log);

        std::string output;

        output += "ERROR: OpenGL linking of program failed.\n";
        output += "== Start of link log\n";
        output += log;
        output += "\n== End of link log\n";

        // A chamada "delete" em C++ é equivalente ao "free()" do C
        delete [] log;

        fprintf(stderr, "%s", output.c_str());
    }

    // Retornamos o ID gerado acima
    return program_id;
}

// Definição da função que será chamada sempre que a janela do sistema
// operacional for redimensionada, por consequência alterando o tamanho do
// "framebuffer" (região de memória onde são armazenados os pixels da imagem).
void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    Camera *camera = reinterpret_cast<Camera*>(glfwGetWindowUserPointer(window));

    // Indicamos que queremos renderizar em toda região do framebuffer. A
    // função "glViewport" define o mapeamento das "normalized device
    // coordinates" (NDC) para "pixel coordinates".  Essa é a operação de
    // "Screen Mapping" ou "Viewport Mapping" vista em aula ({+ViewportMapping2+}).
    glViewport(0, 0, width, height);

    // Atualizamos também a razão que define a proporção da janela (largura /
    // altura), a qual será utilizada na definição das matrizes de projeção,
    // tal que não ocorra distorções durante o processo de "Screen Mapping"
    // acima, quando NDC é mapeado para coordenadas de pixels. Veja slides 205-215 do documento Aula_09_Projecoes.pdf.
    //
    // O cast para float é necessário pois números inteiros são arredondados ao
    // serem divididos!
    camera->set_aspect_ratio((float)width / height);
}

void print_system_info()
{
    const GLubyte *vendor      = glGetString(GL_VENDOR);
    const GLubyte *renderer    = glGetString(GL_RENDERER);
    const GLubyte *glversion   = glGetString(GL_VERSION);
    const GLubyte *glslversion = glGetString(GL_SHADING_LANGUAGE_VERSION);

    printf("GPU: %s, %s, OpenGL %s, GLSL %s\n", vendor, renderer, glversion, glslversion);
}

// set makeprg=cd\ ..\ &&\ make\ run\ >/dev/null
// vim: set spell spelllang=pt_br :
