// funções de projeteis
#include "../include/projectile.h"
#include "raylib.h"
#include <stdlib.h>
#include <string.h>

static Projectile *g_pool = NULL;
static int g_poolSize = 0;

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

void Projectiles_Spawn(Vector2 pos, Vector2 vel, float radius, int damage, Color color, float lifeSec)
{
    if (!g_pool) return;
    for (int i = 0; i < g_poolSize; i++) {
        if (!g_pool[i].active) {
            g_pool[i].active = true;
            g_pool[i].position = pos;
            g_pool[i].velocity = vel;
            g_pool[i].radius = radius;
            g_pool[i].life = lifeSec;
            g_pool[i].age = 0.0f;
            g_pool[i].damage = damage;
            g_pool[i].color = color;
            break;
        }
    }
}

void Projectiles_Type(int enemyType, Vector2 pos)
{
    switch (enemyType) {
        case 0: 
        {
            Vector2 vel = (Vector2){ 0.0f, 300.0f };
            Projectiles_Spawn(pos, vel, 4.0f, 1, RED, 5.0f);
            break;
        }
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

        b->age += dt;
        b->position.x += b->velocity.x * dt;
        b->position.y += b->velocity.y * dt;

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

