#include "tabela_paginas.h"
#include "tabela_densa.h"
#include "tabela_hierarquica_2_niveis.h"
#include "tabela_hierarquica_3_niveis.h"
#include "tabela_invertida.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


static void* initDensa(unsigned int tam, unsigned int qtd_q) { 
    (void)qtd_q;
    return inicializarTabelaDensa(tam);}
static int consDensa(void* t, unsigned int p) { return consultarPagina((TabelaDensa*) t, p); }
static void insDensa(void* t, unsigned int p, int q) { return inserirPagina((TabelaDensa*) t, p, q); }
static void remDensa(void* t, unsigned int p) { removerPagina((TabelaDensa*) t, p); }
static void libDensa(void* t) { liberarTabelaDensa((TabelaDensa*) t); }


static void* initTH2(unsigned int tam, unsigned int qtd_q) { 
    (void)qtd_q;
    return inicializarTH2(tam);}
static int consTH2(void* t, unsigned int p) { return consultarPaginaTH2((TabelaHierarquica2*) t, p); }
static void insTH2(void* t, unsigned int p, int q) { return inserirPaginaTH2((TabelaHierarquica2*) t, p, q); }
static void remTH2(void* t, unsigned int p) { removerPaginaTH2((TabelaHierarquica2*) t, p); }
static void libTH2(void* t) { liberarTH2((TabelaHierarquica2*) t); }


static void* initTH3(unsigned int tam, unsigned int qtd_q) { 
    (void)qtd_q;
    return inicializarTH3(tam);}
static int consTH3(void* t, unsigned int p) { return consultarPaginaTH3((TabelaHierarquica3*) t, p); }
static void insTH3(void* t, unsigned int p, int q) { return inserirPaginaTH3((TabelaHierarquica3*) t, p, q); }
static void remTH3(void* t, unsigned int p) { removerPaginaTH3((TabelaHierarquica3*) t, p); }
static void libTH3(void* t) { liberarTH3((TabelaHierarquica3*) t); }


static void* initTI(unsigned int tam, unsigned int qtd_q) { 
    (void)tam;
    return inicializarTI(qtd_q);
}
static int consTI(void* t, unsigned int p) { return consultarPaginaTI((TabelaInvertida*) t, p); }
static void insTI(void* t, unsigned int p, int q) { return inserirPaginaTI((TabelaInvertida*) t, p, q); }
static void remTI(void* t, unsigned int p) { removerPaginaTI((TabelaInvertida*) t, p); }
static void libTI(void* t) { liberarTI((TabelaInvertida*) t); }


TabelaPaginas escolherTabela(const char* nome) {
    TabelaPaginas tabela;
    tabela.tabela_atual = NULL;

    if (strcmp(nome, "densa") == 0) {
        tabela.inicializar = initDensa;
        tabela.consultar = consDensa;
        tabela.inserir = insDensa;
        tabela.remover = remDensa;
        tabela.liberar = libDensa;
        return tabela;
    }

    if (strcmp(nome, "th2") == 0) {
        tabela.inicializar = initTH2;
        tabela.consultar = consTH2;
        tabela.inserir = insTH2;
        tabela.remover = remTH2;
        tabela.liberar = libTH2;
        return tabela;
    }

    if (strcmp(nome, "th3") == 0) {
        tabela.inicializar = initTH3;
        tabela.consultar = consTH3;
        tabela.inserir = insTH3;
        tabela.remover = remTH3;
        tabela.liberar = libTH3;
        return tabela;
    }
    
    if (strcmp(nome, "invertida") == 0) {
        tabela.inicializar = initTI;
        tabela.consultar = consTI;
        tabela.inserir = insTI;
        tabela.remover = remTI;
        tabela.liberar = libTI;
        return tabela;
    }

    printf("Tipo de tabela não existente! Tipos existentes: [densa | th2 | th3 | invertida]");
    exit(1);
}