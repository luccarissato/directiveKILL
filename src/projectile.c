// funções de projeteis
#include "../include/projectile.h"
#include "raylib.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

static Projectile *g_pool = NULL;
static int g_poolSize = 0;
static Vector2 g_playerPos = {0,0};
static const float BULLET_ANGLE_OFFSET = 90.0f; // corrige orientação da sprite

void Projectiles_Init(int maxProjectiles)
{
    if (g_pool) Projectiles_Free();
    g_pool = (Projectile*)malloc(sizeof(Projectile) * maxProjectiles);
    if (!g_pool) return;
    g_poolSize = maxProjectiles;
    memset(g_pool, 0, sizeof(Projectile) * maxProjectiles);
}

void Projectiles_Free(void)
{
    if (g_pool) {
        free(g_pool);
        g_pool = NULL;
        g_poolSize = 0;
    }
}

void Projectiles_Spawn(Vector2 pos, Vector2 vel, float radius, int damage, Color color, float lifeSec, float delaySec, float homingSpeed)
{
    if (!g_pool) return;
    for (int i = 0; i < g_poolSize; i++) {
        if (!g_pool[i].active) {
            g_pool[i].active = true;
            g_pool[i].position = pos;
            g_pool[i].velocity = vel;
            g_pool[i].radius = radius;
            // age marca o tempo desde o spawn, se negativo é porque está em delay, life é o tempo ativo
            g_pool[i].life = lifeSec;
            g_pool[i].age = -fabsf(delaySec);
            g_pool[i].damage = damage;
            g_pool[i].color = color;
            g_pool[i].homingSpeed = homingSpeed;
            g_pool[i].willHome = (homingSpeed > 0.0001f);
            g_pool[i].visualType = 0;
            g_pool[i].flipSprite = false;
            g_pool[i].flipY = (vel.y > 0.0f);
            g_pool[i].willSplit = false;
            g_pool[i].angleDeg = 0.0f;
            g_pool[i].spinSpeedDeg = 0.0f;
            break;
        }
    }
}

void Projectiles_SetPlayerPosition(Vector2 pos)
{
    g_playerPos = pos;
}

