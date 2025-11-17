#include "gui.h"
#include "raylib.h"
#include "../include/game.h"
#include <stdio.h>
#include <math.h>

static int menuSelection = 0;


static Texture2D menuBackground;
static Texture2D menuChoice1;
static Texture2D menuChoice2;
static Texture2D menuChoice3;
static Texture2D health;
static Texture2D scoreMenu;
static Texture2D gameOverImg;
static Texture2D guiBackground;
static Texture2D guiOverlay;
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
    guiBackground = LoadTexture("assets/GUI/GUI_background.png");
    guiOverlay = LoadTexture("assets/GUI/GUI.png");
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

float GUI_GetScale(void) {
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();
    float scaleX = (float)sw / 800.0f;
    float scaleY = (float)sh / 450.0f;
    return fminf(scaleX, scaleY);
}

int GUI_GetScaledFontSize(int baseSize) {
    float scale = GUI_GetScale();
    int scaled = (int)((float)baseSize * scale);
    if (scaled < 10) scaled = 10;
    return scaled;
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

    if (( !IsKeyDown(KEY_LEFT_ALT) && !IsKeyDown(KEY_RIGHT_ALT) ) && (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE))) {
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
            // Desenha GUI background e overlay e elementos dentro das barras
            int sw = GetScreenWidth();
            int sh = GetScreenHeight();

            // background (fit)
            if (guiBackground.width > 0 && guiBackground.height > 0) {
                float bgScale = fminf((float)sw / (float)guiBackground.width, (float)sh / (float)guiBackground.height);
                float bgW = guiBackground.width * bgScale;
                float bgH = guiBackground.height * bgScale;
                float bgX = ((float)sw - bgW) / 2.0f;
                float bgY = ((float)sh - bgH) / 2.0f;
                Rectangle src = {0,0, (float)guiBackground.width, (float)guiBackground.height};
                Rectangle dst = {bgX, bgY, bgW, bgH};
                Vector2 orig = {0,0};
                DrawTexturePro(guiBackground, src, dst, orig, 0.0f, WHITE);
            }

            // overlay
            Rectangle overlay; GUI_GetOverlayDest(&overlay);
            Rectangle srcO = {0,0, (float)guiOverlay.width, (float)guiOverlay.height};
            Vector2 origO = {0,0};
            DrawTexturePro(guiOverlay, srcO, overlay, origO, 0.0f, WHITE);

            // calcula a área de jogo dentro das barras
            Rectangle playArea; GUI_GetPlayArea(&playArea);

            // (crystals drawn below using overlay-based bar width)

            // Desenha o score sob o rótulo SCORE dentro da barra direita (posição aproximada)
            int fontSize = GUI_GetScaledFontSize(18);
            // right bar area
            float rightBarX = playArea.x + playArea.width + 10.0f;
            // score Y position a little below overlay top + some offset
            float scoreY = overlay.y + 60.0f;
            int score = Game_GetScore();
            DrawText(TextFormat("%d", score), (int)rightBarX, (int)scoreY, fontSize, RAYWHITE);

            break;
        }

        case GUI_STATE_PAUSE: {
            int sw = GetScreenWidth();
            int sh = GetScreenHeight();
            int titleSize = GUI_GetScaledFontSize(40);
            int optionSize = GUI_GetScaledFontSize(20);
            const char *pauseText = "PAUSED";
            int pauseWidth = MeasureText(pauseText, titleSize);
            DrawText(pauseText, sw/2 - pauseWidth/2, sh/2 - 100, titleSize, RAYWHITE);
            Color resumeCol = (menuSelection == 0) ? YELLOW : RAYWHITE;
            const char *resumeText = "Resume";
            int resumeWidth = MeasureText(resumeText, optionSize);
            DrawText(resumeText, sw/2 - resumeWidth/2, sh/2 - 20, optionSize, resumeCol);
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
            // Limita a posição para que caiba na tela
            int maxStartY = (int)(sh * 0.3f);
            if (scoresStartY > maxStartY || dstH <= 0) scoresStartY = maxStartY;
            
            int scoreFontSize = GUI_GetScaledFontSize(20);
            int scoreLineSpacing = (int)(28 * GUI_GetScale());
            if (scoreLineSpacing < 20) scoreLineSpacing = 20;
            Game_DrawScoresAtScaled(scoresStartY, scoreFontSize, scoreLineSpacing);
            
            int fontSize = GUI_GetScaledFontSize(18);
            int margin = (int)(10 * GUI_GetScale());
            DrawText("(press Enter to return)", margin * 10, sh - margin * 4, fontSize, RAYWHITE);
            break;
        }
        case GUI_STATE_SAVE_SCORE: {
            int sw = GetScreenWidth();
            int sh = GetScreenHeight();
            int fontSize = GUI_GetScaledFontSize(20);
            int nameSize = GUI_GetScaledFontSize(30);
            const char *promptText = "Enter 4-letter name and press Enter:";
            int promptWidth = MeasureText(promptText, fontSize);
            int promptX = sw/2 - promptWidth/2;
            int promptY = sh/2 - 100;
            DrawText(promptText, promptX, promptY, fontSize, RAYWHITE);
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
    UnloadTexture(guiBackground);
    UnloadTexture(guiOverlay);
}

