// API pública da GUI
#ifndef GUI_H
#define GUI_H

#include "raylib.h"
#include <stdbool.h>

typedef enum GuiState {
    GUI_STATE_MENU,
    GUI_STATE_GAME,
    GUI_STATE_GAMEOVER,
    GUI_STATE_SAVE_SCORE, 
    GUI_STATE_SCORES,     
    GUI_STATE_EXIT
} GuiState;

// Inicializa a GUI (opcional)
void GUI_Init(void);

// Atualiza a máquina de estados da GUI; retorna o novo estado
GuiState Gui_Update(GuiState currentState);

// Desenha a GUI para o estado atual; mostra vidas quando aplicável
void Gui_Draw(GuiState state, int playerLives);

// Libera recursos da GUI (opcional)
void Gui_Unload(void);

// Retorna escala baseada no tamanho atual da janela (base 800x450)
float GUI_GetScale(void);

// Retorna tamanho de fonte escalado
int GUI_GetScaledFontSize(int baseSize);

// Calcula a área de jogo interna (dentro das barras do GUI overlay).
// Preenche o Rectangle com a área utilizável para jogo (x,y,width,height).
void GUI_GetPlayArea(Rectangle *outArea);

// Retorna o dest rectangle onde o overlay GUI é desenhado (útil para alinhar elementos)
void GUI_GetOverlayDest(Rectangle *outDest);

// Desenha apenas o background da GUI (por trás dos objetos de jogo)
void GUI_DrawBackground(void);

// Desenha o overlay da GUI (barras, vidas, score) por cima dos objetos de jogo
void GUI_DrawOverlay(int playerLives);

#endif // GUI_H