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

flat in int instanceID;

// Matrizes computadas no código C++ e enviadas para a GPU
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// Identificador que define qual objeto está sendo desenhado no momento
#define BOARD 0
#define PIECE 1
#define TABLE 2
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
#define BOARD_START -4 * SQUARE_SIZE

ivec2 get_current_square()
{
    ivec2 square;

    square.x = int(8 - (position_model.x - BOARD_START - SQUARE_SIZE) / SQUARE_SIZE) - 1;
    square.y = int((position_model.z - BOARD_START + SQUARE_SIZE) / SQUARE_SIZE) - 1;

    return square;
}

void main()
{
    vec4 norm = normal;

    // Parâmetros que definem as propriedades espectrais da superfície
    vec3 Kd; // Refletância difusa
    vec3 Ks; // Refletância especular
    vec3 Ka; // Refletância ambiente
    float q; // Expoente especular para o modelo de iluminação de Phong

    // Coordenadas de textura U e V
    float U = texcoords.x;
    float V = texcoords.y;

    switch (object_id) {
        // Propriedades espectrais das peças
        case PIECE:
            switch (piece_color) {
                case WHITE:
                    norm = texture(WhitePiecesNormal, vec2(U,V)) * 2.0 - 0.5;
                    norm.xyz = normalize(tbn * norm.xyz);
                    norm.w = 0.0;

                    Kd = texture(WhitePiecesImage, vec2(U,V)).rgb;
                    Ks = max(vec3(0.0), 0.2 - 0.5 * texture(WhitePiecesRoughness, vec2(U,V)).rgb);
                    Ka = Kd * texture(WhitePiecesAmbient, vec2(U,V)).rgb;
                    q = 32.0;
                    break;
                    break;

                case BLACK:
                    norm = texture(BlackPiecesNormal, vec2(U,V)) * 2.0 - 0.5;
                    norm.xyz = normalize(tbn * norm.xyz);
                    norm.w = 0.0;

                    Kd = texture(BlackPiecesImage, vec2(U,V)).rgb;
                    Ks = max(vec3(0.0), 0.2 - 0.5 * texture(BlackPiecesRoughness, vec2(U,V)).rgb);
                    Ka = Kd * texture(BlackPiecesAmbient, vec2(U,V)).rgb;
                    q = 32.0;
                    break;
            }
            break;

        // Propriedades espectrais da mesa
        case TABLE:
            norm = texture(TableNormal, vec2(U,V)) * 2.0 - 0.5;
            norm.xyz = normalize(tbn * norm.xyz);
            norm.w = 0.0;

            Kd = texture(TableImage, vec2(U,V)).rgb;
            Ks = max(vec3(0.0), 0.2 - 1.5 * texture(TableRoughness, vec2(U,V)).rgb);
            Ka = Kd * texture(TableAmbient, vec2(U,V)).rgb;
            q = 16.0;
            break;

        // Propriedades espectrais do tabuleiro
        case BOARD:
            norm = texture(BoardNormal, vec2(U,V)) * 2.0 - 0.5;
            norm.xyz = normalize(tbn * norm.xyz);
            norm.w = 0.0;

            Kd = texture(BoardImage, vec2(U,V)).rgb;
            Ks = 0.1 - 0.1 * texture(BoardRoughness, vec2(U,V)).rgb;
            Ka = Kd * texture(BoardAmbient, vec2(U,V)).rgb;
            q = 64.0;

            if (get_current_square() == selecting_square) {
                Kd *= 0.1;
                Kd += 0.1;
                Kd.g += 0.8;
            }

            break;

        // Objeto desconhecido = preto
        default:
            Kd = vec3(0.0,0.0,0.0);
            Ks = vec3(0.0,0.0,0.0);
            Ka = vec3(0.0,0.0,0.0);
            q = 1.0;
            break;
    }

    // Obtemos a posição da câmera utilizando a inversa da matriz que define o
    // sistema de coordenadas da câmera.
    vec4 origin = vec4(0.0, 0.0, 0.0, 1.0);
    vec4 camera_position = inverse(view) * origin;

    // O fragmento atual é coberto por um ponto que percente à superfície de um
    // dos objetos virtuais da cena. Este ponto, p, possui uma posição no
    // sistema de coordenadas global (World coordinates). Esta posição é obtida
    // através da interpolação, feita pelo rasterizador, da posição de cada
    // vértice.
    vec4 p = position_world;

    // Normal do fragmento atual, interpolada pelo rasterizador a partir das
    // normais de cada vértice.
    vec4 n = normalize(norm);

    vec4 l_pos = vec4(4.0,4.0,4.0,1.0);
    vec4 l_vec = vec4(0.0,-1.0,0.0,0.0);
    float a = radians(30.0);

    // Vetor que define o sentido da fonte de luz em relação ao ponto atual.
    vec4 l = normalize(l_pos - p);

    // Vetor que define o sentido da câmera em relação ao ponto atual.
    vec4 v = normalize(camera_position - p);

    // Vetor que define o sentido da reflexão especular ideal.
    vec4 r = -l + 2.0 * n * dot(n, l); // Vetor de reflexão especular ideal

    // Espectro da fonte de iluminação
    vec3 I = vec3(1.0,1.0,1.0); // Espectro da fonte de luz

    // Espectro da luz ambiente
    vec3 Ia = vec3(0.2,0.2,0.2); // Espectro da luz ambiente

    // Termo difuso utilizando a lei dos cossenos de Lambert
    vec3 lambert_diffuse_term = Kd * I * max(0.0, dot(n, l)); // Termo difuso de Lambert

    // Termo ambiente
    vec3 ambient_term = Ka * Ia; // Termo ambiente

    // Termo especular utilizando o modelo de iluminação de Phong
    vec3 phong_specular_term  = Ks * I * pow(max(0.0, dot(r, v)), q); // Termo especular de Phong

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
    color.rgb = lambert_diffuse_term + ambient_term + phong_specular_term;

    // Cor final com correção gamma, considerando monitor sRGB.
    // Veja https://en.wikipedia.org/w/index.php?title=Gamma_correction&oldid=751281772#Windows.2C_Mac.2C_sRGB_and_TV.2Fvideo_standard_gammas
    color.rgb = pow(color.rgb, vec3(1.0,1.0,1.0)/2.2);
}
