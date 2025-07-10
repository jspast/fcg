# FCG - Trabalho Final

2025/1 - Turma A

Dupla: João Pastorello e Isadora Schwaab

## Contribuições

O contribuinte João Pastorello foi responsável pela montagem inicial da estrutura do projeto, baseada na refatoração do código disponibilizado pelo professor nas atividades de laboratório. Encontrou os modelos de objetos e texturas utilizados no jogo; implementou o sistema de estados que rege o fluxo de controle da aplicação, organizou a implementação das câmeras (livre e look-at) e aplicou os mapeamentos de textura, modelos de iluminação e de interpolação de todos os objetos. Também foi responsável pela aplicação dos testes de colisão e das features extras (Reflection Mapping, seleção de casas do tabuleiro com mouse e mapeamento de normais).

A contribuinte Isadora Schwaab foi responsável pela organização final da instanciação dos objetos (em nível de instâncias que diferem apenas em sua posição). Também foi responsável pela implementação da lógica de jogo (mapeamento entre o jogo 3D e um tabuleiro virtual, que é modificado conforme os inputs), bem como pela animação das peças na captura a partir de curvas de Bézier cúbicas (incluindo a aplicação de um dos testes de colisão na captura) e pela moviementação automática da câmera look-at após uma jogada (que também é implementada como uma animação baseada em tempo decorrido).

## Uso de IAs generativas

O **ChatGPT** foi utilizado principalmente para esclarescer conceitos de C++, como o uso de ponteiros inteligentes, estruturas de dados e processamento assíncrono.
Além disso, gerou um esboço do sistema de estados (classes `GameState` e `GameStateManager`) que foi adaptado e incrementado para a utilização no jogo.
No geral, a ferramenta foi útil, mas se mostrou incapaz de auxiliar com decisões complexas.
Conseguiu substituir muitas buscas rápidas na documentação e internet, mas não tarefas de raciocínio lógico avançado.

O **Deepseek** também foi utilizado como auxiliar para a implementação da lógica de jogo, esclarescendo a documentação da biblioteca Chess Library (Disservin) e fornecendo exemplos práticos para a aplicação de funções.

## Conceitos de Computação Gráfica aplicados no desenvolvimento

- [x] Malhas poligonais complexas
- [x] Transformações geométricas controladas pelo usuário
- [x] Câmera livre e câmera look-at
- [x] Instâncias de objetos
- [x] Três tipos de testes de intersecção
- [x] Modelos de Iluminação Difusa e Blinn-Phong
- [x] Modelos de Interpolação de Phong e Gouraud
- [x] Mapeamento de texturas em todos os objetos
- [x] Movimentação com curva Bézier cúbica
- [x] Animações baseadas no tempo ($\Delta t$)

Extras:
- [x] Mapeamento de normais
- [x] SkyBox com Reflection Mapping
- [x] Seleção de casas do tabuleiro com o mouse

![](screenshots/complex_meshes_0.jpg)

![](screenshots/complex_meshes_1.jpg)
*Malhas poligonais complexas*

![](screenshots/lambert_diffuse_illumination.jpg)
*Modelo de Iluminação Difusa de Lambert com Interpolação de Gouraud*

![](screenshots/blinn_phong_illumination.jpg)
*Modelo de Iluminação de Blinn-Phong com Interpolação de Phong*

![](screenshots/cursor_selection_0.png)

![](screenshots/cursor_selection_1.png)
*Seleção de casas do tabuleiro com o mouse*


## Manual de usuário

Executando a aplicação, o usuário deve ter acesso a um menu simples. Ele pode:
- alterar a qualidade das texturas carregadas, para que estejam correspondentes aos recursos do ambiente de execução;
- iniciar um novo jogo, clicando no botão 'JOGAR'.

Na tela de jogo, o controle se dá tanto através do mouse quanto do teclado. O usuário pode:
- mover o cursor sobre o tabuleiro para indicar uma casa (que fica colorida em verde);
- clicar em uma casa do tabuleiro (com o botão esquerdo do mouse) para seleciona-la (tornando a casa azul);
- com o cursor posicionado fora do tabuleiro, ele pode ainda controlar a seleção da casa com as teclas UP, DOWN, LEFT e RIGHT do teclado; como uma casa pré-selecionada (verde), o usuário pode usar a tecla ENTER para selecioná-la (tornando-a azul);
- usar a tecla ESCAPE para entrar no modo *observador*.