void Projectiles_Type(int enemyType, Vector2 pos, Vector2 target)
{
    switch (enemyType) {
        case 0:
        {
            Vector2 vel = (Vector2){ 0.0f, 300.0f };
            if (!g_pool) return;
            for (int i = 0; i < g_poolSize; i++) {
                if (!g_pool[i].active) {
                    g_pool[i].active = true;
                    g_pool[i].position = pos;
                        g_pool[i].velocity = vel;
                        g_pool[i].radius = 8.0f; // Changed from 4.0f to 8.0f
                    g_pool[i].life = 5.0f;
                    g_pool[i].age = 0.0f;
                    g_pool[i].damage = 1;
                    g_pool[i].color = RED;
                    g_pool[i].homingSpeed = 0.0f;
                    g_pool[i].willHome = false;
                    g_pool[i].visualType = 3;
                    g_pool[i].flipSprite = false;
                    g_pool[i].flipY = (vel.y > 0.0f);
                    g_pool[i].willSplit = false;
                    g_pool[i].angleDeg = 0.0f;
                    g_pool[i].spinSpeedDeg = 0.0f;
                    break;
                }
            }
            break;
        }
        case 1:
        {
            const float sideOffset = 12.0f;
            const float speed = 420.0f;
                const float radius = 16.0f; // Changed from 8.0f to 16.0f
            const int damage = 1;
            const float life = 5.0f;

            const float delay = 0.5f;
            const float sideSlideSpeed = 80.0f;

            Vector2 spawnL = pos;
            spawnL.x -= sideOffset;
            Vector2 initVelL = { -sideSlideSpeed, 0.0f };
            
            if (!g_pool) return;
            for (int i = 0; i < g_poolSize; i++) {
                if (!g_pool[i].active) {
                    g_pool[i].active = true;
                    g_pool[i].position = spawnL;
                    g_pool[i].velocity = initVelL;
                    g_pool[i].radius = radius;
                    g_pool[i].life = life;
                    g_pool[i].age = -fabsf(delay);
                    g_pool[i].damage = damage;
                    g_pool[i].color = WHITE;
                    g_pool[i].homingSpeed = speed;
                    g_pool[i].willHome = true;
                    g_pool[i].visualType = 1;
                    g_pool[i].flipSprite = true;
                    break;
                }
            }

            Vector2 spawnR = pos;
            spawnR.x += sideOffset;
            Vector2 initVelR = { sideSlideSpeed, 0.0f };
            
            for (int i = 0; i < g_poolSize; i++) {
                if (!g_pool[i].active) {
                    g_pool[i].active = true;
                    g_pool[i].position = spawnR;
                    g_pool[i].velocity = initVelR;
                    g_pool[i].radius = radius;
                    g_pool[i].life = life;
                    g_pool[i].age = -fabsf(delay);
                    g_pool[i].damage = damage;
                    g_pool[i].color = WHITE;
                    g_pool[i].homingSpeed = speed;
                    g_pool[i].willHome = true;
                    g_pool[i].visualType = 1;
                    g_pool[i].flipSprite = false;
                    break;
                }
            }

            break;
        }
        case 2:
        {
            const float radius = 20.0f; // Changed from 10.0f to 20.0f
            const int damage = 1;
            const float life = 2.0f;

            if (!g_pool) return;
            for (int i = 0; i < g_poolSize; i++) {
                if (!g_pool[i].active) {
                    g_pool[i].active = true;
                    g_pool[i].position = pos;
                    const float preSplitSpeed = 160.0f;
                    Vector2 dir = { target.x - pos.x, target.y - pos.y };
                    float len = sqrtf(dir.x*dir.x + dir.y*dir.y);
                    if (len > 0.0001f) {
                        dir.x = dir.x / len * preSplitSpeed;
                        dir.y = dir.y / len * preSplitSpeed;
                    } else {
                        dir.x = 0.0f; dir.y = preSplitSpeed;
                    }
                    g_pool[i].velocity = dir;
                        g_pool[i].radius = radius;
                    g_pool[i].life = life;
                    g_pool[i].age = 0.0f;
                    g_pool[i].damage = damage;
                    g_pool[i].color = WHITE;
                    g_pool[i].homingSpeed = 0.0f;
                    g_pool[i].willHome = false;
                    g_pool[i].visualType = 2; 
                    g_pool[i].flipSprite = false;
                    g_pool[i].willSplit = true;
                    g_pool[i].angleDeg = (float)GetRandomValue(0, 359);
                    g_pool[i].spinSpeedDeg = (float)GetRandomValue(-180, 180);
                    break;
                }
            }

            break;
        }
        default:
            break;
    }
}


void Projectiles_Update(float dt)
{
    if (!g_pool) return;
    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();
    for (int i = 0; i < g_poolSize; i++) {
        Projectile *b = &g_pool[i];
        if (!b->active) continue;

        float prevAge = b->age;
        b->age += dt;
        if (prevAge < 0.0f && b->age >= 0.0f) {
            if (b->willHome && b->homingSpeed > 0.0001f) {
                Vector2 dir = { g_playerPos.x - b->position.x, g_playerPos.y - b->position.y };
                float len = sqrtf(dir.x*dir.x + dir.y*dir.y);
                if (len > 0.0001f) {
                    dir.x = dir.x / len * b->homingSpeed;
                    dir.y = dir.y / len * b->homingSpeed;
                } else {
                    dir.x = 0; dir.y = b->homingSpeed;
                }
                b->velocity = dir;
            }
        }

        if (b->willSplit && b->age < b->life) {
            b->angleDeg += b->spinSpeedDeg * dt;
            if (b->angleDeg >= 360.0f) b->angleDeg -= 360.0f;
            if (b->angleDeg < 0.0f) b->angleDeg += 360.0f;
        }

        b->position.x += b->velocity.x * dt;
        b->position.y += b->velocity.y * dt;

        if (b->willSplit && b->age >= b->life) {
            const float childSpeed = 260.0f;
            float baseRad = b->angleDeg * DEG2RAD;
            for (int k = 0; k < 4; k++) {
                float ang = baseRad + (float)k * (PI / 2.0f);
                Vector2 v = { cosf(ang) * childSpeed, sinf(ang) * childSpeed };
                for (int j = 0; j < g_poolSize; j++) {
                    if (!g_pool[j].active) {
                        g_pool[j].active = true;
                        g_pool[j].position = b->position;
                        g_pool[j].velocity = v;
                            g_pool[j].radius = 8.0f; // Changed from 4.0f to 8.0f
                        g_pool[j].life = 5.0f;
                        g_pool[j].age = 0.0f;
                        g_pool[j].damage = 1;
                        g_pool[j].color = WHITE;
                        g_pool[j].homingSpeed = 0.0f;
                        g_pool[j].willHome = false;
                        g_pool[j].visualType = 2;
                        g_pool[j].flipSprite = false;
                        g_pool[j].willSplit = false;
                        g_pool[j].angleDeg = 0.0f;
                        g_pool[j].spinSpeedDeg = 0.0f;
                        break;
                    }
                }
            }
            b->active = false;
            continue;
        }

        if (b->age >= b->life || b->position.y > screenH + 100 || b->position.y < -100 || b->position.x < -200 || b->position.x > screenW + 200) {
            b->active = false;
        }
    }
}

