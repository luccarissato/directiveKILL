#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"


void Player_Init(void);

// Handle player movement and keep it inside bounds
void Player_HandleMovement(Vector2 *playerPosition, float playerRadius, float playerSpeed, int screenWidth, int screenHeight);

// Process shooting input and burst/reload logic (call before updating shots)
void Player_HandleShooting(float delta, Vector2 playerPosition);

// Update active shots positions/lifetimes (call every frame)
void Player_UpdateShots(float delta);

// Draw active shots (call between BeginDrawing/EndDrawing)
void Player_DrawShots(void);

#endif // PLAYER_H
