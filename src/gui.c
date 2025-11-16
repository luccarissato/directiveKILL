#include "gui.h"
#include "raylib.h"
#include "../include/game.h"
#include <math.h>

static int menuSelection = 0;


static Texture2D menuBackground;
static Texture2D menuChoice1;
static Texture2D menuChoice2;
static Texture2D menuChoice3;
static Texture2D health;
static Texture2D scoreMenu;
static Texture2D gameOverImg;
static const int optionX = 360;
static const int optionStartY = 210;
static const int optionSpacing = 60; 

void GUI_Init(void) {
    // Carrega texturas do diretório correto (`assets/GUI`)
    menuBackground = LoadTexture("assets/GUI/menu_background.png");
    menuChoice1 = LoadTexture("assets/GUI/menus_letters1.png");
    menuChoice2 = LoadTexture("assets/GUI/menus_letters2.png");
    menuChoice3 = LoadTexture("assets/GUI/menus_letters3.png");
    health = LoadTexture("assets/GUI/Elements/Crystal_Heath.png");
    scoreMenu = LoadTexture("assets/GUI/score_Menu.png");
    gameOverImg = LoadTexture("assets/GUI/game_over.png");
    menuSelection = 0;
}

// Retorna a escala que deve ser aplicada para que a textura respeite a escala "da janela".
// Se houver um background carregado, usamos a mesma escala do background (para manter
// consistência com as imagens anteriores). Caso contrário, calculamos um "fit" da
// textura na janela preservando proporção.
static float GUI_GetWindowScale(const Texture2D tex) {
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();
    if (menuBackground.width > 0 && menuBackground.height > 0) {
        return fminf((float)sw / (float)menuBackground.width, (float)sh / (float)menuBackground.height);
    }
    if (tex.width > 0 && tex.height > 0) {
        return fminf((float)sw / (float)tex.width, (float)sh / (float)tex.height);
    }
    return 1.0f;
}


static int clampSelection(int sel, int count) {
    if (sel < 0) return count - 1;
    if (sel >= count) return 0;
    return sel;
}

GuiState Gui_Update(GuiState currentState) {
    if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) {
        if (currentState == GUI_STATE_MENU) menuSelection = clampSelection(menuSelection - 1, 3);
        else if (currentState == GUI_STATE_PAUSE) menuSelection = clampSelection(menuSelection - 1, 1);
        else if (currentState == GUI_STATE_GAMEOVER) menuSelection = clampSelection(menuSelection - 1, 2);
        else if (currentState == GUI_STATE_SCORES) menuSelection = clampSelection(menuSelection - 1, 1);
    }
    if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN)) {

        if (currentState == GUI_STATE_MENU) menuSelection = clampSelection(menuSelection + 1, 3);
        else if (currentState == GUI_STATE_PAUSE) menuSelection = clampSelection(menuSelection + 1, 1);
        else if (currentState == GUI_STATE_GAMEOVER) menuSelection = clampSelection(menuSelection + 1, 2);
        else if (currentState == GUI_STATE_SCORES) menuSelection = clampSelection(menuSelection + 1, 1);
    }

    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
        if (currentState == GUI_STATE_MENU) {
            if (menuSelection == 0) return GUI_STATE_GAME;      
            if (menuSelection == 1) return GUI_STATE_SCORES;   
            if (menuSelection == 2) return GUI_STATE_EXIT;     
        }
        else if (currentState == GUI_STATE_PAUSE) {
            return GUI_STATE_GAME;
        }
        else if (currentState == GUI_STATE_GAMEOVER) {
            if (menuSelection == 0) return GUI_STATE_MENU;
            if (menuSelection == 1) return GUI_STATE_SAVE_SCORE;
        }
        else if (currentState == GUI_STATE_SCORES) {
            return GUI_STATE_MENU;
        }
    }

    return currentState;
}

