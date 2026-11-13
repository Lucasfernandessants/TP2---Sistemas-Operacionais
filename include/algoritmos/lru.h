#ifndef LRU_H
#define LRU_H

#include "algoritmo.h"

Algoritmo criarLRU();

void lruInicializar(unsigned int qtd_quadros);

void lruAcessarPagina(Quadro *memoria, int quadro, unsigned long long tempo);

void lruCarregarPagina(Quadro *memoria, int quadro, unsigned long long tempo);

int lruSelecionarVitima(Quadro *memoria, unsigned int qtd_quadros);

void lruLiberar(void);

#endif