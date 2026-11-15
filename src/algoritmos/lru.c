#include "algoritmos/lru.h"
#include <stdlib.h>
#include <stdio.h>

Algoritmo criarLRU(){
    Algoritmo algoritmo;
    algoritmo.estado = NULL;
    algoritmo.inicializar = lruInicializar;
    algoritmo.acessarPagina = lruAcessarPagina;
    algoritmo.carregarPagina = lruCarregarPagina;
    algoritmo.selecionarVitima = lruSelecionarVitima;
    algoritmo.liberar = lruLiberar;
    return algoritmo;
}

void lruInicializar(unsigned int qtd_quadros, Configuracao configuracao){
}

void lruAcessarPagina(Quadro *memoria, int quadro, unsigned long long tempo){
    memoria[quadro].ultimo_acesso = tempo; // Atualiza o instante do acesso mais recente à página.
}

void lruCarregarPagina(Quadro *memoria, int quadro, unsigned long long tempo){
    memoria[quadro].ultimo_acesso = tempo;
}

int lruSelecionarVitima(Quadro *memoria, unsigned int qtd_quadros){
    int vitima = 0;
    // Seleciona o quadro cuja página foi acessada há mais tempo.
    for(unsigned int i = 1; i<qtd_quadros;i++){
        if(memoria[i].ultimo_acesso < memoria[vitima].ultimo_acesso)
            vitima = i;
    }
    return vitima;
}

void lruLiberar(void){
}