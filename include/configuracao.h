#ifndef CONFIGURACAO_H
#define CONFIGURACAO_H

typedef struct {
    char *algoritmo;
    char *arquivo;
    unsigned int tamanho_pagina;
    unsigned int tamanho_memoria;
    int debug;
} Configuracao;

#endif