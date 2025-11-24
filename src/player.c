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

typedef struct Shoot {
	Vector2 speed;
	bool active;
	Color color;
	Vector2 position;
	float radius;
} Shoot;

static Shoot shoot[PLAYER_MAX_SHOTS] = { 0 };
static float burstTimer = 0.0f;
static float reloadTimer = 0.0f;
static int burstCount = 0;
static bool isReloading = false;
static int health = 1;
static float invulnTimer = 0.0f;
static Texture2D playerTex_neutral;
static Texture2D playerTex_left;
static Texture2D playerTex_right;
static Texture2D *currentPlayerTex = NULL;
static Texture2D bulletSprite;
static float base_draw_scale = 1.25f;

// Estado da Animação
static int animState = 0;

void Player_Init(void)
{
	// inicializa array de tiros
	for (int i = 0; i < PLAYER_MAX_SHOTS; i++) {
		shoot[i].active = false;
		shoot[i].position = (Vector2){0,0};
		shoot[i].speed = (Vector2){0,0};
		shoot[i].color = RAYWHITE;
		shoot[i].radius = 0.0f;
	}
	burstTimer = 0.0f;
	reloadTimer = 0.0f;
	burstCount = 0;
	isReloading = false;
	health = 3;
	invulnTimer = 0.0f;
	playerTex_neutral = LoadTexture("assets/textures/player.png");
	playerTex_left = LoadTexture("assets/textures/player1.png");
	playerTex_right = LoadTexture("assets/textures/player2.png");
	bulletSprite = LoadTexture("assets/textures/bullet_player.png");
	currentPlayerTex = &playerTex_neutral;
	animState = 0;
}

// reseta estado do jogador sem recarregar sprite
void Player_Reset(void)
{
	for (int i = 0; i < PLAYER_MAX_SHOTS; i++) {
		shoot[i].active = false;
		shoot[i].position = (Vector2){0,0};
		shoot[i].speed = (Vector2){0,0};
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
	Texture2D drawTex = currentPlayerTex ? *currentPlayerTex : playerTex_neutral;
	Rectangle source = { 0, 0, (float)drawTex.width, (float)drawTex.height };
	Rectangle dest = { playerPosition->x, playerPosition->y, (float)drawTex.width * scale, (float)drawTex.height * scale };
	Vector2 origin = { (drawTex.width * scale) / 2.0f, (drawTex.height * scale) / 2.0f };
	DrawTexturePro(drawTex, source, dest, origin, 0.0f, WHITE);

	if (invulnTimer > 0.0f) {
		Color wash = (Color){ 255, 255, 255, 120 };
		Vector2 center = { dest.x, dest.y };
		float radius = fmaxf(dest.width, dest.height) * 0.5f;
		DrawCircleV(center, radius, wash);
	}
}

void Player_Unload(void)
{
	UnloadTexture(playerTex_neutral);
	UnloadTexture(playerTex_left);
	UnloadTexture(playerTex_right);
	UnloadTexture(bulletSprite);
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

void Player_HandleMovement(float delta, Vector2 *playerPosition, float playerRadius, float playerSpeed, float leftBound, float rightBound, float topBound, float bottomBound)
{
	Vector2 movement = { 0.0f, 0.0f };
    
	if (IsKeyDown(KEY_W)) movement.y -= 1.0f;
	if (IsKeyDown(KEY_A)) movement.x -= 1.0f;
	if (IsKeyDown(KEY_S)) movement.y += 1.0f;
	if (IsKeyDown(KEY_D)) movement.x += 1.0f;

	float length = sqrtf(movement.x * movement.x + movement.y * movement.y);
	if (length > 0.0f) {
		movement.x = (movement.x / length) * playerSpeed;
		movement.y = (movement.y / length) * playerSpeed;
        
		playerPosition->x += movement.x;
		playerPosition->y += movement.y;
	}

	int inputDir = 0;
	bool leftDown = IsKeyDown(KEY_A);
	bool rightDown = IsKeyDown(KEY_D);
	if (rightDown && !leftDown) inputDir = 2;
	else if (leftDown && !rightDown) inputDir = 1;
	else inputDir = 0;

	// Mudança de sprite com input
	if (inputDir != animState) {
		if (inputDir == 0) {
			animState = 0;
			currentPlayerTex = &playerTex_neutral;
		}
		else if (inputDir == 1) {
			animState = 1;
			currentPlayerTex = &playerTex_left;
		}
		else if (inputDir == 2) {
			animState = 2;
			currentPlayerTex = &playerTex_right;
		}
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
					shoot[i].radius = 0.75f * GUI_GetScale();
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

			if (Enemies_CheckHit(shoot[i].position, shoot[i].radius)) {
				shoot[i].active = false;
				continue;
			}

			if (shoot[i].position.y < 0) {
				shoot[i].active = false;
			}
		}
	}
}

void Player_DrawShots(void)
{
	float scale = GUI_GetScale() * 0.375f;
	const float BULLET_ANGLE_OFFSET = 90.0f;
	for (int i = 0; i < PLAYER_MAX_SHOTS; i++) {
		if (shoot[i].active) {
			Texture2D tex = bulletSprite;
			if (tex.width <= 0 || tex.height <= 0) {
				DrawCircleV(shoot[i].position, shoot[i].radius, shoot[i].color);
			} else {
				Rectangle source = { 0, 0, (float)tex.width, (float)tex.height };
				if (shoot[i].speed.x < 0.0f) source.width = -(float)tex.width;
				if (shoot[i].speed.y > 0.0f) source.height = -(float)tex.height;
				float angle = atan2f(shoot[i].speed.y, shoot[i].speed.x) * RAD2DEG + BULLET_ANGLE_OFFSET;
				Rectangle dest = { shoot[i].position.x, shoot[i].position.y, fabsf(source.width) * scale, fabsf(source.height) * scale };
				Vector2 origin = { fabsf(source.width) * scale / 2.0f, fabsf(source.height) * scale / 2.0f };
				DrawTexturePro(tex, source, dest, origin, angle, shoot[i].color);
			}
		}
	}
}