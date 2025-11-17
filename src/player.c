//  funções do jogador

#include "../include/player.h"
#include "raylib.h"
#include <string.h>
#include <math.h>
#include "../include/enemy.h"
#include "../include/gui.h"

#define PLAYER_MAX_SHOTS 50
#define BURST_SIZE 3
#define BURST_INTERVAL 0.1f
#define RELOAD_TIME 0.6f

// player shot lifetime (in frames since code uses an int counter); default doubled from 2600
#define PLAYER_SHOT_LIFE 5200

typedef struct Shoot {
	Vector2 speed;
	bool active;
	Color color;
	Vector2 position;
	int lifeSpawn;
	float radius;
} Shoot;

static Shoot shoot[PLAYER_MAX_SHOTS] = { 0};
static float burstTimer = 0.0f;
static float reloadTimer = 0.0f;
static int burstCount = 0;
static bool isReloading = false;
static int health = 3;
static float invulnTimer = 0.0f;

static Texture2D playerSprite;
static float base_draw_scale = 1.25f; // reduced by half

void Player_Init(void)
{
	// inicializa array de tiros
	for (int i = 0; i < PLAYER_MAX_SHOTS; i++) {
		shoot[i].active = false;
		shoot[i].position = (Vector2){0,0};
		shoot[i].speed = (Vector2){0,0};
		shoot[i].color = RAYWHITE;
		shoot[i].lifeSpawn = 0;
		shoot[i].radius = 0.0f;
	}
	burstTimer = 0.0f;
	reloadTimer = 0.0f;
	burstCount = 0;
	isReloading = false;
	health = 3;
	invulnTimer = 0.0f;
	playerSprite = LoadTexture("assets/textures/player.png");
}

// reseta estado do jogador sem recarregar sprite
void Player_Reset(void)
{
	for (int i = 0; i < PLAYER_MAX_SHOTS; i++) {
		shoot[i].active = false;
		shoot[i].position = (Vector2){0,0};
		shoot[i].speed = (Vector2){0,0};
		shoot[i].lifeSpawn = 0;
		shoot[i].radius = 0.0f;
		shoot[i].color = RAYWHITE;
	}
	burstTimer = 0.0f;
	reloadTimer = 0.0f;
	burstCount = 0;
	isReloading = false;
	health = 3;
	invulnTimer = 0.0f;
}

void Player_Draw(Vector2 *playerPosition) {
	float scale = GUI_GetScale() * base_draw_scale;
	Rectangle source = { 0, 0, (float)playerSprite.width, (float)playerSprite.height };
	Rectangle dest = { playerPosition->x, playerPosition->y, (float)playerSprite.width * scale, (float)playerSprite.height * scale };
	Vector2 origin = { (playerSprite.width * scale) / 2.0f, (playerSprite.height * scale) / 2.0f };
	DrawTexturePro(playerSprite, source, dest, origin, 0.0f, WHITE);

	if (invulnTimer > 0.0f) {
		Color wash = (Color){ 255, 255, 255, 120 };
		Vector2 center = { dest.x, dest.y };
		float radius = fmaxf(dest.width, dest.height) * 0.5f;
		DrawCircleV(center, radius, wash);
	}
}

void Player_Unload(void)
{
	UnloadTexture(playerSprite);
}

void Player_TakeDamage(int amount) {
	if (amount <= 0) return;
	if (invulnTimer > 0.0f) return;

	health -= amount;
	if (health < 0) health = 0;

	invulnTimer = 1.0f; 
}

int Player_GetHealth(void) {
	return health;
}

void Player_HandleMovement(Vector2 *playerPosition, float playerRadius, float playerSpeed, float leftBound, float rightBound, float topBound, float bottomBound)
{
	Vector2 movement = { 0.0f, 0.0f };
    
	if (IsKeyDown(KEY_W)) movement.y -= 1.0f;
	if (IsKeyDown(KEY_A)) movement.x -= 1.0f;
	if (IsKeyDown(KEY_S)) movement.y += 1.0f;
	if (IsKeyDown(KEY_D)) movement.x += 1.0f;

	// Normaliza o vetor de movimento para manter velocidade constante em diagonais
	float length = sqrtf(movement.x * movement.x + movement.y * movement.y);
	if (length > 0.0f) {
		movement.x = (movement.x / length) * playerSpeed;
		movement.y = (movement.y / length) * playerSpeed;
        
		playerPosition->x += movement.x;
		playerPosition->y += movement.y;
	}

	if (playerPosition->x - playerRadius < leftBound)
		playerPosition->x = leftBound + playerRadius;

	if (playerPosition->x + playerRadius > rightBound)
		playerPosition->x = rightBound - playerRadius;

	if (playerPosition->y - playerRadius < topBound)
		playerPosition->y = topBound + playerRadius;

	if (playerPosition->y + playerRadius > bottomBound)
		playerPosition->y = bottomBound - playerRadius;
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
					shoot[i].speed = (Vector2){ 0.0f, -700.0f };
					shoot[i].lifeSpawn = PLAYER_SHOT_LIFE;
					shoot[i].radius = 1.0f * GUI_GetScale(); // reduced by half again
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
	if (invulnTimer > 0.0f) {
		invulnTimer -= delta;
		if (invulnTimer < 0.0f) invulnTimer = 0.0f;
	}

	for (int i = 0; i < PLAYER_MAX_SHOTS; i++) {
		if (shoot[i].active) {
			shoot[i].position.y += shoot[i].speed.y * delta;
			shoot[i].lifeSpawn--;

			if (Enemies_CheckHit(shoot[i].position, shoot[i].radius)) {
				shoot[i].active = false;
				continue;
			}

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