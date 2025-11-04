#include "raylib.h"

#define PLAYER_MAX_SHOTS 50
#define BURST_SIZE 3           
#define BURST_INTERVAL 0.1f    
#define RELOAD_TIME 0.6f

float burstTimer = 0.0f;
float reloadTimer = 0.0f;
int burstCount = 0;
bool isReloading = false;

typedef struct Shoot 
{
    Vector2 speed;
    bool active;
    Color color;
    Vector2 position;
    int lifeSpawn;
    float radius;
} Shoot;

static Shoot shoot [PLAYER_MAX_SHOTS] = { 0 };

int main(void)
{
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "Directive: KILL");

    Vector2 playerPosition = { (float)screenWidth / 2, (float)screenHeight / 2 };
    float playerRadius = 20.0f;
    float playerSpeed = 0.14f;

    while (!WindowShouldClose())    
    {
        if (IsKeyDown(KEY_W)) playerPosition.y -= playerSpeed;
        if (IsKeyDown(KEY_A)) playerPosition.x -= playerSpeed;
        if (IsKeyDown(KEY_S)) playerPosition.y += playerSpeed;
        if (IsKeyDown(KEY_D)) playerPosition.x += playerSpeed; 

        float delta = GetFrameTime();

        if (isReloading) {
            reloadTimer += delta;
            if (reloadTimer >= RELOAD_TIME) {
                reloadTimer = 0.0f;
                burstCount = 0;
                isReloading = false;
            }
        }
        
        else if (IsKeyDown(KEY_K)) {
            burstTimer += delta;

            if (burstCount < BURST_SIZE && (burstTimer >= BURST_INTERVAL || burstCount == 0)) {
                for (int i = 0; i < PLAYER_MAX_SHOTS; i++) {
                    if (!shoot[i].active) {
                        shoot[i].position = (Vector2){ playerPosition.x, playerPosition.y };
                        shoot[i].speed = (Vector2){ 0.0f, -500.0f };
                        shoot[i].lifeSpawn = 2600;
                        shoot[i].radius = 4.0f;
                        shoot[i].color = RAYWHITE;
                        shoot[i].active = true;
                        break;
                    }
                }

                burstCount++;
                burstTimer = 0.0f;

                if (burstCount >= BURST_SIZE) {
                    isReloading = true;
                }
            }
        }

        else {
            burstCount = 0;
            burstTimer = 0.0f;
            reloadTimer = 0.0f;
            isReloading = false;
        }

        for (int i = 0; i < PLAYER_MAX_SHOTS; i++) {
            if (shoot[i].active) {
                shoot[i].position.y += shoot[i].speed.y * delta;
                shoot[i].lifeSpawn--;

                if (shoot[i].lifeSpawn <= 0 || shoot[i].position.y < 0) {
                    shoot[i].active = false;
                }

                if (shoot[i].active) {
                    DrawCircleV(shoot[i].position, shoot[i].radius, shoot[i].color);
                }
            }
     }

        if (playerPosition.x - playerRadius < -playerRadius / 2)
            playerPosition.x = -playerRadius / 2 + playerRadius;

        if (playerPosition.x + playerRadius > screenWidth + playerRadius / 2)
            playerPosition.x = screenWidth + playerRadius / 2 - playerRadius;

        if (playerPosition.y - playerRadius < 350)
            playerPosition.y = 350 + playerRadius;

        if (playerPosition.y + playerRadius > screenHeight + playerRadius / 2)
            playerPosition.y = screenHeight + playerRadius / 2 - playerRadius;

        BeginDrawing();

            ClearBackground(BLACK);
            DrawCircleV(playerPosition, playerRadius, GREEN);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}