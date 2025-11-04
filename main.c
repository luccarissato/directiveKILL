//loop principal do jogo

#include "raylib.h"
#include "include/player.h"

int main(void)
{
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "Directive: KILL");

    Vector2 playerPosition = { (float)screenWidth / 2, (float)screenHeight / 2 };
    float playerRadius = 20.0f;
    float playerSpeed = 0.14f;

    Player_Init();

    while (!WindowShouldClose())    
    {
        float delta = GetFrameTime();

        Player_HandleMovement(&playerPosition, playerRadius, playerSpeed, screenWidth, screenHeight);
        Player_HandleShooting(delta, playerPosition);
        Player_UpdateShots(delta);

        BeginDrawing();

            ClearBackground(BLACK);
        
            Player_DrawShots();
            DrawCircleV(playerPosition, playerRadius, GREEN);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}