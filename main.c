//loop principal do jogo

#include "raylib.h"
#include "include/player.h"
#include "include/enemy.h"
#include "include/projectile.h"
#include "include/gui.h"
#include "include/game.h"
#include <ctype.h>

int main(void)
{   
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "Directive: KILL");

    Texture2D enemySprite = LoadTexture("assets/textures/broken_ship.png");
    float enemiesStopY = 120.0f;
    Enemies_Init(enemiesStopY);


    Projectiles_Init(200);

    Vector2 playerPosition = { (float)screenWidth / 2, (float)screenHeight / 2 };
    float playerRadius = 20.0f;
    float playerSpeed = 0.14f;

    Player_Init(); 
    GUI_Init();
    GuiState guiState = GUI_STATE_MENU;

    bool shouldExit = false;
    while (!WindowShouldClose() && !shouldExit)    
    {
    float delta = GetFrameTime();
        GuiState prevGuiState = guiState;


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
                playerPosition = (Vector2){ (float)screenWidth / 2, (float)screenHeight / 2 };
                Player_Reset();
                Enemies_Init(enemiesStopY);
                Projectiles_Free();
                Projectiles_Init(200);
                Game_Init();
            }
            break;

        case GUI_STATE_GAME:
        {
            static float shootTimer = 0.0f;
            const float shootInterval = 0.5f;
            shootTimer += delta;
            if (shootTimer >= shootInterval) {
                shootTimer = 0.0f;
                Enemies_ShootAll();
            }

            Player_HandleMovement(&playerPosition, playerRadius, playerSpeed, screenWidth, screenHeight);
            Player_HandleShooting(delta, playerPosition);
            Player_UpdateShots(delta);

            Game_Update(delta);

            Player_DrawShots();
            Player_Draw(&playerPosition);
            
            Enemies_Update();
            Enemies_Draw(enemySprite);

            Projectiles_Update(delta);
            Projectiles_Draw();
        
            int hits = Projectiles_CheckPlayerCollision(playerPosition, playerRadius);
            if (hits > 0) {
                Player_TakeDamage(hits);
                Game_AddScore(-10 * hits);
            }

            int lives = Player_GetHealth();
            Gui_Draw(GUI_STATE_GAME, lives);

            int currentWave = Enemies_GetCurrentWave();
            DrawText(TextFormat("Wave: %d", currentWave), 10, 34, 18, RAYWHITE);

            int score = Game_GetScore();
            DrawText(TextFormat("Score: %d", score), 10, 58, 18, RAYWHITE);

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
            DrawText(namebuf, 260, 200, 30, YELLOW);

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

            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
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
Projectiles_Free();
CloseWindow();
return 0;
}