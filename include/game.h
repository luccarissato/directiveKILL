// simple game score API
#ifndef GAME_H
#define GAME_H

#include "raylib.h"

// inicializa o sistema de score
void Game_Init(void);

// atualização por frame do score (lida com o decay de 1 por segundo)
void Game_Update(float dt);

// penalidades e incrementos de score
void Game_AddScore(int delta);

// score atual para testes
int Game_GetScore(void);

// salva um score com um nome de 4 caracteres
void Game_SaveScore(const char *name, int score);

// mostra os scores salvos na tela (usado por GUI_STATE_SCORES)
void Game_DrawScores(void);

#endif // GAME_H
