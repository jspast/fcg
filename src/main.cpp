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
#include <cstdio>
#include <cstdlib>

// Headers abaixo são específicos de C++
#include <memory>

#include "gpu.hpp"

// Headers das bibliotecas OpenGL
#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>   // Criação de contexto OpenGL 3.3
#include <GLFW/glfw3.h>  // Criação de janelas do sistema operacional

// Headers da biblioteca GLM: criação de matrizes e vetores.
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/vec2.hpp>
#include <glm/gtc/type_ptr.hpp>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

// Headers locais, definidos na pasta "include/"
#include "camera.hpp"
#include "window.hpp"
#include "textrendering.hpp"
#include "state.hpp"
#include "states/base.hpp"

void FramebufferSizeCallback(GLFWwindow* window, int width, int height);

void print_system_info();

int main()
{
    glfwSetErrorCallback(glfw_error_callback);

    int success = glfwInit();
    if (!success)
        std::exit(EXIT_FAILURE);

    std::shared_ptr<Window> window = std::make_shared<Window>("INF01047 - Trabalho Final");

    // Desabilita limite de quadros
    // glfwSwapInterval(0);

    // Definimos a função de callback que será chamada sempre que a janela for
    // redimensionada, por consequência alterando o tamanho do "framebuffer"
    // (região de memória onde são armazenados os pixels da imagem).
    window->set_framebuffer_size_callback(FramebufferSizeCallback);

    // Carregamento de todas funções definidas por OpenGL 3.3, utilizando a
    // biblioteca GLAD.
    gladLoadGL(glfwGetProcAddress);

    print_system_info();

    // Inicializamos o código para renderização de texto.
    TextRendering_Init();

    std::shared_ptr<GpuProgram> gpu_program = std::make_shared<GpuProgram>();

    GameStateManager state_manager(window, gpu_program);
    state_manager.push_state(std::make_unique<BaseState>());

    float dt;
    float current_time;
    float prev_time = (float)glfwGetTime();

    // Ficamos em um loop infinito, renderizando, até que o usuário feche a janela
    while (!glfwWindowShouldClose(window->glfw_window))
    {
        // Atualiza delta de tempo
        current_time = (float)glfwGetTime();
        dt = current_time - prev_time;
        prev_time = current_time;

        state_manager.update(dt);
        state_manager.draw();

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

    // Clean up
    while (!state_manager.empty()) {
        state_manager.pop_state();
    }

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
