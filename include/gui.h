// API pública da GUI
#ifndef GUI_H
#define GUI_H

#include "raylib.h"
#include <stdbool.h>

typedef enum GuiState {
    GUI_STATE_MENU,
    GUI_STATE_GAME,
    GUI_STATE_PAUSE,
    GUI_STATE_GAMEOVER,
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

#endif // GUI_H