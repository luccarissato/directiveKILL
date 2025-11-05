// funções dos inimigos
#include "../include/enemy.h"
#include "raylib.h"

#define MAX_ENEMIES 20

typedef struct Enemy {
    Vector2 speed;
	bool active;
	Color color;
	Vector2 position;
	int lifeSpawn;
	float radius;
} Enemy;

Enemy enemies[MAX_ENEMIES] = { 0 };
static float scale = 2.5f;

void Enemies_Init(void) {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        enemies[i].position = (Vector2){GetRandomValue(0, 800), GetRandomValue(0, 200) };
        enemies[i].speed = (Vector2){ 0, GetRandomValue(1, 3) };
        enemies[i].radius = 20.0f;
        enemies[i].active = true;
    }
}

void Enemies_Update(void) {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!enemies[i].active) continue;

        enemies[i].position.y += enemies[i].speed.y;

        if (enemies[i].position.y > GetScreenHeight()) {
            enemies[i].position.y = 0;
            enemies[i].position.x = GetRandomValue(0, GetScreenWidth());
        }
    }
}

void Enemies_Draw(Texture2D enemySprite) {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!enemies[i].active) continue;

        Rectangle source = { 0, 0, enemySprite.width, enemySprite.height };
        Rectangle dest = { enemies[i].position.x, enemies[i].position.y, enemySprite.width * scale, enemySprite.height * scale };
        Vector2 origin = { (enemySprite.width * scale) / 2.0f, (enemySprite.height * scale) / 2.0f };

        DrawTexturePro(enemySprite, source, dest, origin, 0.0f, WHITE);
    }
}