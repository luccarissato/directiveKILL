//loop principal do jogo

#include "raylib.h"
#include "include/player.h"
#include "include/enemy.h"
#include "include/projectile.h"

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

    while (!WindowShouldClose())    
    {
        float delta = GetFrameTime();

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

        BeginDrawing();

            ClearBackground(BLACK);
            Player_DrawShots();
            Player_Draw(&playerPosition);
            
            Enemies_Update();
            Enemies_Draw(enemySprite);

            Projectiles_Update(delta);
            Projectiles_Draw();

        EndDrawing();
    }

    Player_Unload();
    UnloadTexture(enemySprite);

    Projectiles_Free();

    CloseWindow();
    return 0;
}