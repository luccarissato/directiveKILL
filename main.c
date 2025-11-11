//loop principal do jogo

#include "raylib.h"
#include "include/player.h"
#include "include/enemy.h"
#include "include/projectile.h"
#include "include/gui.h"

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
            }
            break;

        case GUI_STATE_GAME:
        {
            static float shootTimer = 0.0f;
            const float shootInterval = 0.5f;
            shootTimer += delta;
            if (shootTimer >= shootInterval) {
                shootTimer = 0.0f;
                Vector2 enemyPos;
                int enemyIndex;
                if (Enemies_GetFirstActivePosition(&enemyPos, &enemyIndex)) {
                    Projectiles_Type(0, enemyPos);
                }
            }

            Player_HandleMovement(&playerPosition, playerRadius, playerSpeed, screenWidth, screenHeight);
            Player_HandleShooting(delta, playerPosition);
            Player_UpdateShots(delta);

            Player_DrawShots();
            Player_Draw(&playerPosition);
            
            Enemies_Update();
            Enemies_Draw(enemySprite);

            Projectiles_Update(delta);
            Projectiles_Draw();
        
            int hits = Projectiles_CheckPlayerCollision(playerPosition, playerRadius);
            if (hits > 0) {
                Player_TakeDamage(hits);
            }

            int lives = Player_GetHealth();
            Gui_Draw(GUI_STATE_GAME, lives);

            if (lives <= 0) {
                guiState = GUI_STATE_GAMEOVER;
            }
            break;
        }

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