void Gui_Draw(GuiState state, int playerLives) {
    switch (state) {
        case GUI_STATE_MENU: {
            // Desenha o fundo mantendo a proporção (fit) e centralizado. Assim o background não é distorcido.
            int sw = GetScreenWidth();
            int sh = GetScreenHeight();
            int bw = menuBackground.width;
            int bh = menuBackground.height;
            float bgScale = 1.0f;
            float bgW = 0.0f, bgH = 0.0f;
            float bgX = 0.0f, bgY = 0.0f;
            if (bw > 0 && bh > 0) {
                // escala para caber na janela (fit)
                bgScale = fminf((float)sw / (float)bw, (float)sh / (float)bh);
                bgW = bw * bgScale;
                bgH = bh * bgScale;
                bgX = ((float)sw - bgW) / 2.0f;
                bgY = ((float)sh - bgH) / 2.0f;

                Rectangle src = { 0.0f, 0.0f, (float)bw, (float)bh };
                Rectangle dst = { bgX, bgY, bgW, bgH };
                Vector2 origin = { 0.0f, 0.0f };
                DrawTexturePro(menuBackground, src, dst, origin, 0.0f, WHITE);
            }

            // Seleciona a textura da opção atual com base em `menuSelection` e escala proporcionalmente ao background
            Texture2D currentChoice = (menuSelection == 0) ? menuChoice1 : (menuSelection == 1 ? menuChoice2 : menuChoice3);
            if (currentChoice.width > 0 && currentChoice.height > 0) {
                Rectangle srcC = { 0.0f, 0.0f, (float)currentChoice.width, (float)currentChoice.height };
                Vector2 originC = { 0.0f, 0.0f };
                // usa a escala baseada na janela (a mesma do background quando disponível)
                float choiceScale = GUI_GetWindowScale(currentChoice);
                if (bw > 0 && bh > 0) {
                    Rectangle dstC = { bgX, bgY, (float)currentChoice.width * choiceScale, (float)currentChoice.height * choiceScale };
                    DrawTexturePro(currentChoice, srcC, dstC, originC, 0.0f, WHITE);
                } else {
                    // fallback: desenha ajustado à janela (fit)
                    float dstW = (float)currentChoice.width * choiceScale;
                    float dstH = (float)currentChoice.height * choiceScale;
                    float drawX = ((float)GetScreenWidth() - dstW) / 2.0f;
                    float drawY = (float)GetScreenHeight() / 6.0f;
                    Rectangle dstC = { drawX, drawY, dstW, dstH };
                    DrawTexturePro(currentChoice, srcC, dstC, originC, 0.0f, WHITE);
                }
            }
            // Sem texto sobreposto; pressionar W/S altera a imagem mostrada
            break;
        }

        case GUI_STATE_GAME: {
            DrawText(TextFormat("Lives: %d", playerLives), 10, 10, 20, RAYWHITE);
            break;
        }

        case GUI_STATE_PAUSE: {
            DrawText("PAUSED", 360, 200, 40, RAYWHITE);
            Color resumeCol = (menuSelection == 0) ? YELLOW : RAYWHITE;
            DrawText("Resume", optionX, optionStartY + 50, 20, resumeCol);
            break;
        }

        case GUI_STATE_GAMEOVER: {
            // Desenha a imagem de Game Over no lugar do título textual
            int sw = GetScreenWidth();
            int sh = GetScreenHeight();
            int gw = gameOverImg.width;
            int gh = gameOverImg.height;
            int y = GetScreenHeight() / 2 - 60; // mantém a mesma referência vertical usada anteriormente
            if (gw > 0 && gh > 0) {
                // Desenha a imagem escalada de acordo com a janela (sem centralizar X)
                float imgScale = GUI_GetWindowScale(gameOverImg);
                float dstW = (float)gw * imgScale;
                float dstH = (float)gh * imgScale;
                float dstX = 20.0f; // margem esquerda fixa (não centraliza)
                float dstY = (float)y;
                Rectangle src = { 0.0f, 0.0f, (float)gw, (float)gh };
                Rectangle dst = { dstX, dstY, dstW, dstH };
                Vector2 origin = { 0.0f, 0.0f };
                DrawTexturePro(gameOverImg, src, dst, origin, 0.0f, WHITE);

                // Ajusta as posições dos textos para ficarem abaixo da imagem escalada,
                // centralizados em relação à largura da própria imagem.
                Color exitCol = (menuSelection == 0) ? YELLOW : RAYWHITE;
                Color saveCol = (menuSelection == 1) ? YELLOW : RAYWHITE;
                const char *exitTxt = "Exit";
                const char *saveTxt = "Save Score";
                int exitW = MeasureText(exitTxt, 20);
                int saveW = MeasureText(saveTxt, 20);
                int optOffsetY = 20;
                int exitY = (int)(dstY + dstH + optOffsetY);

                // Se as opções ficarem fora da tela por causa do tamanho da imagem,
                // ajusta para um fallback próximo ao final da janela.
                int shLocal = GetScreenHeight();
                if (exitY + 40 > shLocal) {
                    exitY = shLocal - 80; // posiciona acima do rodapé
                }

                int exitX = (int)(dstX + dstW / 2.0f - (float)exitW / 2.0f);
                int saveX = (int)(dstX + dstW / 2.0f - (float)saveW / 2.0f);

                // Garante que as X fiquem dentro da janela
                int swLocal = GetScreenWidth();
                if (exitX < 10) exitX = 10;
                if (saveX < 10) saveX = 10;
                if (exitX + exitW > swLocal - 10) exitX = swLocal - exitW - 10;
                if (saveX + saveW > swLocal - 10) saveX = swLocal - saveW - 10;

                DrawText(exitTxt, exitX, exitY, 20, exitCol);
                DrawText(saveTxt, saveX, exitY + 36, 20, saveCol);
            } else {
                // Fallback: se não houver imagem, desenha as opções centralizadas na tela
                Color exitCol = (menuSelection == 0) ? YELLOW : RAYWHITE;
                Color saveCol = (menuSelection == 1) ? YELLOW : RAYWHITE;
                const char *exitTxt = "Exit";
                const char *saveTxt = "Save Score";
                int exitW = MeasureText(exitTxt, 20);
                int exitX = GetScreenWidth() / 2 - exitW / 2;
                int exitY = y + 80;
                DrawText(exitTxt, exitX, exitY, 20, exitCol);

                int saveW = MeasureText(saveTxt, 20);
                int saveX = GetScreenWidth() / 2 - saveW / 2;
                int saveY = exitY + 36;
                DrawText(saveTxt, saveX, saveY, 20, saveCol);
            }
            break;
        }
        case GUI_STATE_SCORES: {
            // Desenha a imagem `scoreMenu` escalada proporcionalmente à tela (fit) e centra no topo.
            int sw = GetScreenWidth();
            int sh = GetScreenHeight();
            int smw = scoreMenu.width;
            int smh = scoreMenu.height;
            float sScale = 1.0f;
            float dstW = 0.0f, dstH = 0.0f, dstX = 0.0f, dstY = 0.0f;
            if (smw > 0 && smh > 0) {
                // Escala a imagem de acordo com a janela (mesma lógica usada para outras imagens)
                float sScale = GUI_GetWindowScale(scoreMenu);
                dstW = smw * sScale;
                dstH = smh * sScale;
                dstX = ((float)sw - dstW) / 2.0f;
                dstY = 30.0f; // margem superior fixa
                Rectangle srcS = { 0.0f, 0.0f, (float)smw, (float)smh };
                Rectangle dstS = { dstX, dstY, dstW, dstH };
                Vector2 origS = { 0.0f, 0.0f };
                DrawTexturePro(scoreMenu, srcS, dstS, origS, 0.0f, WHITE);
            }
            // Agora desenha os scores começando logo abaixo da imagem `scoreMenu` (ou em y padrão se não existir)
            int scoresStartY = (int)(dstY + dstH + 10.0f);
            // Se a imagem ficou muito grande e empurrou os scores para baixo,
            // limitamos a posição para o valor padrão (120) para que a lista
            // permaneça visível e com espaçamento semelhante ao anterior.
            if (scoresStartY > 120) scoresStartY = 120;
            if (dstH <= 0) scoresStartY = 120;
            Game_DrawScoresAt(scoresStartY);
            DrawText("(press Enter to return)", 100, GetScreenHeight() - 40, 18, RAYWHITE);
            break;
        }
        case GUI_STATE_SAVE_SCORE: {
            DrawText("Enter 4-letter name and press Enter:", 160, 160, 20, RAYWHITE);
            break;
        }
    }
}

void Gui_Unload(void) {
    // Descarrega as texturas carregadas em GUI_Init
    UnloadTexture(menuBackground);
    UnloadTexture(menuChoice1);
    UnloadTexture(menuChoice2);
    UnloadTexture(menuChoice3);
    UnloadTexture(health);
    UnloadTexture(scoreMenu);
    UnloadTexture(gameOverImg);
}