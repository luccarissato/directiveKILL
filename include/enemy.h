#ifndef ENEMY_H
#define ENEMY_H

#include "raylib.h"
#include <stdbool.h>

// inicializa os inimigos; pass a Y coordinate where enemies should stop (from top)
void Enemies_Init(float stopY);

// Define a área horizontal (left,right) onde inimigos devem nascer e permanecer.
void Enemies_SetPlayArea(float leftX, float rightX);

// atualiza a posição de parada dos inimigos (quando janela é redimensionada)
void Enemies_UpdateStopY(float newStopY);

// atualiza a posição dos inimigos
void Enemies_Update(void);

// desenha os inimigos na tela
// enemySprite: broken_ship, scoutSprite: scout, soldierSprite: soldier (used for type 2)
void Enemies_Draw(Texture2D enemySprite, Texture2D scoutSprite, Texture2D soldierSprite);

// obtém a posição do primeiro inimigo ativo, retorna true se for bem-sucedido
bool Enemies_GetFirstActivePosition(Vector2 *outPos, int *outIndex);

// todos os inimigos atiram com o seu respectivo padrão
// playerPos: posição atual do jogador para padrões que miram no jogador
void Enemies_ShootAll(Vector2 playerPos);

// temporário só pra mostrar qual wave é
int Enemies_GetCurrentWave(void);

// checa se o inimigo foi atingido e o desaparece casoe ele morra
bool Enemies_CheckHit(Vector2 pos, float radius);

#endif // ENEMY_H