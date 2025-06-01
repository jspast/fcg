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
#include "chess.hpp"
#include "chess_game.hpp"
#include "chess_renderer.hpp"
#include "gpu.hpp"

#define _USE_MATH_DEFINES
#include <cstdio>
#include <cstdlib>

// Headers abaixo são específicos de C++
#include <new>
#include <string>

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

void FramebufferSizeCallback(GLFWwindow* window, int width, int height);

void print_system_info();

int main()
{
    FreeCamera free_camera;
    LookAtCamera lookat_camera;
    Camera *camera = &lookat_camera;

    lookat_camera.set_target_position(4.0f, 0.0f, 4.0f);
    lookat_camera.set_distance(10.0f);

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
                            GLFW_KEY_O,
                            GLFW_KEY_UP,
                            GLFW_KEY_LEFT,
                            GLFW_KEY_DOWN,
                            GLFW_KEY_RIGHT,
                            GLFW_KEY_ENTER},
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

    GpuProgram gpu_program = GpuProgram();

    Hud hud(window->glfw_window);

    print_system_info();

    ObjModel bunny_model("../../data/models/bunny.obj");
    bunny_model.compute_normals();
    bunny_model.build_triangles();

    Object* bunnies[32];

    for (int i = 0; i < 8; i++) {
        bunnies[i] = new Object(bunny_model, gpu_program);
        bunnies[i]->set_transform(Matrix_Translate(0.5, 0.5, i + 0.5) *
                                  Matrix_Scale(0.5, 0.5, 0.5) *
                                  Matrix_Rotate_Y(M_PI));
        bunnies[i]->set_uniform("object_id", PIECE);
        bunnies[i]->set_uniform("object_color", LIGHT);
    }
    for (int i = 0; i < 8; i++) {
        bunnies[i + 8] = new Object(bunny_model, gpu_program);
        bunnies[i + 8]->set_transform(Matrix_Translate(1.5, 0.5, i + 0.5) *
                                      Matrix_Scale(0.5, 0.5, 0.5) *
                                      Matrix_Rotate_Y(M_PI));
        bunnies[i + 8]->set_uniform("object_id", PIECE);
        bunnies[i + 8]->set_uniform("object_color", LIGHT);
    }
    for (int i = 0; i < 8; i++) {
        bunnies[i + 16] = new Object(bunny_model, gpu_program);
        bunnies[i + 16]->set_transform(Matrix_Translate(6.5, 0.5, i + 0.5) *
                                       Matrix_Scale(0.5, 0.5, 0.5));
        bunnies[i + 16]->set_uniform("object_id", PIECE);
        bunnies[i + 16]->set_uniform("object_color", DARK);
    }
    for (int i = 0; i < 8; i++) {
        bunnies[i + 24] = new Object(bunny_model, gpu_program);
        bunnies[i + 24]->set_transform(Matrix_Translate(7.5, 0.5, i + 0.5) *
                                       Matrix_Scale(0.5, 0.5, 0.5));
        bunnies[i + 24]->set_uniform("object_id", PIECE);
        bunnies[i + 24]->set_uniform("object_color", DARK);
    }

    // Inicializamos o código para renderização de texto.
    TextRendering_Init();

    // Buscamos o endereço das variáveis definidas dentro do Vertex Shader.
    // Utilizaremos estas variáveis para enviar dados para a placa de vídeo
    // (GPU)! Veja arquivo "shader_vertex.glsl".
    GLint view_uniform            = gpu_program.get_uniform_location("view");
    GLint projection_uniform      = gpu_program.get_uniform_location("projection");

    ChessGame chess_game = ChessGame();
    ChessRenderer chess_renderer(chess_game, gpu_program);

    // Habilitamos o Z-buffer. Veja slides 104-116 do documento Aula_09_Projecoes.pdf.
    glEnable(GL_DEPTH_TEST);

    // Habilitamos o backface culling
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

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
        glUseProgram(gpu_program.id);

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
            lookat_camera = build_lookat_camera(camera, glm::vec4(4.0f, 0.0f, 4.0f, 1.0f), 10.0f);
            camera = &lookat_camera;
            window->set_user_pointer(camera);
        }

        if (game_input.get_is_key_pressed(GLFW_KEY_F)) {
            free_camera = build_free_camera(camera);
            camera = &free_camera;
            window->set_user_pointer(camera);
        }

        if (game_input.get_is_key_pressed(GLFW_KEY_UP))
            chess_renderer.move_selecting_square(chess::Direction::NORTH);

        if (game_input.get_is_key_pressed(GLFW_KEY_DOWN))
            chess_renderer.move_selecting_square(chess::Direction::SOUTH);

        if (game_input.get_is_key_pressed(GLFW_KEY_LEFT))
            chess_renderer.move_selecting_square(chess::Direction::WEST);

        if (game_input.get_is_key_pressed(GLFW_KEY_RIGHT))
            chess_renderer.move_selecting_square(chess::Direction::EAST);

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
            bunnies[i]->draw();

        chess_renderer.draw();

        hud.update(*camera);

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
