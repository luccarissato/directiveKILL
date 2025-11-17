// funções dos inimigos
#include "../include/enemy.h"
#include "raylib.h"
#include <math.h>
#include "../include/projectile.h"
#include "../include/game.h"
#include "../include/gui.h"

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
    float homeX;
    float motionTimer;
    float wigglePhase;
    float wiggleAmp;
    float wiggleFreq;
} Enemy;

Enemy enemies[MAX_ENEMIES] = { 0 };
static bool spawnOccupied[GRID_ROWS][GRID_COLS] = { 0 };
static float base_draw_scale = 1.25f; // reduced by half
static int g_currentWave = 1;
static int g_enemiesThisWave = 1;
static float g_waveTimer = 0.0f;
static const float g_waveTimeout = 30.0f;
static float g_stopY = 120.0f;
static bool g_infinite = false;
static float g_playLeft = 0.0f;
static float g_playRight = 0.0f;

static void SpawnWave(int count) {
    if (count > MAX_ENEMIES) count = MAX_ENEMIES;
    int screenW = GetScreenWidth();
    const float rowSpacing = 40.0f;
    const int cols = GRID_COLS;

    int freeCount[GRID_ROWS] = {0};
    for (int r = 0; r < GRID_ROWS; r++) {
        for (int c = 0; c < GRID_COLS; c++) if (!spawnOccupied[r][c]) freeCount[r]++;
    }
    int chosenRow = 0;
    if (freeCount[1] > freeCount[0]) chosenRow = 1;

    int freeCols[GRID_COLS]; int freeColsCount = 0;
    for (int c = 0; c < GRID_COLS; c++) if (!spawnOccupied[chosenRow][c]) freeCols[freeColsCount++] = c;

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

    int spawnCount = count;
    if (spawnCount > freeColsCount) spawnCount = freeColsCount;

    for (int k = freeColsCount - 1; k > 0; k--) {
        int r = GetRandomValue(0, k);
        int tmp = freeCols[k]; freeCols[k] = freeCols[r]; freeCols[r] = tmp;
    }

    float playLeft = 0.0f;
    float playRight = (float)screenW;
    if (g_playRight > g_playLeft) { playLeft = g_playLeft; playRight = g_playRight; }
    float playWidth = playRight - playLeft;
    float cellX = playWidth / (cols + 1);

    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (i < spawnCount) {
            int col = freeCols[i];
            float px = playLeft + cellX * (col + 1);
            int spawnY = -GetRandomValue(16, 48);
            enemies[i].position = (Vector2){ px, (float)spawnY };
            enemies[i].speed = (Vector2){ 0, (float)GetRandomValue(30, 80) / 60.0f };
            enemies[i].radius = 10.0f * GUI_GetScale();
            enemies[i].active = true;
            enemies[i].stopped = false;
            enemies[i].hp = 3;
            enemies[i].type = GetRandomValue(0, 2);
            float jitter = (float)GetRandomValue(-10, 10);
            enemies[i].targetY = g_stopY + chosenRow * rowSpacing + jitter;
            if (enemies[i].type == 2) {
                enemies[i].color = WHITE;
            } else {
                enemies[i].color = (enemies[i].type == 1) ? RED : WHITE;
            }
            enemies[i].spawnRow = chosenRow;
            enemies[i].spawnCol = col;
            enemies[i].homeX = px;
            enemies[i].motionTimer = 0.0f;
            enemies[i].wigglePhase = (float)GetRandomValue(0, 6283) / 1000.0f;
            if (enemies[i].type == 0) {
                enemies[i].wiggleAmp = (float)GetRandomValue(12, 28); 
                enemies[i].wiggleFreq = (float)GetRandomValue(80, 140) / 100.0f;
            } else {
                enemies[i].wiggleAmp = (float)GetRandomValue(10, 22);
                enemies[i].wiggleFreq = (float)GetRandomValue(60, 120) / 100.0f;
            }
            spawnOccupied[chosenRow][col] = true;
        } else {
            enemies[i].active = false;
            enemies[i].stopped = false;
            enemies[i].hp = 0;
            enemies[i].spawnRow = -1;
            enemies[i].spawnCol = -1;
        }
    }
}

