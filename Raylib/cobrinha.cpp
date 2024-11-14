// cobrinha.cpp
// baseado no jogo clássico snake

#include "raylib.h"

// ---------- Definindo estruturas ----------

typedef struct Cobra {
    Vector2 pos;    // Posição da cobrinha na tela
    Vector2 tam;    // Tamanho da cobrinha
    Vector2 vel;    // Velocidade da cobrinha
    Color cor;      // Cor da cobrinha
} Cobra;

typedef struct Fruta {
    Vector2 pos;    // Posição da fruta
    Vector2 tam;    // Tamanho da fruta
    bool ativo;     // Estado da fruta: ativa (não foi comida) ou não
    Color cor;      // Cor da fruta
} Fruta;

// ---------- Definindo tamanhos ----------

#define TAM_COBRA      256
#define TAM_QUADRADO    31
#define TAM_LARGURA    800
#define TAM_ALTURA     450

// ---------- Variáveis Globais ----------

int framesCounter = 0;
int contTamCobra = 0;
bool telaInicial = true;
bool primeiraVez = true;
bool gameOver = false;
bool pause = false;
bool permitirMovimento = false;

Fruta fruta = { 0 };
Cobra cobra[TAM_COBRA] = { 0 };
Vector2 cobraPos[TAM_COBRA] = { 0 }; // posição da cobra
Vector2 offset = { 0 }; // deslocamento

Color transWhite = Color{255, 255, 255, 160}; // Cor personalizada (branco, mas com opacidade)

//------------------------------------------------------------------------------------
// Controla a lógica do jogo a cada quadro
//------------------------------------------------------------------------------------
void InitGame(){
    framesCounter = 0;
    gameOver = false;
    pause = false;

    contTamCobra = 1;
    permitirMovimento = false;

    // calcula o deslocamento da tela, para centralizar a área jogavel, a fruta e a cobrinha
    offset.x = TAM_LARGURA%TAM_QUADRADO;
    offset.y = TAM_ALTURA%TAM_QUADRADO;

    // definindo a posição, tamanho e cor da cobrinha
    for (int i = 0; i < TAM_COBRA; i++){
        cobra[i].pos = (Vector2){ offset.x/2, offset.y/2 }; // posição inicial no canto da tela
        cobra[i].tam = (Vector2){ TAM_QUADRADO, TAM_QUADRADO }; // tamanho inicial é 1x1 (ou 31x31 pixels)
        cobra[i].vel = (Vector2){ TAM_QUADRADO, 0 }; // define a direção inicial da cobrinha (direita)

        if (i == 0) // Define a cabeça da cobra como VERDE
            cobra[i].cor = GREEN;
        else{ // o resto do corpo ira alternar entre VERDE ESCURO e VERDE LIMÃO
            if (i % 3 == 0)
                cobra[i].cor = DARKGREEN;
            else 
                cobra[i].cor = LIME;
        }   
    }

    for (int i = 0; i < TAM_COBRA; i++){
        cobraPos[i] = (Vector2){ 0.0f, 0.0f };
    }

    fruta.tam = (Vector2){ TAM_QUADRADO, TAM_QUADRADO }; // tamanho da fruta é 1x1 (ou 31x31 pixels)
    fruta.cor = RED; // Define a cor da fruta como VERMELHO
    fruta.ativo = false;
}

