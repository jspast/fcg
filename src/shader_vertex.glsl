#version 330 core

// Atributos de vértice recebidos como entrada ("in") pelo Vertex Shader.
layout (location = 0) in vec4 model_coefficients;
layout (location = 1) in vec4 normal_coefficients;
layout (location = 2) in vec2 texture_coefficients;
layout (location = 3) in vec4 tangent_coefficients;

// Matrizes computadas no código C++ e enviadas para a GPU
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// Identificador que define qual objeto está sendo desenhado no momento
#define BOARD 0
#define PIECE 1
#define TABLE 2
#define SKY 3
#define FLOOR 4
uniform int object_id;

// Identificador que define qual a cor do objeto
#define WHITE 0
#define BLACK 1
uniform int piece_color;

// Atributos de vértice que serão gerados como saída ("out") pelo Vertex Shader.
// ** Estes serão interpolados pelo rasterizador! ** gerando, assim, valores
// para cada fragmento, os quais serão recebidos como entrada pelo Fragment
// Shader. Veja o arquivo "shader_fragment.glsl".
out vec4 position_world;
out vec4 position_model;
out vec4 normal;
out mat3 tbn;
out vec2 texcoords;
out vec3 texcoords_skybox;
out vec3 color_vert;

// Variável utilizada para encaminharmos a informação de gl_InstanceID para o
// fragment shader, caso necessário.
flat out int instanceID;

#define SQUARE_SIZE (0.05789 * 1.5f)
#define BOARD_START (-4 * SQUARE_SIZE)

vec3 lambert_diffuse_gouraud(vec3 diffuse_light_color,
                             vec4 normal,
                             vec4 light_vec)
{
    return diffuse_light_color * max(0.0, dot(normal, light_vec));
}

void main()
{
    // Computamos o deslocamento em X da instância atual de acordo com seu ID
    float dx = gl_InstanceID * SQUARE_SIZE;
    mat4 translation_matrix = mat4(
        1.0, 0.0, 0.0, 0.0, // 1a coluna
        0.0, 1.0, 0.0, 0.0, // 2a coluna
        0.0, 0.0, 1.0, 0.0, // 3a coluna
        dx , 0.0, 0.0, 1.0  // 4a coluna
    );

    mat4 updated_model_matrix = translation_matrix * model;

    // Encaminhamos a informação de gl_InstanceID para o fragment shader, caso necessário.
    instanceID = gl_InstanceID;

    // A variável gl_Position define a posição final de cada vértice
    // OBRIGATORIAMENTE em "normalized device coordinates" (NDC), onde cada
    // coeficiente estará entre -1 e 1 após divisão por w.
    // Veja {+NDC2+}.
    //
    // O código em "main.cpp" define os vértices dos modelos em coordenadas
    // locais de cada modelo (array model_coefficients). Abaixo, utilizamos
    // operações de modelagem, definição da câmera, e projeção, para computar
    // as coordenadas finais em NDC (variável gl_Position). Após a execução
    // deste Vertex Shader, a placa de vídeo (GPU) fará a divisão por W. Veja
    // slides 41-67 e 69-86 do documento Aula_09_Projecoes.pdf.

    gl_Position = projection * view * updated_model_matrix * model_coefficients;

    // Agora definimos outros atributos dos vértices que serão interpolados pelo
    // rasterizador para gerar atributos únicos para cada fragmento gerado.

    // Posição do vértice atual no sistema de coordenadas global (World).
    position_world = updated_model_matrix * model_coefficients;

    // Posição do vértice atual no sistema de coordenadas local do modelo.
    position_model = model_coefficients;

    // Normal do vértice atual no sistema de coordenadas global (World).
    // Veja slides 123-151 do documento Aula_07_Transformacoes_Geometricas_3D.pdf.
    normal = inverse(transpose(model)) * normal_coefficients;
    normal.w = 0.0;

    // Coordenadas de textura obtidas do arquivo OBJ (se existirem!)
    texcoords = texture_coefficients;

    // Matriz TBN
    vec3 t = normalize(vec3(updated_model_matrix * tangent_coefficients));
    vec3 n = normalize(vec3(updated_model_matrix * normal));

    t = normalize(t - dot(t, n) * n);

    vec3 b = cross(n, t);

    tbn = mat3(t, b, n);

    vec4 origin = vec4(0.0, 0.0, 0.0, 1.0);
    vec4 camera_position = inverse(view) * origin;
    texcoords_skybox = (position_world - camera_position).xyz;

    vec3 diffuse_light_color = vec3(1.0,1.0,1.0);

    vec4 light_pos = vec4(70.0,100.0,71.0,1.0);

    // Vetor que define o sentido da fonte de luz em relação ao ponto atual.
    vec4 light_vec = normalize(light_pos - position_world);

    color_vert = vec3(0.0);

    switch (object_id) {
        case PIECE:
            switch (piece_color) {
                case WHITE:
                    color_vert = lambert_diffuse_gouraud(diffuse_light_color, normal, light_vec);
            }
    }
}
