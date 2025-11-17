// funções de projeteis
#include "../include/projectile.h"
#include "raylib.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

static Projectile *g_pool = NULL;
static int g_poolSize = 0;
static Vector2 g_playerPos = {0,0};

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
            Projectiles_Spawn(pos, vel, 4.0f, 1, RED, 5.0f, 0.0f, 0.0f);
            break;
        }
        case 1:
        {
            const float sideOffset = 12.0f;
            const float speed = 380.0f;
            const float radius = 8.0f;
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

        if (b->age >= 0.0f) {
            b->position.x += b->velocity.x * dt;
            b->position.y += b->velocity.y * dt;
        } else {
            b->position.x += b->velocity.x * dt;
            b->position.y += b->velocity.y * dt;
        }

        if (b->age >= b->life || b->position.y > screenH + 100 || b->position.y < -100 || b->position.x < -200 || b->position.x > screenW + 200) {
            b->active = false;
        }
    }
}

void Projectiles_Draw(void)
{
    if (!g_pool) return;
    for (int i = 0; i < g_poolSize; i++) {
        Projectile *b = &g_pool[i];
        if (!b->active) continue;
        DrawCircleV(b->position, b->radius, b->color);
    }
}

void Projectiles_DrawWithSprite(Texture2D spikeSprite)
{
    if (!g_pool) return;
    for (int i = 0; i < g_poolSize; i++) {
        Projectile *b = &g_pool[i];
        if (!b->active) continue;
        
        if (b->visualType == 0) {
            DrawCircleV(b->position, b->radius, b->color);
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