// Calcula o retângulo destino usado ao desenhar o overlay da GUI centralizado e ajustado à tela.
void GUI_GetOverlayDest(Rectangle *outDest) {
    if (!outDest) return;
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();
    if (guiOverlay.width > 0 && guiOverlay.height > 0) {
        float scale = fminf((float)sw / (float)guiOverlay.width, (float)sh / (float)guiOverlay.height);
        float w = guiOverlay.width * scale;
        float h = guiOverlay.height * scale;
        float x = ((float)sw - w) / 2.0f;
        float y = ((float)sh - h) / 2.0f;
        outDest->x = x; outDest->y = y; outDest->width = w; outDest->height = h;
        return;
    }
    outDest->x = 0; outDest->y = 0; outDest->width = sw; outDest->height = sh;
}

// Calcula o retângulo destino usado ao desenhar o background da GUI centralizado e ajustado à tela.
void GUI_GetBackgroundDest(Rectangle *outDest) {
    if (!outDest) return;
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();
    if (guiBackground.width > 0 && guiBackground.height > 0) {
        float scale = fminf((float)sw / (float)guiBackground.width, (float)sh / (float)guiBackground.height);
        float w = guiBackground.width * scale;
        float h = guiBackground.height * scale;
        float x = ((float)sw - w) / 2.0f;
        float y = ((float)sh - h) / 2.0f;
        outDest->x = x; outDest->y = y; outDest->width = w; outDest->height = h;
        return;
    }
    outDest->x = 0; outDest->y = 0; outDest->width = sw; outDest->height = sh;
}

// Calcula a área jogável dentro das barras do overlay da GUI. Tratamos uma fração da largura
// do overlay como pilares esquerdo/direito. A área interna exclui esses pilares.
void GUI_GetPlayArea(Rectangle *outArea) {
    Rectangle dest = {0,0, (float)GetScreenWidth(), (float)GetScreenHeight()};
    // Prefer GUI background area if available so spawn/movement are constrained to background
    if (guiBackground.width > 0 && guiBackground.height > 0) {
        GUI_GetBackgroundDest(&dest);
    } else {
        GUI_GetOverlayDest(&dest);
    }
    // fraction representing bar width relative to overlay width (tweakable)
    const float baseBarFrac = 0.12f;
    const float barFrac = baseBarFrac * 2.0f; // doubled to match debug/visual bar width
    // small inward shifts (scaled). Left moved further 4px, right moved further 8px.
    float baseShift = 4.0f * GUI_GetScale();
    // increase both pillars inward by additional 5 pixels (scaled)
    float extra = 5.0f * GUI_GetScale();
    float leftShift = baseShift * 2.0f + extra;   // ~8px + 5px scaled
    float rightShift = baseShift * 3.0f + extra;  // ~12px + 5px scaled
    float left = dest.x + dest.width * barFrac + leftShift;
    float right = dest.x + dest.width * (1.0f - barFrac) - rightShift;
    float top = dest.y;
    float bottom = dest.y + dest.height;
    if (outArea) {
        outArea->x = left;
        outArea->y = top;
        outArea->width = right - left;
        outArea->height = bottom - top;
    }
}

// Desenha apenas o plano de fundo (será desenhado antes do jogador e dos inimigos)
void GUI_DrawBackground(void) {
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();
    if (guiBackground.width > 0 && guiBackground.height > 0) {
        float bgScale = fminf((float)sw / (float)guiBackground.width, (float)sh / (float)guiBackground.height);
        float bgW = guiBackground.width * bgScale;
        float bgH = guiBackground.height * bgScale;
        float bgX = ((float)sw - bgW) / 2.0f;
        float bgY = ((float)sh - bgH) / 2.0f;
        Rectangle src = {0,0, (float)guiBackground.width, (float)guiBackground.height};
        Rectangle dst = {bgX, bgY, bgW, bgH};
        Vector2 orig = {0,0};
        DrawTexturePro(guiBackground, src, dst, orig, 0.0f, WHITE);
    }
}

