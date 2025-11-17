//loop principal do jogo

#include "raylib.h"
#include "include/player.h"
#include "include/enemy.h"
#include "include/projectile.h"
#include "include/gui.h"
#include "include/game.h"
#include <ctype.h>
#include <math.h>

int main(void)
{   
    // Obtém o tamanho do monitor e usa 70% para a janela
    int monitorWidth = GetMonitorWidth(0);
    int monitorHeight = GetMonitorHeight(0);
    const int screenWidth = (int)(monitorWidth * 0.7f);
    const int screenHeight = (int)(monitorHeight * 0.7f);

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "Directive: KILL");
    SetWindowMinSize(800, 450);

    Texture2D enemySprite = LoadTexture("assets/textures/broken_ship.png");
    Texture2D scoutSprite = LoadTexture("assets/textures/scout.png");
    Texture2D spikeSprite = LoadTexture("assets/GUI/Elements/spike_proj.png");
    float enemiesStopYRatio = 0.27f; // 27% da altura da janela
    float enemiesStopY = screenHeight * enemiesStopYRatio;
    Enemies_Init(enemiesStopY);


    Projectiles_Init(200);

    Vector2 playerPosition = { (float)screenWidth / 2, (float)screenHeight / 2 };
    float playerRadius = 20.0f;
    float basePlayerSpeed = 200.0f; // Velocidade base em pixels por segundo

    Player_Init(); 
    GUI_Init();
    GuiState guiState = GUI_STATE_MENU;

    bool shouldExit = false;
    while (!WindowShouldClose() && !shouldExit)    
    {
    float delta = GetFrameTime();
        GuiState prevGuiState = guiState;
        
        // Atualiza dimensões da janela a cada frame
        int currentWidth = GetScreenWidth();
        int currentHeight = GetScreenHeight();
        
        // Toggle fullscreen com Alt+Enter (não dispara confirmações porque Enter é ignorado quando Alt está pressionado)
        if ((IsKeyDown(KEY_LEFT_ALT) || IsKeyDown(KEY_RIGHT_ALT)) && IsKeyPressed(KEY_ENTER)) {
            ToggleFullscreen();
        }

    BeginDrawing();
    ClearBackground(BLACK);

    switch (guiState)
    {
        case GUI_STATE_MENU:
            Gui_Draw(GUI_STATE_MENU, 0);
            guiState = Gui_Update(guiState);

            if (guiState == GUI_STATE_EXIT) {
                shouldExit = true;
                break; 
            }

            if (prevGuiState == GUI_STATE_MENU && guiState == GUI_STATE_GAME) {
                playerPosition = (Vector2){ (float)currentWidth / 2, (float)currentHeight / 2 };
                Player_Reset();
                enemiesStopY = currentHeight * enemiesStopYRatio;
                Enemies_Init(enemiesStopY);
                Projectiles_Free();
                Projectiles_Init(200);
                Game_Init();
            }
            break;

        case GUI_STATE_GAME:
        {
            // Calcula velocidade proporcional ao tamanho da janela (pixels por segundo)
            float scale = fminf((float)currentWidth / 800.0f, (float)currentHeight / 450.0f);
            float playerSpeed = basePlayerSpeed * scale * delta; // Multiplica por delta para pixels por frame
            
            // Atualiza stopY se a altura mudou
            float newStopY = currentHeight * enemiesStopYRatio;
            if (fabsf(newStopY - enemiesStopY) > 1.0f) {
                enemiesStopY = newStopY;
                Enemies_UpdateStopY(enemiesStopY);
            }
            
            static float shootTimer = 0.0f;
            const float shootInterval = 0.5f;
            shootTimer += delta;
            if (shootTimer >= shootInterval) {
                shootTimer = 0.0f;
                Enemies_ShootAll(playerPosition);
            }

            Player_HandleMovement(&playerPosition, playerRadius, playerSpeed, currentWidth, currentHeight);
            Player_HandleShooting(delta, playerPosition);
            Player_UpdateShots(delta);

            Game_Update(delta);

            Player_DrawShots();
            Player_Draw(&playerPosition);
            
            Enemies_Update();
            Enemies_Draw(enemySprite, scoutSprite);

            Projectiles_SetPlayerPosition(playerPosition);

            Projectiles_Update(delta);
            Projectiles_DrawWithSprite(spikeSprite);
        
            int hits = Projectiles_CheckPlayerCollision(playerPosition, playerRadius);
            if (hits > 0) {
                Player_TakeDamage(hits);
                Game_AddScore(-10 * hits);
            }

            int lives = Player_GetHealth();
            Gui_Draw(GUI_STATE_GAME, lives);

            int currentWave = Enemies_GetCurrentWave();
            int fontSize = GUI_GetScaledFontSize(18);
            int margin = (int)(10 * fminf((float)currentWidth / 800.0f, (float)currentHeight / 450.0f));
            if (margin < 5) margin = 5;
            DrawText(TextFormat("Wave: %d", currentWave), margin, margin + 24, fontSize, RAYWHITE);

            int score = Game_GetScore();
            DrawText(TextFormat("Score: %d", score), margin, margin + 48, fontSize, RAYWHITE);

            if (lives <= 0) {
                guiState = GUI_STATE_GAMEOVER;
            }
            break;
        }

        case GUI_STATE_SAVE_SCORE: {
            static char namebuf[5] = {0};
            static int nameLen = 0;
            if (prevGuiState != GUI_STATE_SAVE_SCORE) {
                nameLen = 0; namebuf[0] = '\0';
            }

            Gui_Draw(GUI_STATE_SAVE_SCORE, 0);
            
            int nameSize = GUI_GetScaledFontSize(30);
            int nameWidth = MeasureText(namebuf, nameSize);
            int nameX = currentWidth/2 - nameWidth/2;
            int nameY = currentHeight/2 - 20;
            DrawText(namebuf, nameX, nameY, nameSize, YELLOW);

            int c = GetCharPressed();
            while (c > 0) {
                if (isalpha(c) && nameLen < 4) {
                    namebuf[nameLen++] = (char) toupper(c);
                    namebuf[nameLen] = '\0';
                }
                c = GetCharPressed();
            }
            if (IsKeyPressed(KEY_BACKSPACE) && nameLen > 0) {
                nameLen--; namebuf[nameLen] = '\0';
            }

            if (( !IsKeyDown(KEY_LEFT_ALT) && !IsKeyDown(KEY_RIGHT_ALT) ) && (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE))) {
                for (int i = nameLen; i < 4; i++) namebuf[i] = '-';
                namebuf[4] = '\0';
                Game_SaveScore(namebuf, Game_GetScore());
                guiState = GUI_STATE_MENU;
            }
            break;
        }

        case GUI_STATE_SCORES:
            Gui_Draw(GUI_STATE_SCORES, 0);
            guiState = Gui_Update(guiState);
            break;

        case GUI_STATE_GAMEOVER:
            Gui_Draw(GUI_STATE_GAMEOVER, 0);
            guiState = Gui_Update(guiState);
            break;
    }

    EndDrawing();
}

Gui_Unload();
Player_Unload();
UnloadTexture(enemySprite);
UnloadTexture(scoutSprite);
UnloadTexture(spikeSprite);
Projectiles_Free();
CloseWindow();
return 0;
}