#ifndef CLOCK_H
#define CLOCK_H

#include "algoritmo.h"

Algoritmo criarClock();

void clockInicializar(unsigned int qtd_quadros);

void clockAcessarPagina(Quadro *memoria, int quadro, unsigned long long tempo);

void clockCarregarPagina(Quadro *memoria, int quadro, unsigned long long tempo);

int clockSelecionarVitima(Quadro *memoria, unsigned int qtd_quadros);

void clockLiberar(void);

#endif