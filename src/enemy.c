// funções dos inimigos
#include "../include/enemy.h"
#include "raylib.h"
#include "../include/player.h"
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
    int maxHp;
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
    bool isBoss;
    float laserCooldown;
    float laserTimer;
    float laserDamageAccum;
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
static float g_playLeft = 0.0f;
static float g_playRight = 0.0f;

static void SpawnWave(int count) {
    if (count > MAX_ENEMIES) count = MAX_ENEMIES;
    int screenW = GetScreenWidth();
    const float rowSpacing = 40.0f;
    const int cols = GRID_COLS;

    if (g_currentWave == 6) {
        for (int i = 0; i < MAX_ENEMIES; i++) {
            enemies[i].active = false;
            enemies[i].stopped = false;
            enemies[i].hp = 0;
            enemies[i].maxHp = 0;
            enemies[i].spawnRow = -1;
            enemies[i].spawnCol = -1;
            enemies[i].isBoss = false;
            enemies[i].laserCooldown = 0.0f;
            enemies[i].laserTimer = 0.0f;
            enemies[i].laserDamageAccum = 0.0f;
        }
        for (int r = 0; r < GRID_ROWS; r++) for (int c = 0; c < GRID_COLS; c++) spawnOccupied[r][c] = false;

        float centerX = (float)screenW * 0.5f;
        int spawnY = -GetRandomValue(80, 120);
        enemies[0].position = (Vector2){ centerX, (float)spawnY };
        enemies[0].speed = (Vector2){ 0, (float)GetRandomValue(20, 60) / 60.0f };
        enemies[0].radius = 48.0f;
        enemies[0].active = true;
        enemies[0].stopped = false;
        enemies[0].hp = 60;
        enemies[0].maxHp = 60;
        enemies[0].type = 0;
        enemies[0].targetY = g_stopY + rowSpacing;
        enemies[0].color = WHITE;
        enemies[0].spawnRow = -1;
        enemies[0].spawnCol = -1;
        enemies[0].homeX = centerX;
        enemies[0].motionTimer = 0.0f;
        enemies[0].wigglePhase = (float)GetRandomValue(0, 6283) / 1000.0f;
        enemies[0].wiggleAmp = (float)GetRandomValue(20, 40);
        enemies[0].wiggleFreq = (float)GetRandomValue(60, 140) / 100.0f;
        enemies[0].laserCooldown = 3.0f;
        enemies[0].laserTimer = 0.0f;
        enemies[0].laserDamageAccum = 0.0f;
        enemies[0].isBoss = true;
        return;
    }

    int freeCount[GRID_ROWS] = {0};
    for (int r = 0; r < GRID_ROWS; r++) {
        for (int c = 0; c < GRID_COLS; c++) if (!spawnOccupied[r][c]) freeCount[r]++;
    }
    int chosenRow;
    if (freeCount[1] > freeCount[0]) chosenRow = 1;
    else if (freeCount[0] > freeCount[1]) chosenRow = 0;
    else chosenRow = GetRandomValue(0, 1);

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
            enemies[i].radius = 20.0f;
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
            enemies[i].isBoss = false;
            enemies[i].laserDamageAccum = 0.0f;
            spawnOccupied[chosenRow][col] = true;
        } else {
            enemies[i].active = false;
            enemies[i].stopped = false;
            enemies[i].hp = 0;
            enemies[i].spawnRow = -1;
            enemies[i].spawnCol = -1;
            enemies[i].isBoss = false;
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
    for (int i = 0; i < MAX_ENEMIES; i++) { enemies[i].spawnRow = -1; enemies[i].spawnCol = -1; enemies[i].isBoss = false; }

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

void Enemies_Update(Vector2 playerPos) {
    float dt = GetFrameTime();
    const float smoothing = 6.0f;

    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!enemies[i].active) continue;

        if (enemies[i].isBoss) {
            if (enemies[i].laserTimer > 0.0f) {
                enemies[i].laserTimer -= dt;
                if (enemies[i].laserTimer < 0.0f) enemies[i].laserTimer = 0.0f;

                int sh = GetScreenHeight();
                float lw = fmaxf(28.0f, enemies[i].radius * 0.9f) * (scale * 0.9f);
                float lx = enemies[i].position.x - lw * 0.5f;

                float laserStartY = enemies[i].position.y + (enemies[i].radius * scale * 0.5f) + 8.0f;

                bool playerInBeam = (playerPos.x >= lx && playerPos.x <= lx + lw && playerPos.y >= laserStartY && playerPos.y <= (float)sh);
                if (playerInBeam) {
                    enemies[i].laserDamageAccum += dt;
                    while (enemies[i].laserDamageAccum >= 0.25f) {
                        Player_TakeDamage(1);
                        enemies[i].laserDamageAccum -= 0.25f;
                    }
                } else {
                    enemies[i].laserDamageAccum = 0.0f;
                }

            } else {
                enemies[i].laserCooldown -= dt;
                if (enemies[i].laserCooldown <= 0.0f) {
                    enemies[i].laserTimer = 3.0f;
                    enemies[i].laserCooldown = 3.0f; 
                }
            }
        }

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

            if (enemies[i].type == 0 || enemies[i].type == 2 || enemies[i].isBoss) {
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

        if (enemies[i].isBoss) {
            float arcOffset = enemies[i].radius * 2.0f;
            float arcYOffset = -enemies[i].radius * 0.15f;

            Vector2 arc3Pos = { enemies[i].position.x - arcOffset, enemies[i].position.y + arcYOffset };
            Vector2 arc4Pos = { enemies[i].position.x + arcOffset, enemies[i].position.y + arcYOffset };

            float dx3 = arc3Pos.x - pos.x;
            float dy3 = arc3Pos.y - pos.y;
            float dist23 = dx3*dx3 + dy3*dy3;
            float minDistArc = enemies[i].radius * 0.9f + radius;

            if (dist23 <= minDistArc * minDistArc) {
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

            float dx4 = arc4Pos.x - pos.x;
            float dy4 = arc4Pos.y - pos.y;
            float dist24 = dx4*dx4 + dy4*dy4;
            if (dist24 <= minDistArc * minDistArc) {
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
    }
    return false;
}

void Enemies_Draw(Texture2D enemySprite, Texture2D scoutSprite, Texture2D soldierSprite, Texture2D arcSprite, Texture2D arc2Sprite, Texture2D arc3Sprite, Texture2D arc4Sprite) {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!enemies[i].active) continue;

        Texture2D currentSprite;
        Color tint = WHITE;

        if (enemies[i].isBoss) {
            currentSprite = arcSprite;
            tint = WHITE;
        } else if (enemies[i].type == 1) {
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
        Rectangle dest = { enemies[i].position.x, enemies[i].position.y, currentSprite.width * scale, currentSprite.height * scale };
        Vector2 origin = { (currentSprite.width * scale) / 2.0f, (currentSprite.height * scale) / 2.0f };

        if (enemies[i].isBoss && arc2Sprite.width > 0 && arc2Sprite.height > 0) {
            float halfH = (currentSprite.height * scale) / 2.0f;
            float topY = enemies[i].position.y - halfH;

            if (arc3Sprite.width > 0 && arc3Sprite.height > 0) {
                float arc2CenterX = enemies[i].position.x;
                float arc3CenterX = arc2CenterX - (arc2Sprite.width * scale) / 2.0f - (arc3Sprite.width * scale) / 2.0f;

                Rectangle source3 = { 0, 0, (float)arc3Sprite.width, (float)arc3Sprite.height };
                Rectangle dest3 = { arc3CenterX, topY, arc3Sprite.width * scale, arc3Sprite.height * scale };
                Vector2 origin3 = { (arc3Sprite.width * scale) / 2.0f, arc3Sprite.height * scale };
                DrawTexturePro(arc3Sprite, source3, dest3, origin3, 0.0f, WHITE);
            }

            Rectangle source2 = { 0, 0, (float)arc2Sprite.width, (float)arc2Sprite.height };
            Rectangle dest2 = { enemies[i].position.x, topY, arc2Sprite.width * scale, arc2Sprite.height * scale };
            Vector2 origin2 = { (arc2Sprite.width * scale) / 2.0f, arc2Sprite.height * scale };
            DrawTexturePro(arc2Sprite, source2, dest2, origin2, 0.0f, WHITE);

            if (arc4Sprite.width > 0 && arc4Sprite.height > 0) {
                float arc2CenterX = enemies[i].position.x;
                float arc4CenterX = arc2CenterX + (arc2Sprite.width * scale) / 2.0f + (arc4Sprite.width * scale) / 2.0f;

                Rectangle source4 = { 0, 0, (float)arc4Sprite.width, (float)arc4Sprite.height };
                Rectangle dest4 = { arc4CenterX, topY, arc4Sprite.width * scale, arc4Sprite.height * scale };
                Vector2 origin4 = { (arc4Sprite.width * scale) / 2.0f, arc4Sprite.height * scale };
                DrawTexturePro(arc4Sprite, source4, dest4, origin4, 0.0f, WHITE);
            }


            if (enemies[i].laserTimer > 0.0f) {
                int sh = GetScreenHeight();
                float lw = fmaxf(28.0f, enemies[i].radius * 0.9f) * (scale * 0.9f);
                float lx = enemies[i].position.x - lw * 0.5f;

                float bossHalfH = (currentSprite.height * scale) / 2.0f;
                float bossBottomY = enemies[i].position.y + bossHalfH;
                float laserStartY = bossBottomY + 8.0f;
                float laserH = (float)sh - laserStartY;

                DrawRectangleRec((Rectangle){ lx, laserStartY, lw, laserH }, (Color){ 255, 220, 220, 200 });
                float coreW = lw * 0.35f;
                float coreX = enemies[i].position.x - coreW * 0.5f;
                DrawRectangleRec((Rectangle){ coreX, laserStartY, coreW, laserH }, (Color){ 255, 180, 180, 255 });

                DrawRectangleRec((Rectangle){ coreX - 2.0f, laserStartY + sh * 0.05f, 2.0f, sh * 0.9f }, (Color){ 255, 120, 120, 80 });
                DrawRectangleRec((Rectangle){ coreX + coreW, laserStartY + sh * 0.05f, 2.0f, sh * 0.9f }, (Color){ 255, 120, 120, 80 });

                float step = 40.0f * (scale * 0.9f);
                for (float y = laserStartY + sh * 0.08f; y < sh * 0.95f; y += step) {
                    DrawCircleV((Vector2){ enemies[i].position.x, y }, 2.5f * (scale * 0.6f), (Color){ 200, 60, 60, 220 });
                }
            }
        }

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
        if (enemies[i].isBoss) {
            const float sideOffset = enemies[i].radius * 0.9f;
            const float spawnYOff = 10.0f;
            const float speed = 320.0f;
            const float bulletRadius = 4.0f;
            const int damage = 1;
            const float life = 6.0f;
            const int bursts = 3;
            const float burstDelay = 0.22f;
            const float spreadDeg = 10.0f;
            const float centerLeft = 110.0f;
            const float centerRight = 70.0f;

            for (int b = 0; b < bursts; b++) {
                float delay = b * burstDelay;

                for (int k = -1; k <= 1; k++) {
                    float ang = (centerLeft + k * spreadDeg) * DEG2RAD;
                    Vector2 vel = { cosf(ang) * speed, sinf(ang) * speed };
                    Vector2 spawn = { enemies[i].position.x - sideOffset, enemies[i].position.y + spawnYOff };
                    Projectiles_Spawn(spawn, vel, bulletRadius, damage, WHITE, life, delay, 0.0f);
                }

                for (int k = -1; k <= 1; k++) {
                    float ang = (centerRight + k * spreadDeg) * DEG2RAD;
                    Vector2 vel = { cosf(ang) * speed, sinf(ang) * speed };
                    Vector2 spawn = { enemies[i].position.x + sideOffset, enemies[i].position.y + spawnYOff };
                    Projectiles_Spawn(spawn, vel, bulletRadius, damage, WHITE, life, delay, 0.0f);
                }
            }
        } else {
            Projectiles_Type(enemies[i].type, enemies[i].position, playerPos);
        }
    }
}
