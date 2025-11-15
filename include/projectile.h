#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "raylib.h"
#include <stdbool.h>

typedef struct Projectile {
	Vector2 position;
	Vector2 velocity; // pixels por segundo
	float radius;
	float life; // segundos
	float age;
	int damage;
	bool active;
	Color color;
    // homing: if >0, when activation occurs the projectile will re-aim towards
    // the player's current position with this speed (pixels/sec)
    float homingSpeed;
    bool willHome;
} Projectile;

// inicializa os projeteis com uma quantidade fixa deles
void Projectiles_Init(int maxProjectiles);

// spawna os projeteis com parametros específicos
// spawna os projeteis com parametros especificos
void Projectiles_Spawn(Vector2 pos, Vector2 vel, float radius, int damage, Color color, float lifeSec, float delaySec, float homingSpeed);

// set the player's current position (used when homing projectiles activate)
void Projectiles_SetPlayerPosition(Vector2 pos);

// spawna os projeteis baseado no tipo de inimigo (padrões de ataque distintos)
// target: position to aim at (e.g. player position). If not used by the pattern, pass (0,0).
void Projectiles_Type(int enemyType, Vector2 pos, Vector2 target);

// update e atualiza os projeteis
void Projectiles_Update(float dt);
void Projectiles_Draw(void);

// free na pool de projeteis
void Projectiles_Free(void);

// checa colisão dos tiros inimigos com o player
int Projectiles_CheckPlayerCollision(Vector2 playerPos, float playerRadius);

#endif // PROJECTILE_H