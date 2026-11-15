#include "algoritmos/algoritmo.h"
#include "algoritmos/lru.h"
#include "algoritmos/random.h"
#include "algoritmos/clock.h"
#include "algoritmos/optimal.h"
#include <stdio.h>
#include <stdlib.h>

// Identifica o algoritmo solicitado pelo nome informado na execução.
Algoritmo escolherAlgoritmo(const char *nome) {
    if (strcmp(nome, "lru") == 0) return criarLRU();
    if (strcmp(nome, "clock") == 0)  return criarClock();
    if (strcmp(nome, "random") == 0) return criarRandom();
    if (strcmp(nome, "opt") == 0) return criarOpt();

    fprintf(stderr, "Algoritmo desconhecido: %s (use lru, clock, random ou opt)\n", nome);
    exit(1);
}