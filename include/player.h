#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"

// inicializa o jogador
void Player_Init(void);

// free nos sprites que nao vao ser utilizados
void Player_Unload(void);

// desenha o player na tela
void Player_Draw(Vector2 *playerPosition);

// movimentação do player dentro da área de jogo (passar limites left/right/top/bottom)
// adiciona o parâmetro `delta` para atualizar timers de animação
void Player_HandleMovement(float delta, Vector2 *playerPosition, float playerRadius, float playerSpeed, float leftBound, float rightBound, float topBound, float bottomBound);

// lógica de tiro e intervalo entre rajadas
void Player_HandleShooting(float delta, Vector2 playerPosition);

// atualiza posição e o tempo de vida dos tiros
void Player_UpdateShots(float delta);

// desenha os tiros na tela
void Player_DrawShots(void);

// reduz a vida do player
void Player_TakeDamage(int amount);

// temporário mostra a vida do player
int Player_GetHealth(void);

// reseta o estado interno do player (tiros, timers, vida) sem recarregar sprites
void Player_Reset(void);

#endif // PLAYER_H
