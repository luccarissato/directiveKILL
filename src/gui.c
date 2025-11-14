#include "gui.h"
#include "raylib.h"
#include "../include/game.h"

static int menuSelection = 0;


static const int titleX = 280;
static const int titleY = 100;
static const int optionX = 360;
static const int optionStartY = 210;
static const int optionSpacing = 60; 

void GUI_Init(void) {
    menuSelection = 0;
}


static int clampSelection(int sel, int count) {
    if (sel < 0) return count - 1;
    if (sel >= count) return 0;
    return sel;
}

GuiState Gui_Update(GuiState currentState) {
    if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_A)) {
        if (currentState == GUI_STATE_MENU) menuSelection = clampSelection(menuSelection - 1, 3);
        else if (currentState == GUI_STATE_PAUSE) menuSelection = clampSelection(menuSelection - 1, 1);
        else if (currentState == GUI_STATE_GAMEOVER) menuSelection = clampSelection(menuSelection - 1, 2);
        else if (currentState == GUI_STATE_SCORES) menuSelection = clampSelection(menuSelection - 1, 1);
    }
    if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_D)) {

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
            DrawText("DIRECTIVE: KILL", titleX, titleY, 30, RAYWHITE);

            Color startCol = (menuSelection == 0) ? YELLOW : RAYWHITE;
            Color scoresCol = (menuSelection == 1) ? YELLOW : RAYWHITE;
            Color quitCol  = (menuSelection == 2) ? YELLOW : RAYWHITE;

            DrawText("Start", optionX, optionStartY, 20, startCol);
            DrawText("Scores", optionX, optionStartY + optionSpacing, 20, scoresCol);
            DrawText("Quit",  optionX, optionStartY + optionSpacing*2, 20, quitCol);
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
            const char *txt = "GAME OVER";
            int fontSize = 40;
            int txtW = MeasureText(txt, fontSize);
            int x = GetScreenWidth() / 2 - txtW / 2;
            int y = GetScreenHeight() / 2 - 60;
            DrawText(txt, x, y, fontSize, RED);
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
            break;
        }
        case GUI_STATE_SCORES: {
            Game_DrawScores();
            DrawText("(press Enter to return)", 100, GetScreenHeight() - 40, 18, RAYWHITE);
            break;
        }
        case GUI_STATE_SAVE_SCORE: {
            DrawText("Enter 4-letter name and press Enter:", 160, 160, 20, RAYWHITE);
            break;
        }
    }
}

void Gui_Unload(void) { }