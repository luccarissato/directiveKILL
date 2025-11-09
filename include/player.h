#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"


void Player_Init(void);

// free nos sprites que nao vao ser utilizados
void Player_Unload(void);

// desenha o player na tela
void Player_Draw(Vector2 *playerPosition);

// movimentação do player dentro da tela
void Player_HandleMovement(Vector2 *playerPosition, float playerRadius, float playerSpeed, int screenWidth, int screenHeight);

// lógica de tiro e intervalo entre rajadas
void Player_HandleShooting(float delta, Vector2 playerPosition);

// atualiza posição e o tempo de vida dos tiros
void Player_UpdateShots(float delta);

// desenha os tiros na tela
void Player_DrawShots(void);

// reduz a vida do player
void Player_TakeDamage(int amount);
int Player_GetHealth(void);

#endif // PLAYER_H
