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
    float homingSpeed;
    bool willHome;
    bool willSplit; 
    float angleDeg; 
    float spinSpeedDeg;
	int ownerType;
    int visualType; 
    bool flipSprite; 
} Projectile;

// inicializa os projeteis com uma quantidade fixa deles
void Projectiles_Init(int maxProjectiles);

// spawna os projeteis com parametros especificos
void Projectiles_Spawn(Vector2 pos, Vector2 vel, float radius, int damage, Color color, float lifeSec, float delaySec, float homingSpeed, int ownerType);

// define a posição atual do jogador (usada quando projeteis homing ativam)
void Projectiles_SetPlayerPosition(Vector2 pos);

// spawna os projeteis baseado no tipo de inimigo (padrões de ataque distintos)
void Projectiles_Type(int enemyType, Vector2 pos, Vector2 target);

// update e atualiza os projeteis
void Projectiles_Update(float dt);
void Projectiles_Draw(void);
void Projectiles_DrawWithSprite(Texture2D spikeSprite, Texture2D spikeSprite2);

// free na pool de projeteis
void Projectiles_Free(void);

// checa colisão dos tiros inimigos com o player
int Projectiles_CheckPlayerCollision(Vector2 playerPos, float playerRadius);

#endif // PROJECTILE_H