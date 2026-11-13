#include "algoritmos/random.h"
#include <stdlib.h>
#include <stdio.h>

Algoritmo criarRandom(){
    printf("Algoritmo escolhido: random!\n");
    Algoritmo algoritmo;
    algoritmo.estado = NULL;
    algoritmo.inicializar = randomInicializar;
    algoritmo.acessarPagina = randomAcessarPagina;
    algoritmo.carregarPagina = randomCarregarPagina;
    algoritmo.selecionarVitima = randomSelecionarVitima;
    algoritmo.liberar = randomLiberar;
    return algoritmo;
}

void randomInicializar(unsigned int qtd_quadros){
    srandom(42);
}

void randomAcessarPagina(Quadro *memoria, int quadro, unsigned long long tempo){
}

void randomCarregarPagina(Quadro *memoria, int quadro, unsigned long long tempo){
}

int randomSelecionarVitima(Quadro *memoria, unsigned int qtd_quadros){
    return random() % qtd_quadros;
}

void randomLiberar(void){
}