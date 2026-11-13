#include "algoritmos/algoritmo.h"
#include "algoritmos/lru.h"
#include "algoritmos/random.h"
#include "algoritmos/clock.h"

Algoritmo escolherAlgoritmo(const char *nome) {
    if (strcmp(nome, "lru") == 0) return criarLRU();
    if (strcmp(nome, "clock") == 0)  return criarClock();
    if (strcmp(nome, "random") == 0) return criarRandom();
}