Estando no modo observador (o qual captura o cursor, não permitindo que o usuário faça um movimento de peça), o usuário pode:
- usar o *scroll* do mouse para controlar a aproximação da câmera look-at com o ponto de foco;
- usar a tecla O para ativar o modo de projeção ortográfica, e pressioná-la novamente para retornar à projeção perspectiva;
- usar a tecla F para ativar a câmera livre, e a tecla L para retornar à câmera look-at (com foco no centro do tabuleiro);
- estando com a câmera livre ativa, o usuário pode se movimentar com as teclas W, A, S e D, deterninando sua direção com o cursor do mouse;
- usar a tecla F3 para exibir informações de depuração na tela.

O jogo segue a lógica de xadrez comum, alternando entre turnos. Contudo, não permite jogadas especiais (promoção de peça, roque, *en-passant*...). A aplicação detecta automaticamente o fim de jogo e exibe na tela o resultado correspondente. Encerrado o jogo, é necessário reiniciar a aplicação para jogar novamente.


## Compilação

Este repositório faz uso de submódulos git para incluir bibliotecas externas.
Para clonar o repositório juntamente com os submódulos utilizados, use o seguinte comando:

```shell
git clone --recurse-submodules https://github.com/jspast/fcg.git
```

Caso o repositório já esteja clonado sem os submódulos, use o comando:

```shell
git submodule update --init --recursive
```

### Windows

#### VSCode (Visual Studio Code)
1) Instale o [VSCode](https://code.visualstudio.com/).

2) Instale o compilador GCC no Windows seguindo as [instruções do VSCode](https://code.visualstudio.com/docs/cpp/config-mingw#_installing-the-mingww64-toolchain).

3) Instale o [CMake](https://cmake.org/download/).

4) Instale as extensões `ms-vscode.cpptools` e `ms-vscode.cmake-tools` no VSCode.
Se você abrir o diretório deste projeto no VSCode, automaticamente será sugerida a instalação destas extensões.

5) Abra as configurações da extensão cmake-tools (Ctrl-Shift-P e busque por `CMake: Open CMake Tools Extension Settings`), e adicione o caminho de instalação do GCC na opção de configuração `additionalCompilerSearchDirs`.

6) Clique no botão de "Play" na barra inferior do VSCode para compilar e executar o projeto.
Na primeira compilação, a extensão do CMake para o VSCode irá perguntar qual compilador você quer utilizar.
Selecione da lista o compilador GCC que você instalou com o MSYS/MinGW.

Se necessário, mais [instruções de uso do CMake no VSCode](https://github.com/microsoft/vscode-cmake-tools/blob/main/docs/README.md) estão disponíveis.

Caso você tenha problemas de compilação no Windows com GCC, cuide para extrair o código em um caminho que não contenha espaços no nome de algum diretório.

Caso você tenha problemas de execução, tente atualizar o driver da sua placa de vídeo.

### Linux

#### Instalando bibliotecas necessárias

##### Ubuntu

```shell
apt-get install build-essential make libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxcb1-dev libxext-dev libxrender-dev libxfixes-dev libxau-dev libxdmcp-dev
```

##### Linux Mint

```shell
apt-get install build-essential make libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxcb1-dev libxext-dev libxrender-dev libxfixes-dev libxau-dev libxdmcp-dev libmesa-dev libxxf86vm-dev
```

##### Fedora Linux

```shell
dnf install make automake gcc gcc-c++ kernel-devel libXxf86vm-devel mesa-libGL-devel libxcb-devel libX11-devel libXrandr-devel libXinerama-devel libXcursor-devel libXext-devel libXrender-devel libXfixes-devel libXau-devel libXdmcp-devel
```

#### VSCode (Visual Studio Code)
1) Instale o [VSCode](https://code.visualstudio.com/).

2) Instale as extensões `ms-vscode.cpptools` e `ms-vscode.cmake-tools` no VSCode.
Se você abrir o diretório deste projeto no VSCode, automaticamente será sugerida a instalação destas extensões.

3) Clique no botão de "Play" na barra inferior do VSCode para compilar
e executar o projeto.
Na primeira compilação, a extensão do CMake para o VSCode irá perguntar qual compilador você quer utilizar.
Selecione da lista o compilador que você deseja utilizar.

Se necessário, mais [instruções de uso do CMake no VSCode](https://github.com/microsoft/vscode-cmake-tools/blob/main/docs/README.md) estão disponíveis.

### macOS (não testado)
Para compilar e executar esse projeto no macOS, primeiro você precisa instalar o HOMEBREW, um gerenciador de pacotes para facilitar a instação de bibliotecas.
O HOMEBREW pode ser instalado com o seguinte comando no terminal:

```shell
/usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
```

Após a instalação do HOMEBREW, a biblioteca GLFW deve ser instalada.
Isso pode ser feito pelo terminal com o comando:

```shell
brew install glfw
```

#### Makefile
Abra um terminal no diretório do projeto e execute o comando `make -f Makefile.macOS` para compilar.
Para executar o código compilado, execute o comando `make -f Makefile.macOS run`.
