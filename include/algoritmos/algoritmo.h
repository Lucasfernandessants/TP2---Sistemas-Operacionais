#ifndef ALGORITMO_H
#define ALGORITMO_H

#include "memoria.h"
#include <string.h>

typedef struct {
    void* estado;
    void (*inicializar)(unsigned int qtd_quadros);
    void (*acessarPagina)(Quadro *memoria, int quadro, unsigned long long tempo);
    void (*carregarPagina)(Quadro *memoria, int quadro, unsigned long long tempo);
    int (*selecionarVitima)(Quadro *memoria, unsigned int qtd_quadros);
    void (*liberar)(void);
} Algoritmo;

Algoritmo escolherAlgoritmo(const char *nome);

#endif