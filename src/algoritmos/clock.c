#include "algoritmos/clock.h"
#include <stdlib.h>
#include <stdio.h>

Algoritmo criarClock(){
    printf("Algoritmo escolhido: clock!\n");
    Algoritmo algoritmo;
    algoritmo.estado = NULL;
    algoritmo.inicializar = clockInicializar;
    algoritmo.acessarPagina = clockAcessarPagina;
    algoritmo.carregarPagina = clockCarregarPagina;
    algoritmo.selecionarVitima = clockSelecionarVitima;
    algoritmo.liberar = clockLiberar;
    return algoritmo;
}

static int ponto;

void clockInicializar(unsigned int qtd_quadros){
    ponto = 0;
}

void clockAcessarPagina(Quadro *memoria, int quadro, unsigned long long tempo){
    memoria[quadro].bit_ref = 1;
}

void clockCarregarPagina(Quadro *memoria, int quadro, unsigned long long tempo){
    memoria[quadro].bit_ref = 1;
}

int clockSelecionarVitima(Quadro *memoria, unsigned int qtd_quadros){
    unsigned int i = ponto;
    while(1){
        if(memoria[i].bit_ref == 0){
            ponto = (i+1 == qtd_quadros) ? 0 : i+1;
            return i;
        }
        memoria[i].bit_ref = 0;
        i++;
        if (i == qtd_quadros)
            i = 0;
    }
}

void clockLiberar(void){
}