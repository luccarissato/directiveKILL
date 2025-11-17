// sistema de score
#include "../include/game.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

static int g_score = 0;
static float g_accum = 0.0f; 

void Game_Init(void) {
    g_score = 0;
    g_accum = 0.0f;
}

void Game_Update(float dt) {
    // Score no longer decays over time. Keep accumulator in case of future use.
    (void)dt;
}

void Game_AddScore(int delta) {
    g_score += delta;
    if (g_score < 0) g_score = 0;
}

int Game_GetScore(void) {
    return g_score;
}

typedef struct ScoreNode {
    char name[5];
    int score;
    struct ScoreNode *next;
} ScoreNode;

// unsorted
static ScoreNode* load_scores_from_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return NULL;
    ScoreNode *head = NULL;
    char name[256];
    int sc;
    while (fscanf(f, "%255s %d", name, &sc) == 2) {
        ScoreNode *n = (ScoreNode*)malloc(sizeof(ScoreNode));
        memset(n, 0, sizeof(*n));
        strncpy(n->name, name, 4);
        n->name[4] = '\0';
        n->score = sc;
        n->next = head;
        head = n;
    }
    fclose(f);
    return head;
}

static void free_scores(ScoreNode *head) {
    while (head) {
        ScoreNode *t = head->next;
        free(head);
        head = t;
    }
}

// sobrescreve
static void write_scores_to_file(const char *path, ScoreNode *head) {
    FILE *f = fopen(path, "w");
    if (!f) return;
    for (ScoreNode *n = head; n; n = n->next) {
        fprintf(f, "%s %d\n", n->name, n->score);
    }
    fclose(f);
}

static ScoreNode* insert_sorted(ScoreNode *head, ScoreNode *node) {
    if (!head || node->score >= head->score) {
        node->next = head;
        return node;
    }
    ScoreNode *cur = head;
    while (cur->next && cur->next->score > node->score) cur = cur->next;
    node->next = cur->next;
    cur->next = node;
    return head;
}

void Game_SaveScore(const char *name, int score) {
    const char *path = "score.txt";
    ScoreNode *head = load_scores_from_file(path);
    ScoreNode *n = (ScoreNode*)malloc(sizeof(ScoreNode));
    memset(n, 0, sizeof(*n));
    strncpy(n->name, name ? name : "----", 4);
    n->name[4] = '\0';
    n->score = score;
    n->next = NULL;
    head = insert_sorted(head, n);
    write_scores_to_file(path, head);
    free_scores(head);
}

void Game_DrawScores(void) {
    // Mantemos a função antiga chamando a nova com posição padrão
    Game_DrawScoresAt(120);
}

void Game_DrawScoresAt(int startY) {
    const char *path = "score.txt";
    ScoreNode *head = load_scores_from_file(path);
    int count = 0;
    for (ScoreNode *t = head; t; t = t->next) count++;
    if (count == 0) {
        DrawText("No scores saved.", 100, startY, 20, RAYWHITE);
        free_scores(head);
        return;
    }
    ScoreNode **arr = (ScoreNode**)malloc(sizeof(ScoreNode*) * count);
    int i = 0;
    for (ScoreNode *t = head; t; t = t->next) arr[i++] = t;

    // bubble sort descendente
    for (int a = 0; a < count - 1; a++) {
        for (int b = 0; b < count - 1 - a; b++) {
            if (arr[b]->score < arr[b+1]->score) {
                ScoreNode *tmp = arr[b]; arr[b] = arr[b+1]; arr[b+1] = tmp;
            }
        }
    }

    int y = startY;
    for (int k = 0; k < count; k++) {
        char buf[64];
        snprintf(buf, sizeof(buf), "%d. %s - %d", k+1, arr[k]->name, arr[k]->score);
        DrawText(buf, 100, y, 20, RAYWHITE);
        y += 28;
    }
    free(arr);
    free_scores(head);
}

void Game_DrawScoresAtScaled(int startY, int fontSize, int lineSpacing) {
    const char *path = "score.txt";
    ScoreNode *head = load_scores_from_file(path);
    int count = 0;
    for (ScoreNode *t = head; t; t = t->next) count++;
    
    int sw = GetScreenWidth();
    int margin = (int)(sw * 0.125f); // 12.5% da largura como margem
    if (margin < 10) margin = 10;
    
    if (count == 0) {
        DrawText("No scores saved.", margin, startY, fontSize, RAYWHITE);
        free_scores(head);
        return;
    }
    ScoreNode **arr = (ScoreNode**)malloc(sizeof(ScoreNode*) * count);
    int i = 0;
    for (ScoreNode *t = head; t; t = t->next) arr[i++] = t;

    // bubble sort descendente
    for (int a = 0; a < count - 1; a++) {
        for (int b = 0; b < count - 1 - a; b++) {
            if (arr[b]->score < arr[b+1]->score) {
                ScoreNode *tmp = arr[b]; arr[b] = arr[b+1]; arr[b+1] = tmp;
            }
        }
    }

    int y = startY;
    for (int k = 0; k < count; k++) {
        char buf[64];
        snprintf(buf, sizeof(buf), "%d. %s - %d", k+1, arr[k]->name, arr[k]->score);
        DrawText(buf, margin, y, fontSize, RAYWHITE);
        y += lineSpacing;
    }
    free(arr);
    free_scores(head);
}