void Enemies_SetPlayArea(float leftX, float rightX) {
    if (rightX <= leftX) return;
    g_playLeft = leftX;
    g_playRight = rightX;
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

    for (int r = 0; r < GRID_ROWS; r++) for (int c = 0; c < GRID_COLS; c++) spawnOccupied[r][c] = false;
    for (int i = 0; i < MAX_ENEMIES; i++) { enemies[i].spawnRow = -1; enemies[i].spawnCol = -1; }

    SpawnWave(g_enemiesThisWave);
}

void Enemies_UpdateStopY(float newStopY) {
    float delta = newStopY - g_stopY;
    g_stopY = newStopY;
    
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i].active) {
            enemies[i].targetY += delta;
        }
    }
}

void Enemies_Update(void) {
    float dt = GetFrameTime();
    const float smoothing = 6.0f;

    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!enemies[i].active) continue;

        if (!enemies[i].stopped) {
            float toTarget = enemies[i].targetY - enemies[i].position.y;

            float factor = smoothing * dt;
            if (factor > 1.0f) factor = 1.0f;

            enemies[i].position.y += toTarget * factor;

            if (fabsf(enemies[i].targetY - enemies[i].position.y) <= 0.5f) {
                enemies[i].position.y = enemies[i].targetY;
                enemies[i].stopped = true;
                enemies[i].speed.y = 0.0f;
                enemies[i].motionTimer = 0.0f;
            }
        } else {
            enemies[i].motionTimer += dt;
            float t = enemies[i].motionTimer;

            if (enemies[i].type == 0) {
                float amp = enemies[i].wiggleAmp;
                float freq = enemies[i].wiggleFreq;
                float mainOsc = sinf(t * freq + enemies[i].wigglePhase) * amp;
                float noise = sinf(t * (freq * 1.73f) + enemies[i].wigglePhase * 1.31f) * (amp * 0.25f);
                float targetX = enemies[i].homeX + mainOsc + noise;

                const float xSmooth = 8.0f;
                enemies[i].position.x += (targetX - enemies[i].position.x) * fminf(dt * xSmooth, 1.0f);

                float maxOff = amp * 1.5f;
                if (enemies[i].position.x > enemies[i].homeX + maxOff) enemies[i].position.x = enemies[i].homeX + maxOff;
                if (enemies[i].position.x < enemies[i].homeX - maxOff) enemies[i].position.x = enemies[i].homeX - maxOff;
            }
            else if (enemies[i].type == 1) {
                float ampX = enemies[i].wiggleAmp;
                float ampY = enemies[i].wiggleAmp * 0.6f;
                float freq = enemies[i].wiggleFreq;
                float phase = enemies[i].wigglePhase;

                float ox = cosf(t * freq + phase) * ampX;
                float oy = fabsf(sinf(t * freq + phase)) * ampY;

                float targetX = enemies[i].homeX + ox;
                float targetY = enemies[i].targetY + oy;

                const float smooth = 6.0f;
                enemies[i].position.x += (targetX - enemies[i].position.x) * fminf(dt * smooth, 1.0f);
                enemies[i].position.y += (targetY - enemies[i].position.y) * fminf(dt * smooth, 1.0f);
            }
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
                if (g_enemiesThisWave == 9) {
                    g_infinite = true;
                    nextDesired = GetRandomValue(1, 9);
                    g_currentWave = nextWave;
                } else {
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

void Enemies_Draw(Texture2D enemySprite, Texture2D scoutSprite, Texture2D soldierSprite) {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!enemies[i].active) continue;

        Texture2D currentSprite;
        Color tint = WHITE;

        if (enemies[i].type == 1) {
            currentSprite = scoutSprite;
            tint = WHITE;
        } else if (enemies[i].type == 2) {
            currentSprite = soldierSprite;
            tint = WHITE;
        } else {
            currentSprite = enemySprite;
            tint = enemies[i].color;
        }

        Rectangle source = { 0, 0, (float)currentSprite.width, (float)currentSprite.height };
        Rectangle dest = { enemies[i].position.x, enemies[i].position.y, currentSprite.width, currentSprite.height};
        Vector2 origin = { (currentSprite.width) / 2.0f, (currentSprite.height) / 2.0f };

        DrawTexturePro(currentSprite, source, dest, origin, 0.0f, tint);
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