//------------------------------------------------------------------------------------
// Controla a lógica do jogo a cada quadro
//------------------------------------------------------------------------------------
void UpdateGame(){
    if (!gameOver){ // Se o player não perdeu o jogo...
        if (IsKeyPressed('P')) // Se a tecla 'P' for apertada
            pause = !pause; // muda o estado de pause 

        if (!pause){ // Se o jogo não estiver pausado
            // ----- Direção da cobrinha -----
            // Tratamento com entrada de dado feita pelo player

            // Se o player apertar a SETA_DIREITA
            if (IsKeyPressed(KEY_RIGHT) && (cobra[0].vel.x == 0) && permitirMovimento){
                cobra[0].vel = (Vector2){ TAM_QUADRADO, 0 };
                permitirMovimento = false; // não permite novo movimento até que seja atualizado na tela
            }
            // Se o player apertar a SETA_ESQUERDA
            if (IsKeyPressed(KEY_LEFT) && (cobra[0].vel.x == 0) && permitirMovimento){
                cobra[0].vel = (Vector2){ -TAM_QUADRADO, 0 };
                permitirMovimento = false; // não permite novo movimento até que seja atualizado na tela
            } 
            // Se o player apertar a SETA_CIMA
            if (IsKeyPressed(KEY_UP) && (cobra[0].vel.y == 0) && permitirMovimento){
                cobra[0].vel = (Vector2){ 0, -TAM_QUADRADO };
                permitirMovimento = false; // não permite novo movimento até que seja atualizado na tela
            }
            // Se o player apertar a SETA_BAIXO
            if (IsKeyPressed(KEY_DOWN) && (cobra[0].vel.y == 0) && permitirMovimento){
                cobra[0].vel = (Vector2){ 0, TAM_QUADRADO };
                permitirMovimento = false; // não permite novo movimento até que seja atualizado na tela
            }

            // ----- Movimento da cobrinha -----
            for (int i = 0; i < contTamCobra; i++)
                cobraPos[i] = cobra[i].pos;

            if ((framesCounter % 5) == 0){
                for (int i = 0; i < contTamCobra; i++){
                    if (i == 0){ // se for a cabeça, atualiza a a posição de acordo com a direção atual
                        cobra[0].pos.x += cobra[0].vel.x;
                        cobra[0].pos.y += cobra[0].vel.y;
                        permitirMovimento = true; // após a atualização do movimento da cabeça, permite nova direção
                    }
                    else // para o resto do corpo, a posição atual vai para a posição anterior
                        cobra[i].pos = cobraPos[i-1];
                }
            }

            // ----- Se colidir com a parede -----
            if (((cobra[0].pos.x) > (TAM_LARGURA - offset.x)) ||
                ((cobra[0].pos.y) > (TAM_ALTURA - offset.y)) ||
                (cobra[0].pos.x < 0) || (cobra[0].pos.y < 0))
            {
                gameOver = true;
            }

            // ----- Se colidir com a cobrinha -----
            for (int i = 1; i < contTamCobra; i++){
                if ((cobra[0].pos.x == cobra[i].pos.x) && (cobra[0].pos.y == cobra[i].pos.y))
                    gameOver = true;
            }

            // ----- Calcula a posição da fruta -----
            if (!fruta.ativo){ // Se fruta for comida
                fruta.ativo = true; // Informa que ela está ativa novamente

                // pega uma posição aleatória
                fruta.pos = (Vector2){ GetRandomValue(0, (TAM_LARGURA/TAM_QUADRADO) - 1)*TAM_QUADRADO + offset.x/2, GetRandomValue(0, (TAM_ALTURA/TAM_QUADRADO) - 1)*TAM_QUADRADO + offset.y/2 };

                // Confere se a posição escolhida não esteja ocupada pela cobra
                for (int i = 0; i < contTamCobra; i++){
                    while ((fruta.pos.x == cobra[i].pos.x) && (fruta.pos.y == cobra[i].pos.y)){
                        fruta.pos = (Vector2){ GetRandomValue(0, (TAM_LARGURA/TAM_QUADRADO) - 1)*TAM_QUADRADO + offset.x/2, GetRandomValue(0, (TAM_ALTURA/TAM_QUADRADO) - 1)*TAM_QUADRADO + offset.y/2 };
                        i = 0;
                    }
                }
            }

            // ----- Se a cobra colidir (comer) a fruta -----
            if ((cobra[0].pos.x < (fruta.pos.x + fruta.tam.x) && (cobra[0].pos.x + cobra[0].tam.x) > fruta.pos.x) &&
                (cobra[0].pos.y < (fruta.pos.y + fruta.tam.y) && (cobra[0].pos.y + cobra[0].tam.y) > fruta.pos.y))
            {
                cobra[contTamCobra].pos = cobraPos[contTamCobra - 1];
                contTamCobra += 1; // Aumenta tamanho da cobra
                fruta.ativo = false; // Registra que a fruta foi comida
            }

            framesCounter++;
        }
    }
    else{
        if (IsKeyPressed(KEY_ENTER)){ // Player inicia o jogo novamente
            InitGame();
            gameOver = false;
        }
    }
}

