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

// Coordenadas de textura obtidas do arquivo OBJ (se existirem!)
in vec2 texcoords;
in vec3 texcoords_skybox;

in vec3 color_vert;

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

// Variáveis para acesso das imagens de textura
uniform samplerCube SkyImage;

uniform sampler2D FloorNormal;
uniform sampler2D FloorImage;
uniform sampler2D FloorAmbient;

uniform sampler2D TableNormal;
uniform sampler2D TableImage;
uniform sampler2D TableAmbient;
uniform sampler2D TableRoughness;

uniform sampler2D BoardNormal;
uniform sampler2D BoardImage;
uniform sampler2D BoardAmbient;
uniform sampler2D BoardRoughness;

uniform sampler2D WhitePiecesImage;
uniform sampler2D WhitePiecesAmbient;

uniform sampler2D BlackPiecesImage;
uniform sampler2D BlackPiecesAmbient;

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
vec3 apply_fog(vec3 color, float distance)
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
                               vec4 view_vec,
                               float q)
{
    vec4 half_vec = (view_vec + light_vec) / length(view_vec + light_vec);

    return specular_refl_color * specular_light_color * pow(max(0.0, dot(normal, half_vec)), q);
}

vec3 glossy_reflection(vec4 refl_vec)
{
    vec3 glossy_vec = refl_vec.xyz;

    glossy_vec.y = -abs(glossy_vec.y);

    return glossy_vec;
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

    vec4 light_pos = vec4(70.0,100.0,71.0,1.0);

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

    // Alpha default = 1 = 100% opaco = 0% transparente
    color.a = 1;

    // Expoente especular para o modelo de iluminação de Blinn-Phong
    float q = 1.0;

    // Vetor que define o sentido da câmera em relação ao ponto atual.
    vec4 view_vec = normalize(p - camera_position);

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

            refl_vec = reflect(view_vec, norm);
            specular_light_color = texture(SkyImage, glossy_reflection(refl_vec)).rgb;
            specular_refl_color = max(vec3(0.0), (1 - 4 * gain(texture(TableRoughness, texcoords).r, 0.5)));

            q = 15.0;
            break;

        case BOARD:
            norm = texture(BoardNormal, texcoords) * 2.0 - 0.5;
            norm = vec4(normalize(tbn * norm.xyz), 0.0);

            surface_color = texture(BoardImage, texcoords).rgb;
            ambient_refl_color = surface_color * texture(BoardAmbient, texcoords).rgb;

            refl_vec = reflect(view_vec, norm);
            specular_light_color = texture(SkyImage, refl_vec.xyz).rgb;
            specular_refl_color = 0.05 * (1 - texture(BoardRoughness, texcoords).rgb);

            q = 60.0;

            if (get_current_square() == selecting_square) {
                surface_color *= 0.1;
                surface_color += 0.1;
                surface_color.g += 0.8;
            }
            break;

        case PIECE:
            switch (piece_color) {
                case WHITE:
                    surface_color = texture(WhitePiecesImage, texcoords).rgb;
                    ambient_refl_color = surface_color * texture(WhitePiecesAmbient, texcoords).rgb;
                    specular_refl_color = vec3(0.0);

                    color.a = 0.8;
                    break;

                case BLACK:
                    norm = normalize(normal);

                    surface_color = texture(BlackPiecesImage, texcoords).rgb;
                    ambient_refl_color = surface_color * texture(BlackPiecesAmbient, texcoords).rgb;

                    refl_vec = reflect(view_vec, norm);
                    specular_light_color = texture(SkyImage, refl_vec.xyz).rgb;
                    specular_refl_color = vec3(0.1);
                    q = 60.0;
                    break;
            }
            break;
    }

    vec3 diffuse_term;

    // If vertex shader generated a color, use Gouraud Shading for diffuse illumination
    if (color_vert != vec3(0.0)) {
        diffuse_term = surface_color * color_vert;
    }
    else {
        diffuse_term = lambert_diffuse_term(diffuse_light_color, surface_color, norm, light_vec);
    }

    vec3 ambient_term_ = ambient_term(ambient_light_color, ambient_refl_color);

    vec3 specular_term = vec3(0.0);

    // If object has a reflection color, compute Blinn-Phong specular term
    if (specular_refl_color != vec3(0.0))
        specular_term = blinn_phong_specular_term(specular_light_color, specular_refl_color, norm, light_vec, -view_vec, q);

    color.rgb = diffuse_term + ambient_term_ + specular_term;

    color.rgb = apply_fog(color.rgb, length(camera_position - p));

    // Cor final com correção gamma, considerando monitor sRGB.
    // Veja https://en.wikipedia.org/w/index.php?title=Gamma_correction&oldid=751281772#Windows.2C_Mac.2C_sRGB_and_TV.2Fvideo_standard_gammas
    color.rgb = pow(color.rgb, vec3(1.0)/2.2);
}
