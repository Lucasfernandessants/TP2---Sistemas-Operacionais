#ifndef OPTIMAL_H
#define OPTIMAL_H

#include "algoritmo.h"

Algoritmo criarOpt();

void optInicializar(unsigned int qtd_quadros, Configuracao configuracao);

void optAcessarPagina(Quadro *memoria, int quadro, unsigned long long tempo);

void optCarregarPagina(Quadro *memoria, int quadro, unsigned long long tempo);

int optSelecionarVitima(Quadro *memoria, unsigned int qtd_quadros);

void optLiberar(void);

#endif