void Projectiles_DrawWithSprite(Texture2D spikeSprite, Texture2D spike2Sprite, Texture2D bulletSprite)
{
    if (!g_pool) return;
    for (int i = 0; i < g_poolSize; i++) {
        Projectile *b = &g_pool[i];
        if (!b->active) continue;
        
        if (b->visualType == 0) {
            DrawCircleV(b->position, b->radius, b->color);
        } else if (b->visualType == 3) {
            if (bulletSprite.width <= 0 || bulletSprite.height <= 0) {
                DrawCircleV(b->position, b->radius, b->color);
            } else {
                float angle = atan2f(b->velocity.y, b->velocity.x) * RAD2DEG + BULLET_ANGLE_OFFSET;
                float scale = 0.75f; // metade do tamanho anterior (era 1.5)
                Rectangle source = { 0, 0, (float)bulletSprite.width, (float)bulletSprite.height };
                if (b->flipSprite) source.width = -(float)bulletSprite.width;
                if (b->flipY) source.height = -(float)bulletSprite.height;
                Rectangle dest = { b->position.x, b->position.y, fabsf(source.width) * scale, fabsf(source.height) * scale };
                Vector2 origin = { fabsf(source.width) * scale / 2.0f, fabsf(source.height) * scale / 2.0f };
                DrawTexturePro(bulletSprite, source, dest, origin, angle, b->color);
            }
        } else if (b->visualType == 1) {
            float angle = atan2f(b->velocity.y, b->velocity.x) * RAD2DEG;
            
            float scale = 2.0f;
            Rectangle source = { 0, 0, (float)spikeSprite.width, (float)spikeSprite.height };
            
            if (b->flipSprite) {
                source.width = -(float)spikeSprite.width;
            }
            
            Rectangle dest = { 
                b->position.x, 
                b->position.y, 
                fabsf(source.width) * scale, 
                spikeSprite.height * scale 
            };
            
            Vector2 origin = { 
                fabsf(source.width) * scale / 2.0f, 
                spikeSprite.height * scale / 2.0f 
            };
            
            DrawTexturePro(spikeSprite, source, dest, origin, angle, b->color);
        } else if (b->visualType == 2) {
            float angle = atan2f(b->velocity.y, b->velocity.x) * RAD2DEG;
            float scale = 2.0f;
            Rectangle source2 = { 0, 0, (float)spike2Sprite.width, (float)spike2Sprite.height };
            if (b->flipSprite) source2.width = -(float)spike2Sprite.width;
            Rectangle dest2 = { b->position.x, b->position.y, fabsf(source2.width) * scale, spike2Sprite.height * scale };
            Vector2 origin2 = { fabsf(source2.width) * scale / 2.0f, spike2Sprite.height * scale / 2.0f };
            DrawTexturePro(spike2Sprite, source2, dest2, origin2, angle, b->color);
        }
    }
}

int Projectiles_CheckPlayerCollision(Vector2 playerPos, float playerRadius) {
    if (!g_pool) return 0;
    int hits = 0;
    for (int i = 0; i < g_poolSize; i++) {
        Projectile *p = &g_pool[i];
        if (!p->active) continue;

        float dx = p->position.x - playerPos.x;
        float dy = p->position.y - playerPos.y;
        float dist2 = dx*dx + dy*dy;
        float minDist = p->radius + playerRadius;
        if (dist2 <= minDist * minDist) {
            p->active = false;
            hits++;
        }
    }
    return hits;
}

