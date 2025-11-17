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
	// homing: se >0, quando ativado o projétil irá mirar na posição atual do jogador
	// usando esta velocidade (pixels/segundo)
    float homingSpeed;
    bool willHome;
    // splitting / spinning behavior
    bool willSplit; // se true, quando age >= life, explode em múltiplos projeteis
    float angleDeg; // current orientation for spinning (degrees)
    float spinSpeedDeg; // degrees per second
    int visualType; // 0 = circle, 1 = spike
    bool flipSprite; // para spike da esquerda (invertido)
} Projectile;

// inicializa os projeteis com uma quantidade fixa deles
void Projectiles_Init(int maxProjectiles);

// spawna os projeteis com parametros específicos
// spawna os projeteis com parametros especificos
void Projectiles_Spawn(Vector2 pos, Vector2 vel, float radius, int damage, Color color, float lifeSec, float delaySec, float homingSpeed);

// define a posição atual do jogador (usada quando projeteis homing ativam)
void Projectiles_SetPlayerPosition(Vector2 pos);

// spawna os projeteis baseado no tipo de inimigo (padrões de ataque distintos)
// target: posição para mirar (ex.: posição do jogador). Se não for usada pelo padrão, passe (0,0).
void Projectiles_Type(int enemyType, Vector2 pos, Vector2 target);

// update e atualiza os projeteis
void Projectiles_Update(float dt);
void Projectiles_Draw(void);
void Projectiles_DrawWithSprite(Texture2D spikeSprite);

// free na pool de projeteis
void Projectiles_Free(void);

// checa colisão dos tiros inimigos com o player
int Projectiles_CheckPlayerCollision(Vector2 playerPos, float playerRadius);

#endif // PROJECTILE_H