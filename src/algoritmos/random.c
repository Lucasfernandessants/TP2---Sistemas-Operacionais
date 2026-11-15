#include "algoritmos/random.h"
#include <stdlib.h>
#include <stdio.h>

Algoritmo criarRandom(){
    Algoritmo algoritmo;
    algoritmo.estado = NULL;
    algoritmo.inicializar = randomInicializar;
    algoritmo.acessarPagina = randomAcessarPagina;
    algoritmo.carregarPagina = randomCarregarPagina;
    algoritmo.selecionarVitima = randomSelecionarVitima;
    algoritmo.liberar = randomLiberar;
    return algoritmo;
}

void randomInicializar(unsigned int qtd_quadros, Configuracao configuracao){
    srandom(42); // Define uma semente fixa para tornar os resultados reproduzíveis.
}

void randomAcessarPagina(Quadro *memoria, int quadro, unsigned long long tempo){
}

void randomCarregarPagina(Quadro *memoria, int quadro, unsigned long long tempo){
}

int randomSelecionarVitima(Quadro *memoria, unsigned int qtd_quadros){
    return random() % qtd_quadros; // Escolhe aleatoriamente um quadro para substituição.
}

void randomLiberar(void){
}