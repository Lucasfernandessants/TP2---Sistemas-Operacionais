#ifndef RANDOM_H
#define RANDOM_H

#include "algoritmo.h"

Algoritmo criarRandom();

void randomInicializar(unsigned int qtd_quadros);

void randomAcessarPagina(Quadro *memoria, int quadro, unsigned long long tempo);

void randomCarregarPagina(Quadro *memoria, int quadro, unsigned long long tempo);

int randomSelecionarVitima(Quadro *memoria, unsigned int qtd_quadros);

void randomLiberar(void);

#endif