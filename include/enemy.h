#ifndef ENEMY_H
#define ENEMY_H

#include "raylib.h"

// inicializa os inimigos
void Enemies_Init(void);

// atualiza a posição dos inimigos
void Enemies_Update(void);

// desenha os inimigos na tela
void Enemies_Draw(Texture2D enemySprite);


#endif // ENEMY_H