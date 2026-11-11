#include<stdio.h>
#include <stdlib.h>
#include "simulador.h"
#include "memoria.h"
#include "arquivo.h"

int main(int argc, char *argv[]){
    if(argc < 5){
        printf("Poucos argumentos fornecidos!\n");
        return 1;
    }
    
    Configuracao configuracao;
    configuracao.algoritmo = argv[1];
    configuracao.arquivo = argv[2];
    configuracao.tamanho_pagina = atoi(argv[3]);

    if(configuracao.tamanho_pagina > 64 || configuracao.tamanho_pagina < 2)
    {
        printf("Tamanho da página fora dos limites permitidos!\n");
        return 1;
    }

    configuracao.tamanho_memoria = atoi(argv[4]);
    
    if (configuracao.tamanho_memoria > 16384 || configuracao.tamanho_memoria < 128)
    {
        printf("Tamanho da memória fora dos limites permitidos!\n");
        return 1;
    }

    FILE *arquivo = fopen(configuracao.arquivo, "r");
    if(arquivo == NULL){
        printf("Falha ao ler o arquivo!");
        return 1;
    }

    unsigned int qtd_quadros = calcularQuantidadeQuadros(configuracao.tamanho_memoria, configuracao.tamanho_pagina);
    
    Quadro* memoria = inicializarMemoria(qtd_quadros);
    if(memoria == NULL){
        printf("Falha ao alocar memória!");
        return 1;
    }

    executarSimulacao(arquivo, memoria, configuracao, qtd_quadros);
    
    liberarMemoria(memoria);
    fclose(arquivo);

    return 0;
}