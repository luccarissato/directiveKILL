// funções dos inimigos
#include "../include/enemy.h"
#include "raylib.h"
#include <math.h>
#include "../include/projectile.h"
#include "../include/game.h"

#define MAX_ENEMIES 10
#define GRID_COLS 9
#define GRID_ROWS 2

typedef struct Enemy {
    Vector2 speed;
	bool active;
	Color color;
	Vector2 position;
	int lifeSpawn;
    int hp;
    float radius;
    float targetY; // onde os inimigos devem parar na tela
    bool stopped;
    int type;
    int spawnRow;
    int spawnCol;
} Enemy;

Enemy enemies[MAX_ENEMIES] = { 0 };
static bool spawnOccupied[GRID_ROWS][GRID_COLS] = { 0 };
static float scale = 2.5f;
static int g_currentWave = 1;
static int g_enemiesThisWave = 1;
static float g_waveTimer = 0.0f;
static const float g_waveTimeout = 30.0f;
static float g_stopY = 120.0f;
static bool g_infinite = false;

static void SpawnWave(int count) {
    if (count > MAX_ENEMIES) count = MAX_ENEMIES;
    int screenW = GetScreenWidth();
    const float rowSpacing = 40.0f;
    const int cols = GRID_COLS;

    // decide qual linha usar: preferir a que tiver mais espaços livres
    int freeCount[GRID_ROWS] = {0};
    for (int r = 0; r < GRID_ROWS; r++) {
        for (int c = 0; c < GRID_COLS; c++) if (!spawnOccupied[r][c]) freeCount[r]++;
    }
    int chosenRow = 0;
    if (freeCount[1] > freeCount[0]) chosenRow = 1;

    // lista de colunas livres na linha escolhida
    int freeCols[GRID_COLS]; int freeColsCount = 0;
    for (int c = 0; c < GRID_COLS; c++) if (!spawnOccupied[chosenRow][c]) freeCols[freeColsCount++] = c;

    // se não houver colunas livres suficientes na linha, tenta a outra 
    if (freeColsCount < count) {
        int other = 1 - chosenRow;
        int otherCount = 0;
        for (int c = 0; c < GRID_COLS; c++) if (!spawnOccupied[other][c]) otherCount++;
        if (otherCount > freeColsCount) {
            chosenRow = other;
            freeColsCount = 0;
            for (int c = 0; c < GRID_COLS; c++) if (!spawnOccupied[chosenRow][c]) freeCols[freeColsCount++] = c;
        }
    }

    // limite de células disponíveis
    int spawnCount = count;
    if (spawnCount > freeColsCount) spawnCount = freeColsCount;

    // embaralha as colunas livres (aleatorizações nos spawns)
    for (int k = freeColsCount - 1; k > 0; k--) {
        int r = GetRandomValue(0, k);
        int tmp = freeCols[k]; freeCols[k] = freeCols[r]; freeCols[r] = tmp;
    }

    float cellX = (float)screenW / (cols + 1);

    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (i < spawnCount) {
            int col = freeCols[i];
            float px = cellX * (col + 1);
            int spawnY = -GetRandomValue(16, 48);
            enemies[i].position = (Vector2){ px, (float)spawnY };
            // velocidade inicial para descer
            enemies[i].speed = (Vector2){ 0, (float)GetRandomValue(30, 80) / 60.0f };
            enemies[i].radius = 20.0f;
            enemies[i].active = true;
            enemies[i].stopped = false;
            enemies[i].hp = 3;
            enemies[i].type = GetRandomValue(0, 1);
            float jitter = (float)GetRandomValue(-10, 10);
            enemies[i].targetY = g_stopY + chosenRow * rowSpacing + jitter;
            enemies[i].color = (enemies[i].type == 1) ? RED : WHITE;
            enemies[i].spawnRow = chosenRow;
            enemies[i].spawnCol = col;
            spawnOccupied[chosenRow][col] = true;
        } else {
            // desativa os inimigos não utilizados
            enemies[i].active = false;
            enemies[i].stopped = false;
            enemies[i].hp = 0;
            enemies[i].spawnRow = -1;
            enemies[i].spawnCol = -1;
        }
    }
}

int Enemies_GetCurrentWave(void) {
    return g_currentWave;
}

void Enemies_Init(float stopY) {
    g_stopY = stopY;
    g_currentWave = 1;
    g_enemiesThisWave = 1;
    g_waveTimer = 0.0f;
    g_infinite = false;

    // limpa as células ocupadas e as reseta
    for (int r = 0; r < GRID_ROWS; r++) for (int c = 0; c < GRID_COLS; c++) spawnOccupied[r][c] = false;
    for (int i = 0; i < MAX_ENEMIES; i++) { enemies[i].spawnRow = -1; enemies[i].spawnCol = -1; }

    SpawnWave(g_enemiesThisWave);
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

    int activeCount = 0;
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i].active) activeCount++;
    }

    g_waveTimer += dt;

    if (activeCount == 0 || g_waveTimer >= g_waveTimeout) {
        int nextDesired = 1;
        if (!g_infinite) {
            int nextWave = g_currentWave + 1;
            int candidate = 2 * nextWave - 1;
            if (candidate > 9) {
                // se a ultima wave foi a ultima, passa para o modo infinito
                if (g_enemiesThisWave == 9) {
                    g_infinite = true;
                    nextDesired = GetRandomValue(1, 9);
                    g_currentWave = nextWave;
                } else {
                    // cap to 9 for this next wave
                    nextDesired = 9;
                    g_currentWave = nextWave;
                }
            } else {
                nextDesired = candidate;
                g_currentWave = nextWave;
            }
        } else {
            g_currentWave++;
            nextDesired = GetRandomValue(1, 9);
        }

        g_enemiesThisWave = nextDesired;
        SpawnWave(g_enemiesThisWave);
        g_waveTimer = 0.0f;
    }
}

bool Enemies_CheckHit(Vector2 pos, float radius) {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!enemies[i].active) continue;

        float dx = enemies[i].position.x - pos.x;
        float dy = enemies[i].position.y - pos.y;
        float dist2 = dx*dx + dy*dy;
        float minDist = enemies[i].radius + radius;
        if (dist2 <= minDist * minDist) {
            enemies[i].hp -= 1;
            if (enemies[i].hp <= 0) {
                enemies[i].active = false;
                // free spawn cell if this enemy had one
                if (enemies[i].spawnRow >= 0 && enemies[i].spawnCol >= 0) {
                    int sr = enemies[i].spawnRow;
                    int sc = enemies[i].spawnCol;
                    if (sr >= 0 && sr < GRID_ROWS && sc >= 0 && sc < GRID_COLS) spawnOccupied[sr][sc] = false;
                    enemies[i].spawnRow = -1; enemies[i].spawnCol = -1;
                }
                Game_AddScore(100);
            }
            return true;
        }
    }
    return false;
}

void Enemies_Draw(Texture2D enemySprite) {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!enemies[i].active) continue;

        Rectangle source = { 0, 0, enemySprite.width, enemySprite.height };
        Rectangle dest = { enemies[i].position.x, enemies[i].position.y, enemySprite.width * scale, enemySprite.height * scale };
        Vector2 origin = { (enemySprite.width * scale) / 2.0f, (enemySprite.height * scale) / 2.0f };

        DrawTexturePro(enemySprite, source, dest, origin, 0.0f, enemies[i].color);
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

void Enemies_ShootAll(Vector2 playerPos) {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!enemies[i].active) continue;
        Projectiles_Type(enemies[i].type, enemies[i].position, playerPos);
    }
}
