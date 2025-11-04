//  funções do jogador

#include "../include/player.h"
#include "raylib.h"
#include <string.h>

#define PLAYER_MAX_SHOTS 50
#define BURST_SIZE 3
#define BURST_INTERVAL 0.1f
#define RELOAD_TIME 0.6f

typedef struct Shoot {
	Vector2 speed;
	bool active;
	Color color;
	Vector2 position;
	int lifeSpawn;
	float radius;
} Shoot;

static Shoot shoot[PLAYER_MAX_SHOTS];
static float burstTimer = 0.0f;
static float reloadTimer = 0.0f;
static int burstCount = 0;
static bool isReloading = false;
static Texture2D playerSprite;
static float scale = 2.5f;

void Player_Init(void)
{
	memset(shoot, 0, sizeof(shoot));
	burstTimer = 0.0f;
	reloadTimer = 0.0f;
	burstCount = 0;
	isReloading = false;
	playerSprite = LoadTexture("assets/textures/player.png");
}

void Draw_Player(Vector2 *playerPosition) {
	Rectangle source = { 0, 0, (float)playerSprite.width, (float)playerSprite.height };
	Rectangle dest = { playerPosition->x, playerPosition->y, (float)playerSprite.width*scale, (float)playerSprite.height*scale};
	Vector2 origin = { (playerSprite.width*scale) / 2.0f, (playerSprite.height*scale) / 2.0f };
	DrawTexturePro(playerSprite, source, dest, origin, 0.0f, WHITE);
}

void Player_Unload(void)
{
	UnloadTexture(playerSprite);
}

void Player_HandleMovement(Vector2 *playerPosition, float playerRadius, float playerSpeed, int screenWidth, int screenHeight)
{
	if (IsKeyDown(KEY_W)) playerPosition->y -= playerSpeed;
	if (IsKeyDown(KEY_A)) playerPosition->x -= playerSpeed;
	if (IsKeyDown(KEY_S)) playerPosition->y += playerSpeed;
	if (IsKeyDown(KEY_D)) playerPosition->x += playerSpeed;

	if (playerPosition->x - playerRadius < -playerRadius / 2)
		playerPosition->x = -playerRadius / 2 + playerRadius;

	if (playerPosition->x + playerRadius > screenWidth + playerRadius / 2)
		playerPosition->x = screenWidth + playerRadius / 2 - playerRadius;

	if (playerPosition->y - playerRadius < 350)
		playerPosition->y = 350 + playerRadius;

	if (playerPosition->y + playerRadius > screenHeight + playerRadius / 2)
		playerPosition->y = screenHeight + playerRadius / 2 - playerRadius;
}

void Player_HandleShooting(float delta, Vector2 playerPosition)
{
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
}

void Player_UpdateShots(float delta)
{
	for (int i = 0; i < PLAYER_MAX_SHOTS; i++) {
		if (shoot[i].active) {
			shoot[i].position.y += shoot[i].speed.y * delta;
			shoot[i].lifeSpawn--;

			if (shoot[i].lifeSpawn <= 0 || shoot[i].position.y < 0) {
				shoot[i].active = false;
			}
		}
	}
}

void Player_DrawShots(void)
{
	for (int i = 0; i < PLAYER_MAX_SHOTS; i++) {
		if (shoot[i].active) {
			DrawCircleV(shoot[i].position, shoot[i].radius, shoot[i].color);
		}
	}
}