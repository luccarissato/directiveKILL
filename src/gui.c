//funções da interface grafica (GUI)
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
        else if (currentState == GUI_STATE_GAMEOVER) menuSelection = clampSelection(menuSelection - 1, 2);
        else if (currentState == GUI_STATE_SCORES) menuSelection = clampSelection(menuSelection - 1, 1);
    }
    if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN)) {

        if (currentState == GUI_STATE_MENU) menuSelection = clampSelection(menuSelection + 1, 3);
        else if (currentState == GUI_STATE_GAMEOVER) menuSelection = clampSelection(menuSelection + 1, 2);
        else if (currentState == GUI_STATE_SCORES) menuSelection = clampSelection(menuSelection + 1, 1);
    }

    if (( !IsKeyDown(KEY_LEFT_ALT) && !IsKeyDown(KEY_RIGHT_ALT) ) && (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE))) {
        if (currentState == GUI_STATE_MENU) {
            if (menuSelection == 0) return GUI_STATE_GAME;      
            if (menuSelection == 1) return GUI_STATE_SCORES;   
            if (menuSelection == 2) return GUI_STATE_EXIT;     
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
            int sw = GetScreenWidth();
            int sh = GetScreenHeight();
            int bw = menuBackground.width;
            int bh = menuBackground.height;
            float bgScale = 1.0f;
            float bgW = 0.0f, bgH = 0.0f;
            float bgX = 0.0f, bgY = 0.0f;
            if (bw > 0 && bh > 0) {
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

            Texture2D currentChoice = (menuSelection == 0) ? menuChoice1 : (menuSelection == 1 ? menuChoice2 : menuChoice3);
            if (currentChoice.width > 0 && currentChoice.height > 0) {
                Rectangle srcC = { 0.0f, 0.0f, (float)currentChoice.width, (float)currentChoice.height };
                Vector2 originC = { 0.0f, 0.0f };
                float choiceScale = GUI_GetWindowScale(currentChoice);
                if (bw > 0 && bh > 0) {
                    Rectangle dstC = { bgX, bgY, (float)currentChoice.width * choiceScale, (float)currentChoice.height * choiceScale };
                    DrawTexturePro(currentChoice, srcC, dstC, originC, 0.0f, WHITE);
                } else {
                    float dstW = (float)currentChoice.width * choiceScale;
                    float dstH = (float)currentChoice.height * choiceScale;
                    float drawX = ((float)GetScreenWidth() - dstW) / 2.0f;
                    float drawY = (float)GetScreenHeight() / 6.0f;
                    Rectangle dstC = { drawX, drawY, dstW, dstH };
                    DrawTexturePro(currentChoice, srcC, dstC, originC, 0.0f, WHITE);
                }
            }
            break;
        }

        case GUI_STATE_GAME: {
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

            Rectangle overlay; GUI_GetOverlayDest(&overlay);
            Rectangle srcO = {0,0, (float)guiOverlay.width, (float)guiOverlay.height};
            Vector2 origO = {0,0};
            DrawTexturePro(guiOverlay, srcO, overlay, origO, 0.0f, WHITE);

            Rectangle playArea; GUI_GetPlayArea(&playArea);


            int fontSize = GUI_GetScaledFontSize(18);
            float rightBarX = playArea.x + playArea.width + 10.0f;
            float scoreY = overlay.y + 60.0f;
            int score = Game_GetScore();
            DrawText(TextFormat("%d", score), (int)rightBarX, (int)scoreY, fontSize, RAYWHITE);

            break;
        }

        case GUI_STATE_GAMEOVER: {
            int sw = GetScreenWidth();
            int sh = GetScreenHeight();
            int gw = gameOverImg.width;
            int gh = gameOverImg.height;
            int y = GetScreenHeight() / 2 - 60; 
            if (gw > 0 && gh > 0) {
                float imgScale = GUI_GetWindowScale(gameOverImg);
                float dstW = (float)gw * imgScale;
                float dstH = (float)gh * imgScale;
                float dstX = 20.0f; 
                float dstY = (float)y;
                Rectangle src = { 0.0f, 0.0f, (float)gw, (float)gh };
                Rectangle dst = { dstX, dstY, dstW, dstH };
                Vector2 origin = { 0.0f, 0.0f };
                DrawTexturePro(gameOverImg, src, dst, origin, 0.0f, WHITE);

                Color exitCol = (menuSelection == 0) ? YELLOW : RAYWHITE;
                Color saveCol = (menuSelection == 1) ? YELLOW : RAYWHITE;
                const char *exitTxt = "Exit";
                const char *saveTxt = "Save Score";
                int exitW = MeasureText(exitTxt, 20);
                int saveW = MeasureText(saveTxt, 20);
                int optOffsetY = 20;
                int exitY = (int)(dstY + dstH + optOffsetY);

                int shLocal = GetScreenHeight();
                if (exitY + 40 > shLocal) {
                    exitY = shLocal - 80; 
                }

                int exitX = (int)(dstX + dstW / 2.0f - (float)exitW / 2.0f);
                int saveX = (int)(dstX + dstW / 2.0f - (float)saveW / 2.0f);

                int swLocal = GetScreenWidth();
                if (exitX < 10) exitX = 10;
                if (saveX < 10) saveX = 10;
                if (exitX + exitW > swLocal - 10) exitX = swLocal - exitW - 10;
                if (saveX + saveW > swLocal - 10) saveX = swLocal - saveW - 10;

                DrawText(exitTxt, exitX, exitY, 20, exitCol);
                DrawText(saveTxt, saveX, exitY + 36, 20, saveCol);
            } else {
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
            int sw = GetScreenWidth();
            int sh = GetScreenHeight();
            int smw = scoreMenu.width;
            int smh = scoreMenu.height;
            float sScale = 1.0f;
            float dstW = 0.0f, dstH = 0.0f, dstX = 0.0f, dstY = 0.0f;
            if (smw > 0 && smh > 0) {
                float sScale = GUI_GetWindowScale(scoreMenu);
                dstW = smw * sScale;
                dstH = smh * sScale;
                dstX = ((float)sw - dstW) / 2.0f;
                dstY = 30.0f;
                Rectangle srcS = { 0.0f, 0.0f, (float)smw, (float)smh };
                Rectangle dstS = { dstX, dstY, dstW, dstH };
                Vector2 origS = { 0.0f, 0.0f };
                DrawTexturePro(scoreMenu, srcS, dstS, origS, 0.0f, WHITE);
            }
            int scoresStartY = (int)(dstY + dstH + 10.0f);
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
    if (guiBackground.width > 0 && guiBackground.height > 0) {
        GUI_GetBackgroundDest(&dest);
    } else {
        GUI_GetOverlayDest(&dest);
    }
    const float baseBarFrac = 0.12f;
    const float barFrac = baseBarFrac * 2.0f;
    float baseShift = 4.0f * GUI_GetScale();
    float extra = 5.0f * GUI_GetScale();
    float leftShift = baseShift * 2.0f + extra; 
    float rightShift = baseShift * 3.0f + extra; 
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
    const float baseBarFrac = 0.12f; 
    const float barFrac = baseBarFrac * 2.0f;
    float baseShift = 4.0f * GUI_GetScale();
    float extra = 5.0f * GUI_GetScale();
    float leftShift = baseShift * 2.0f + extra;
    float rightShift = baseShift * 3.0f + extra; 
    float leftBarWidth = overlay.width * barFrac;
    float rightBarWidth = overlay.width * barFrac;

    // Desenha vidas como cristais dentro da barra esquerda (espalhados horizontalmente, tamanho aumentado)
    if (health.width > 0 && health.height > 0) {
        float maxIconW = (leftBarWidth - 16.0f) / (float)fmaxf(1, playerLives);
        float baseScale = maxIconW / (float)health.width;
        if (baseScale > 2.0f) baseScale = 2.0f; 
        if (baseScale < 0.2f) baseScale = 0.2f;
        float iconW = health.width * baseScale;
        float iconH = health.height * baseScale;
        iconW *= 1.5f;
        iconH *= 1.5f;

        // início X centralizado na barra esquerda (overlay.x + leftShift + leftBarWidth/2)
        float leftBarXCenter = overlay.x + leftShift + leftBarWidth / 2.0f;
        float totalWidth = playerLives * iconW + (playerLives - 1) * 6.0f;
        float startX = leftBarXCenter - totalWidth / 2.0f;
        float extraY = 24.0f * GUI_GetScale();
        float y = overlay.y + 16.0f + extraY;

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
    float nudge = 4.0f * GUI_GetScale();
    DrawText(scoreBuf, (int)(rightBarCenterX - textW / 2.0f - nudge), (int)scoreY, fontSize, RAYWHITE);
}