#include "gui.h"

// inclui header público da GUI
#include "gui.h"

#include "raylib.h"

//Imagens do Menu
static Texture2D menuBackground;
static Texture2D menuChoice1;
static Texture2D menuChoice2;
static Texture2D menuChoice3;
static Texture2D health;
static int menuSelection = 0; // índice da opção selecionada

// Posições do menu
static const int optionX = 360;
static const int optionStartY = 210;
static const int optionSpacing = 60; // espaçamento vertical entre opções

void GUI_Init(void) {
    menuBackground= LoadTexture("assets\\textures\\menu_background.png");
    menuChoice1= LoadTexture("assets\\textures\\menus_letters1.png");
    menuChoice2= LoadTexture("assets\\textures\\menus_letters2.png");
    menuChoice3= LoadTexture("assets\\textures\\menus_letters3.png");
    health= LoadTexture("assets\textures\\Crystal_heath.png");
    menuSelection = 0;
}

// Ajusta seleção de forma circular
static int clampSelection(int sel, int count) {
    if (sel < 0) return count - 1;
    if (sel >= count) return 0;
    return sel;
}

GuiState Gui_Update(GuiState currentState) {
    // Navegação com W/S ou setas; confirmar com Enter/Espaço
    if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) {
        // sobe
        if (currentState == GUI_STATE_MENU) menuSelection = clampSelection(menuSelection - 1, 3);
        else if (currentState == GUI_STATE_PAUSE) menuSelection = clampSelection(menuSelection - 1, 1);
    }
    if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN)) {
        // desce
        if (currentState == GUI_STATE_MENU) menuSelection = clampSelection(menuSelection + 1, 3);
        else if (currentState == GUI_STATE_PAUSE) menuSelection = clampSelection(menuSelection + 1, 1);
    }

    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
        if (currentState == GUI_STATE_MENU) {
            if (menuSelection == 0) return GUI_STATE_GAME; // Iniciar
            if (menuSelection == 1) return GUI_STATE_GAME; // Temporario
            if (menuSelection == 2) return GUI_STATE_EXIT; // Sair (fecha o jogo)
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
            int screenWidth=GetScreenWidth();
            int screenHeight=GetScreenHeight();

            DrawTexturePro(menuBackground, 
                           (Rectangle){ 0.0f, 0.0f, (float)menuBackground.width, (float)menuBackground.height }, // Fonte: Toda a textura
                           (Rectangle){ 0.0f, 0.0f, (float)screenWidth, (float)screenHeight },                   // Destino: Toda a tela
                           (Vector2){ 0.0f, 0.0f }, 0.0f, RAYWHITE);

            // tres opções: Start (índice 0), Options (índice 1) e Quit (índice 2)
            if (menuSelection == 0) {
                DrawTexturePro(menuChoice1,
                               (Rectangle){ 0.0f, 0.0f, (float)menuChoice1.width, (float)menuChoice1.height },
                               (Rectangle){ 0.0f, 0.0f, (float)screenWidth, (float)screenHeight },
                               (Vector2){ 0.0f, 0.0f }, 0.0f, RAYWHITE);
            }
            else if (menuSelection == 1){
                DrawTexturePro(menuChoice2,
                               (Rectangle){ 0.0f, 0.0f, (float)menuChoice2.width, (float)menuChoice2.height },
                               (Rectangle){ 0.0f, 0.0f, (float)screenWidth, (float)screenHeight },
                               (Vector2){ 0.0f, 0.0f }, 0.0f, RAYWHITE);
            }
            else if(menuSelection == 2){
                DrawTexturePro(menuChoice3,
                               (Rectangle){ 0.0f, 0.0f, (float)menuChoice3.width, (float)menuChoice3.height },
                               (Rectangle){ 0.0f, 0.0f, (float)screenWidth, (float)screenHeight },
                               (Vector2){ 0.0f, 0.0f }, 0.0f, RAYWHITE);
            }
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

void Gui_Unload(void) {
    UnloadTexture(menuBackground);
    UnloadTexture(menuChoice1);
    UnloadTexture(menuChoice2);
    UnloadTexture(menuChoice3);
    UnloadTexture(health);
 }