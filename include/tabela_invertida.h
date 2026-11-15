#ifndef TABELA_INVERTIDA_H
#define TABELA_INVERTIDA_H

typedef struct {
    int *paginas;
    unsigned int quantidade_quadros;
} TabelaInvertida;

TabelaInvertida* inicializarTI(unsigned int tamanho_pagina);

int consultarPaginaTI(TabelaInvertida* tabela, unsigned int pagina_virtual);

void inserirPaginaTI(TabelaInvertida* tabela, unsigned int pagina_virtual, int quadro_fisico);

void removerPaginaTI(TabelaInvertida* tabela, unsigned int pagina_virtual);

void liberarTI(TabelaInvertida* tabela);


#endif