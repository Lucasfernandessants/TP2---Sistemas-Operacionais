#ifndef SIMULADOR_H
#define SIMULADOR_H

#include <stdio.h>
#include "memoria.h"
#include "algoritmos/algoritmo.h"

typedef struct {
    char *algoritmo;
    char *arquivo;
    unsigned int tamanho_pagina;
    unsigned int tamanho_memoria;
} Configuracao;

typedef struct {
    unsigned long long acessos;
    unsigned long long faults;    
    unsigned long long escritas;  
} Estatisticas;

unsigned int calcularDeslocamento(unsigned int tamanho_pagina);

unsigned int determinarPagina(unsigned int endereco, unsigned int s);

void executarSimulacao(FILE* arquivo, Quadro* memoria, Configuracao configuracao, unsigned int qtd_quadros, Algoritmo algoritmo);
#endif