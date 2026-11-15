#ifndef TABELA_DENSA_H
#define TABELA_DENSA_H

#include <stddef.h>
typedef struct {
    int *paginas;
    unsigned int quantidade_paginas;

    size_t memoria_ocupada;
} TabelaDensa;

TabelaDensa* inicializarTabelaDensa(unsigned int tamanho_pagina);

int consultarPagina(TabelaDensa* tabela_densa, unsigned int pagina_virtual);

void inserirPagina(TabelaDensa* tabela_densa, unsigned int pagina_virtual, int quadro_fisico);

void removerPagina(TabelaDensa* tabela_densa, unsigned int pagina_virtual);

void liberarTabelaDensa(TabelaDensa* tabela_densa);

#endif