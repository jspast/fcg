#version 330 core

// Atributos de fragmentos recebidos como entrada ("in") pelo Fragment Shader.
// Neste exemplo, este atributo foi gerado pelo rasterizador como a
// interpolação da posição global e a normal de cada vértice, definidas em
// "shader_vertex.glsl" e "main.cpp".
in vec4 position_world;
in vec4 normal;

// Posição do vértice atual no sistema de coordenadas local do modelo.
in vec4 position_model;

in mat3 tbn;

in vec3 texcoords_skybox;

flat in int instanceID;

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

#define NONE 0
#define SELECTING 1
#define SELECTED 2
#define LAST_MOVE 3
#define AVAILABLE_MOVE 4
#define AVAILABLE_CAPTURE 5
#define CHECK 6
uniform int square_state;

// Identificadores que definem os estados das casas do tabuleiro
uniform vec2 selecting_square;
uniform vec2 selected_square;

uniform vec2 lastmove_start_square;
uniform vec2 lastmove_end_square;

// Coordenadas de textura obtidas do arquivo OBJ (se existirem!)
in vec2 texcoords;

// Variáveis para acesso das imagens de textura
uniform samplerCube SkyImage;

uniform sampler2D FloorNormal;
uniform sampler2D FloorImage;
uniform sampler2D FloorAmbient;
uniform sampler2D FloorRoughness;

uniform sampler2D TableNormal;
uniform sampler2D TableImage;
uniform sampler2D TableAmbient;
uniform sampler2D TableRoughness;

uniform sampler2D BoardNormal;
uniform sampler2D BoardImage;
uniform sampler2D BoardAmbient;
uniform sampler2D BoardRoughness;

uniform sampler2D WhitePiecesNormal;
uniform sampler2D WhitePiecesImage;
uniform sampler2D WhitePiecesAmbient;
uniform sampler2D WhitePiecesRoughness;

uniform sampler2D BlackPiecesNormal;
uniform sampler2D BlackPiecesImage;
uniform sampler2D BlackPiecesAmbient;
uniform sampler2D BlackPiecesRoughness;

// O valor de saída ("out") de um Fragment Shader é a cor final do fragmento.
out vec4 color;

#define SQUARE_SIZE 0.05789
#define BOARD_START (-4 * SQUARE_SIZE)

// FONTE: https://iquilezles.org/articles/functions/
float gain(float x, float k)
{
    float a = 0.5 * pow(2.0 * ((x<0.5) ? x : 1.0-x), k);
    return (x<0.5) ? a : 1.0-a;
}

// Simple fog
// Inspired by: https://iquilezles.org/articles/fog/
vec3 apply_fog(in vec3 color, in float distance)
{
    vec3  fog_color  = texture(SkyImage, vec3(0.5, -0.01, 0.5)).rgb;
    float fog_amount = 1.0 - exp(-distance * 0.015);
    fog_amount = gain(fog_amount, 1.5);
    return mix(color, fog_color, fog_amount);
}

vec3 lambert_diffuse_term(vec3 diffuse_light_color,
                          vec3 surface_color,
                          vec4 normal,
                          vec4 light_vec)
{
    return surface_color * diffuse_light_color * max(0.0, dot(normal, light_vec));
}

vec3 ambient_term(vec3 ambient_light_color,
                  vec3 ambient_refl_color)
{
    return ambient_light_color * ambient_refl_color;
}

vec3 blinn_phong_specular_term(vec3 specular_light_color,
                               vec3 specular_refl_color,
                               vec4 normal,
                               vec4 light_vec,
                               vec4 refl_vec,
                               float q)
{
    vec4 half_vec = (refl_vec + light_vec) / length(refl_vec + light_vec);

    return specular_refl_color * specular_light_color * pow(dot(normal, half_vec), q);
}

ivec2 get_current_square()
{
    ivec2 square;

    square.x = int(8 - (position_model.x - BOARD_START - SQUARE_SIZE) / SQUARE_SIZE) - 1;
    square.y = int((position_model.z - BOARD_START + SQUARE_SIZE) / SQUARE_SIZE) - 1;

    return square;
}