//------------------------------------------------------------------------------------
// Renderização que desenha o estado do jogo na tela
//------------------------------------------------------------------------------------
void DrawGame(){
    BeginDrawing();

        ClearBackground(BLACK);

        while(telaInicial){
            BeginDrawing();

                // Mensagem tela inicial
                DrawText("COBRINHA!", TAM_LARGURA / 2 - MeasureText("COBRINHA!", 80)/2, TAM_ALTURA / 2 - 80, 80, LIME);
                DrawText("Iniciar? <ENTER>", TAM_LARGURA / 2 - MeasureText("Iniciar? <ENTER>", 40)/2, TAM_ALTURA / 2, 40, RAYWHITE);

                if(IsKeyPressed(KEY_ENTER))
                    telaInicial = false;

            EndDrawing();
        }

        if (!gameOver){
            // Desenha o grid 
            for (int i = 0; i < TAM_LARGURA/TAM_QUADRADO + 1; i++){
                DrawLineV((Vector2){TAM_QUADRADO*i + offset.x/2, offset.y/2}, (Vector2){TAM_QUADRADO*i + offset.x/2, TAM_ALTURA - offset.y/2}, DARKGRAY);
            }

            for (int i = 0; i < TAM_ALTURA/TAM_QUADRADO + 1; i++){
                DrawLineV((Vector2){offset.x/2, TAM_QUADRADO*i + offset.y/2}, (Vector2){TAM_LARGURA - offset.x/2, TAM_QUADRADO*i + offset.y/2}, DARKGRAY);
            }

            // Desenha a cobrinha na tela
            for (int i = 0; i < contTamCobra; i++) DrawRectangleV(cobra[i].pos, cobra[i].tam, cobra[i].cor);

            // Desenha a fruta na tela
            DrawRectangleV(fruta.pos, fruta.tam, fruta.cor);

            // Escreve os atalhos do jogo no canto inferior esquerdo
            DrawText("Pause (P)", 10, TAM_ALTURA - 20, 20, transWhite);

            // Escreve a pontuação do jogador (quantas frutas comeu) no canto superior direito
            DrawText(TextFormat("Pontuação: %i", (int)contTamCobra-1), (TAM_LARGURA - 7) - MeasureText(TextFormat("Pontuação: %i", (int)contTamCobra-1), 20), TAM_ALTURA - 20, 20, transWhite);

            if (pause)
                DrawText("Jogo Pausado!", TAM_LARGURA/2 - MeasureText("Jogo Pausado!", 40)/2, TAM_ALTURA/2 - 40, 40, RAYWHITE);
        }
        else{
            DrawText(TextFormat("Você Perdeu! Pontuação: %i", (int)contTamCobra-1), GetScreenWidth()/2 - MeasureText(TextFormat("Você Perdeu! Pontuação: %.0i", (int)contTamCobra-1), 40)/2, GetScreenHeight()/2 - 50, 40, RAYWHITE);
            DrawText("Tentar Novamente? <ENTER>", TAM_LARGURA / 2 - MeasureText("Tentar Novamente? <ENTER>", 30)/2, TAM_ALTURA / 2, 30, RAYWHITE);
        }
        
    EndDrawing();
}

//------------------------------------------------------------------------------------
// Chama as funções UpdateGame e DrawGame para atualizar o jogo
//------------------------------------------------------------------------------------
void UpdateDrawFrame(){
    UpdateGame();
    DrawGame();
}

//------------------------------------------------------------------------------------
// Função principal
//------------------------------------------------------------------------------------
int main(){
    // Inicia a janela
    InitWindow(TAM_LARGURA, TAM_ALTURA, "Cobrinha! (snake)");

    InitGame();

    // Framerate
    SetTargetFPS(60);

    // Loop do jogo
    while (!WindowShouldClose()){ // Enquanto janela não for fechada...
        // Atualiza a tela
        UpdateDrawFrame();
    }

    CloseWindow(); // Fecha a janela e limpa recursos do raylib

    return 0;
}