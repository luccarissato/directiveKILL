// funções dos inimigos
#include "../include/enemy.h"
#include "raylib.h"
#include <math.h>

#define MAX_ENEMIES 1

typedef struct Enemy {
    Vector2 speed;
	bool active;
	Color color;
	Vector2 position;
	int lifeSpawn;
    float radius;
    float targetY; // onde os inimigos devem parar na tela
    bool stopped;
    int type;
} Enemy;

Enemy enemies[MAX_ENEMIES] = { 0 };
static float scale = 2.5f;

void Enemies_Init(float stopY) {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        // aparecem um pouco acima na tela (antes de entrar em visão)
        int spawnX = GetRandomValue(0, GetScreenWidth());
        int spawnY = -GetRandomValue(16, 48);
        enemies[i].position = (Vector2){ (float)spawnX, (float)spawnY };
        // velocidade vertical
        enemies[i].speed = (Vector2){ 0, (float)GetRandomValue(30, 80) / 60.0f }; // aprox 0.5 - 1.3 por frame
        enemies[i].radius = 20.0f;
        enemies[i].active = true;
        enemies[i].stopped = false;
        // variação no limite Y para parar os inimigos
        float jitter = (float)GetRandomValue(-30, 30);
        enemies[i].targetY = stopY + jitter;
    }
}

void Enemies_Update(void) {
    // parada mais suavizada dos inimigos sem depender de FPS
    float dt = GetFrameTime();
    const float smoothing = 6.0f; // para que inimigos maiores possam chegar mais rápido

    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!enemies[i].active) continue;
        if (enemies[i].stopped) continue;

        float toTarget = enemies[i].targetY - enemies[i].position.y;

        // interpolação necessária pra suavização, factor sendo a distância que o inimigo percorre nesse frame
        float factor = smoothing * dt;
        // evita ultrapassar o alvo de uma vez só
        if (factor > 1.0f) factor = 1.0f;

        enemies[i].position.y += toTarget * factor;

        // quando a diferença for menor que 0.5 pixels, para direto
        if (fabsf(enemies[i].targetY - enemies[i].position.y) <= 0.5f) {
            enemies[i].position.y = enemies[i].targetY;
            enemies[i].stopped = true;
            enemies[i].speed.y = 0.0f;
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

bool Enemies_GetFirstActivePosition(Vector2 *outPos, int *outIndex) {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!enemies[i].active) continue;
        if (outPos) *outPos = enemies[i].position;
        if (outIndex) *outIndex = i;
        return true;
    }
    return false;
}