void main()
{
    // O fragmento atual é coberto por um ponto que percente à superfície de um
    // dos objetos virtuais da cena. Este ponto, p, possui uma posição no
    // sistema de coordenadas global (World coordinates). Esta posição é obtida
    // através da interpolação, feita pelo rasterizador, da posição de cada
    // vértice.
    vec4 p = position_world;

    vec4 norm = normal;

    // Obtemos a posição da câmera utilizando a inversa da matriz que define o
    // sistema de coordenadas da câmera.
    vec4 origin = vec4(0.0, 0.0, 0.0, 1.0);
    vec4 camera_position = inverse(view) * origin;

    vec4 light_pos = vec4(40.0,100.0,80.0,1.0);

    // Vetor que define o sentido da fonte de luz em relação ao ponto atual.
    vec4 light_vec = normalize(light_pos - p);

    // Espectro da fonte de luz
    vec3 diffuse_light_color = vec3(1.0,1.0,1.0);

    // Refletância difusa
    vec3 surface_color = vec3(0.0, 0.0, 0.0);

    vec3 ambient_light_color = vec3(0.1, 0.2, 0.3);

    // Refletância ambiente
    vec3 ambient_refl_color = vec3(0.2, 0.2, 0.2);

    // Refletância especular
    vec3 specular_refl_color = vec3(0.5, 0.5, 0.5);

    vec3 specular_light_color = vec3(1.0,1.0,1.0);

    vec4 refl_vec = vec4(0.0, 0.0, 0.0, 0.0);

    // Expoente especular para o modelo de iluminação de Blinn-Phong
    float q = 1.0;

    // Vetor que define o sentido da câmera em relação ao ponto atual.
    vec4 v = normalize(camera_position - p);

    switch (object_id) {
        case SKY:
            color.rgb = texture(SkyImage, texcoords_skybox).rgb;
            color.rgb = apply_fog(color.rgb, 10 * length((camera_position - p).xz));
            color.rgb = pow(color.rgb, vec3(1.0)/3.5);
            return;

        case FLOOR:
            norm = texture(FloorNormal, 50 * texcoords) * 2.0 - 0.5;
            norm = vec4(normalize(tbn * norm.xyz), 0.0);

            surface_color = texture(FloorImage, 50 * texcoords).rgb;
            ambient_refl_color = surface_color * texture(FloorAmbient, 50 * texcoords).rgb;
            specular_refl_color = vec3(0.0);
            break;

        case TABLE:
            norm = texture(TableNormal, texcoords) * 2.0 - 0.5;
            norm = vec4(normalize(tbn * norm.xyz), 0.0);

            surface_color = texture(TableImage, texcoords).rgb;
            ambient_refl_color = surface_color * texture(TableAmbient, texcoords).rgb;

            refl_vec = reflect(-v, norm);
            specular_light_color = texture(SkyImage, refl_vec.xyz).rgb;
            specular_refl_color = 0.2 * max(vec3(0.0), (1 - 12 * texture(TableRoughness, texcoords).rgb));

            q = 8.0;
            break;

        case BOARD:
            norm = texture(BoardNormal, texcoords) * 2.0 - 0.5;
            norm = vec4(normalize(tbn * norm.xyz), 0.0);

            surface_color = texture(BoardImage, texcoords).rgb;
            ambient_refl_color = surface_color * texture(BoardAmbient, texcoords).rgb;

            refl_vec = reflect(-v, norm);
            specular_light_color = texture(SkyImage, refl_vec.xyz).rgb;
            specular_refl_color = 0.2 * (1 - texture(BoardRoughness, texcoords).rgb);

            q = 6.0;

            if (get_current_square() == selecting_square) {
                surface_color *= 0.1;
                surface_color += 0.1;
                surface_color.g += 0.8;
            }
            break;

        case PIECE:
            switch (piece_color) {
                case WHITE:
                    norm = texture(WhitePiecesNormal, texcoords) * 2.0 - 0.5;
                    norm = vec4(normalize(tbn * norm.xyz), 0.0);

                    surface_color = texture(WhitePiecesImage, texcoords).rgb;
                    ambient_refl_color = surface_color * texture(WhitePiecesAmbient, texcoords).rgb;
                    specular_refl_color = vec3(0.0);
                    break;

                case BLACK:
                    norm = texture(BlackPiecesNormal, texcoords) * 2.0 - 0.5;
                    norm = vec4(normalize(tbn * norm.xyz), 0.0);

                    surface_color = texture(BlackPiecesImage, texcoords).rgb;
                    ambient_refl_color = surface_color * texture(BlackPiecesAmbient, texcoords).rgb;

                    refl_vec = reflect(-v, norm);
                    specular_light_color = texture(SkyImage, refl_vec.xyz).rgb;
                    specular_refl_color = 0.1 * (1 - texture(BlackPiecesRoughness, texcoords).rgb);
                    q = 6.0;
                    break;
            }
            break;
    }

    // Termo difuso utilizando a lei dos cossenos de Lambert
    vec3 diffuse_term = lambert_diffuse_term(diffuse_light_color, surface_color, norm, light_vec);

    // Termo ambiente
    vec3 ambient_term_ = ambient_term(ambient_light_color, ambient_refl_color);

    vec3 specular_term;
    if (specular_refl_color == vec3(0.0))
        specular_term = vec3(0.0);
    else
        // Termo especular utilizando o modelo de iluminação de Blinn-Phong
        specular_term = blinn_phong_specular_term(specular_light_color, specular_refl_color, norm, light_vec, refl_vec, q);

    // NOTE: Se você quiser fazer o rendering de objetos transparentes, é
    // necessário:
    // 1) Habilitar a operação de "blending" de OpenGL logo antes de realizar o
    //    desenho dos objetos transparentes, com os comandos abaixo no código C++:
    //      glEnable(GL_BLEND);
    //      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // 2) Realizar o desenho de todos objetos transparentes *após* ter desenhado
    //    todos os objetos opacos; e
    // 3) Realizar o desenho de objetos transparentes ordenados de acordo com
    //    suas distâncias para a câmera (desenhando primeiro objetos
    //    transparentes que estão mais longe da câmera).
    // Alpha default = 1 = 100% opaco = 0% transparente
    color.a = 1;

    // Cor final do fragmento calculada com uma combinação dos termos difuso,
    // especular, e ambiente. Veja slide 129 do documento Aula_17_e_18_Modelos_de_Iluminacao.pdf.
    //color.rgb = lambert_diffuse_term + ambient_term + phong_specular_term;

    color.rgb = diffuse_term + ambient_term_ + specular_term;

    color.rgb = apply_fog(color.rgb, length(camera_position - p));

    // Cor final com correção gamma, considerando monitor sRGB.
    // Veja https://en.wikipedia.org/w/index.php?title=Gamma_correction&oldid=751281772#Windows.2C_Mac.2C_sRGB_and_TV.2Fvideo_standard_gammas
    color.rgb = pow(color.rgb, vec3(1.0)/2.2);
}