// Desenha o overlay (barras, vidas, score) sobre os objetos do jogo
void GUI_DrawOverlay(int playerLives) {
    // overlay
    Rectangle overlay; GUI_GetOverlayDest(&overlay);
    Rectangle srcO = {0,0, (float)guiOverlay.width, (float)guiOverlay.height};
    Vector2 origO = {0,0};
    DrawTexturePro(guiOverlay, srcO, overlay, origO, 0.0f, WHITE);

    // calcula a área de jogo dentro das barras
    Rectangle playArea; GUI_GetPlayArea(&playArea);
    // Visual do bloqueador de movimento removido (colisão ainda aplicada via limites de playArea)
    const float baseBarFrac = 0.12f; // must match GUI_GetPlayArea base
    const float barFrac = baseBarFrac * 2.0f; // doubled to match debug/visual request
    // asymmetric inward shifts (scaled): left and right moved differently toward center
    float baseShift = 4.0f * GUI_GetScale();
    float extra = 5.0f * GUI_GetScale();
    float leftShift = baseShift * 2.0f + extra;   // ~8px + 5px scaled
    float rightShift = baseShift * 3.0f + extra;  // ~12px + 5px scaled
    // calcula larguras das barras esquerda/direita com base no overlay (consistente com GUI_GetPlayArea)
    float leftBarWidth = overlay.width * barFrac;
    float rightBarWidth = overlay.width * barFrac;

    // Desenha vidas como cristais dentro da barra esquerda (espalhados horizontalmente, tamanho aumentado)
    if (health.width > 0 && health.height > 0) {
        // calcula escala para que os ícones caibam na largura da barra (com algum padding)
        float maxIconW = (leftBarWidth - 16.0f) / (float)fmaxf(1, playerLives);
        float baseScale = maxIconW / (float)health.width;
        if (baseScale > 2.0f) baseScale = 2.0f; // cap scale to ~2x
        if (baseScale < 0.2f) baseScale = 0.2f;
        float iconW = health.width * baseScale;
        float iconH = health.height * baseScale;
        // Aumenta o tamanho visual dos cristais conforme solicitado
        iconW *= 1.5f;
        iconH *= 1.5f;

        // início X centralizado na barra esquerda (overlay.x + leftShift + leftBarWidth/2)
        float leftBarXCenter = overlay.x + leftShift + leftBarWidth / 2.0f;
        float totalWidth = playerLives * iconW + (playerLives - 1) * 6.0f;
        float startX = leftBarXCenter - totalWidth / 2.0f;
        float extraY = 24.0f * GUI_GetScale();
        float y = overlay.y + 16.0f + extraY; // moved down to avoid overlapping header

        for (int i = 0; i < playerLives; i++) {
            float ix = startX + i * (iconW + 6.0f);
            Rectangle dst = { ix, y, iconW, iconH };
            Vector2 o = { iconW/2.0f, iconH/2.0f };
            DrawTexturePro(health, (Rectangle){0,0,(float)health.width,(float)health.height}, dst, o, 0.0f, WHITE);
        }
    }

    // Desenha o score sob o rótulo SCORE dentro da barra direita (centralizado e um pouco mais abaixo)
    int fontSize = GUI_GetScaledFontSize(18);
    float rightBarCenterX = overlay.x + overlay.width - rightShift - rightBarWidth / 2.0f;
    float scoreY = overlay.y + overlay.height * 0.15f; // move down a bit
    int score = Game_GetScore();
    char scoreBuf[64];
    snprintf(scoreBuf, sizeof(scoreBuf), "%d", score);
    int textW = MeasureText(scoreBuf, fontSize);
    // antes o número era deslocado um pouco para a esquerda; agora reduzimos esse deslocamento para aproximar à direita
    float nudge = 4.0f * GUI_GetScale();
    DrawText(scoreBuf, (int)(rightBarCenterX - textW / 2.0f - nudge), (int)scoreY, fontSize, RAYWHITE);
}

// Desenha retângulos semi-transparente para depuração e alinhamento do background/overlay/área de jogo
void GUI_DrawDebugOverlay(void) {
    Rectangle overlay; GUI_GetOverlayDest(&overlay);
    const float barFrac = 0.12f;
    // dobra a largura lateral das barras para visualização de depuração (solicitado pelo usuário)
    const float adjFrac = barFrac * 2.0f;
    // aplica os mesmos deslocamentos assimétricos para dentro usados pela área de jogo, assim as barras de depuração batem com as colisões
    float baseShift = 4.0f * GUI_GetScale();
    float extra = 5.0f * GUI_GetScale();
    float leftShift = baseShift * 2.0f + extra;
    float rightShift = baseShift * 3.0f + extra;
    Rectangle leftBar = { overlay.x + leftShift, overlay.y, overlay.width * adjFrac, overlay.height };
    Rectangle rightBar = { overlay.x + overlay.width * (1.0f - adjFrac) - rightShift, overlay.y, overlay.width * adjFrac, overlay.height };
    Rectangle playArea; GUI_GetPlayArea(&playArea);

    // preenchimentos semi-transparente
    Color leftFill = (Color){ 255, 0, 0, 80 };
    Color rightFill = (Color){ 0, 0, 255, 80 };
    Color playFill = (Color){ 0, 255, 0, 60 };

    DrawRectangleRec(leftBar, leftFill);
    DrawRectangleRec(rightBar, rightFill);
    DrawRectangleRec(playArea, playFill);

    // outlines for clarity
    Color outline = (Color){ 255, 255, 255, 180 };
    DrawRectangleLinesEx(leftBar, 2.0f, outline);
    DrawRectangleLinesEx(rightBar, 2.0f, outline);
    DrawRectangleLinesEx(playArea, 2.0f, outline);
}