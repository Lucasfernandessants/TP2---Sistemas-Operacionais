#ifndef TABELA_DENSA_H
#define TABELA_DENSA_H

typedef struct {
    int *quadros;
    unsigned int quantidade_paginas;
} TabelaDensa;

TabelaDensa* inicializarTabelaDensa(unsigned int tamanho_pagina);

int consultarPagina(TabelaDensa* tabela_densa, unsigned int pagina_virtual);

void inserirPagina(TabelaDensa* tabela_densa, unsigned int pagina_virtual, int quadro_fisico);

void removerPagina(TabelaDensa* tabela_densa, unsigned int pagina_virtual);

void liberarTabelaDensa(TabelaDensa* tabela_densa);

#endif