#ifndef ENEMY_H
#define ENEMY_H

#include "raylib.h"
#include <stdbool.h>

// inicializa os inimigos; pass a Y coordinate where enemies should stop (from top)
void Enemies_Init(float stopY);

// atualiza a posição dos inimigos
void Enemies_Update(void);

// desenha os inimigos na tela
void Enemies_Draw(Texture2D enemySprite);

// obtém a posição do primeiro inimigo ativo, retorna true se for bem-sucedido
bool Enemies_GetFirstActivePosition(Vector2 *outPos, int *outIndex);

// checa se o inimigo foi atingido e o desaparece casoe ele morra
bool Enemies_CheckHit(Vector2 pos, float radius);

#endif // ENEMY_H