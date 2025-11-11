#include "gui.h"

// inclui header público da GUI
#include "gui.h"

#include "raylib.h"

static int menuSelection = 0; // índice da opção selecionada

// Posições do menu
static const int titleX = 280;
static const int titleY = 100;
static const int optionX = 360;
static const int optionStartY = 210;
static const int optionSpacing = 60; // espaçamento vertical entre opções

void GUI_Init(void) {
    menuSelection = 0;
}

// Ajusta seleção de forma circular
static int clampSelection(int sel, int count) {
    if (sel < 0) return count - 1;
    if (sel >= count) return 0;
    return sel;
}

GuiState Gui_Update(GuiState currentState) {
    // Navegação com W/S/A/D ou setas; confirmar com Enter/Space
    if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_A)) {
        // sobe
        if (currentState == GUI_STATE_MENU) menuSelection = clampSelection(menuSelection - 1, 2);
        else if (currentState == GUI_STATE_PAUSE) menuSelection = clampSelection(menuSelection - 1, 1);
    }
    if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_D)) {
        // desce
        if (currentState == GUI_STATE_MENU) menuSelection = clampSelection(menuSelection + 1, 2);
        else if (currentState == GUI_STATE_PAUSE) menuSelection = clampSelection(menuSelection + 1, 1);
    }

    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
        if (currentState == GUI_STATE_MENU) {
            if (menuSelection == 0) return GUI_STATE_GAME;      // Iniciar
            if (menuSelection == 1) return GUI_STATE_EXIT;     // Sair (fecha o jogo)
        }
        else if (currentState == GUI_STATE_PAUSE) {
            // única opção: voltar
            return GUI_STATE_GAME;
        }
        else if (currentState == GUI_STATE_GAMEOVER) {
            // em gameover, confirmar volta ao menu
            return GUI_STATE_MENU;
        }
    }

    return currentState;
}

void Gui_Draw(GuiState state, int playerLives) {
    switch (state) {
        case GUI_STATE_MENU: {
            DrawText("DIRECTIVE: KILL", titleX, titleY, 30, RAYWHITE);

            // duas opções: Start (índice 0) e Quit (índice 1)
            Color startCol = (menuSelection == 0) ? YELLOW : RAYWHITE;
            Color quitCol  = (menuSelection == 1) ? YELLOW : RAYWHITE;

            DrawText("Start", optionX, optionStartY, 20, startCol);
            DrawText("Quit",  optionX, optionStartY + optionSpacing, 20, quitCol);
            break;
        }

        case GUI_STATE_GAME: {
            DrawText(TextFormat("Lives: %d", playerLives), 10, 10, 20, RAYWHITE);
            break;
        }

        case GUI_STATE_PAUSE: {
            DrawText("PAUSED", 360, 200, 40, RAYWHITE);
            // opção única: Resume
            Color resumeCol = (menuSelection == 0) ? YELLOW : RAYWHITE;
            DrawText("Resume", optionX, optionStartY + 50, 20, resumeCol);
            break;
        }

        case GUI_STATE_GAMEOVER: {
            // centraliza "GAME OVER" e o botão Exit
            const char *txt = "GAME OVER";
            int fontSize = 40;
            int txtW = MeasureText(txt, fontSize);
            int x = GetScreenWidth() / 2 - txtW / 2;
            int y = GetScreenHeight() / 2 - 60; // ligeiramente acima do centro
            DrawText(txt, x, y, fontSize, RED);

            // Exit abaixo do texto, centralizado
            Color exitCol = (menuSelection == 0) ? YELLOW : RAYWHITE;
            const char *exitTxt = "Exit";
            int exitW = MeasureText(exitTxt, 20);
            int exitX = GetScreenWidth() / 2 - exitW / 2;
            int exitY = y + 80;
            DrawText(exitTxt, exitX, exitY, 20, exitCol);
            break;
        }
    }
}

void Gui_Unload(void) { }