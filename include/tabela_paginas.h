#ifndef TABELA_PAGINAS_H
#define TABELA_PAGINAS_H

#include <stddef.h>
typedef struct {
    void* tabela_atual;
    void* (*inicializar)(unsigned int tamanho_pagina, unsigned int qtd_quadros);
    int (*consultar)(void *pagina, unsigned int pagina_virtual);
    void (*inserir)(void *pagina, unsigned int pagina_virtual, int quadro_fisico);
    void (*remover)(void *pagina, unsigned int pagina_virtual);
    void (*liberar)(void *pagina);
    size_t (*obterMemoria)(void *pagina);

} TabelaPaginas;

TabelaPaginas escolherTabela(const char* nome);

#endif
