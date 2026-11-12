#include "memoria.h"
#include <stdlib.h>
#include <stdio.h>

unsigned int calcularQuantidadeQuadros(unsigned int memoria, unsigned int paginas){
    return memoria/paginas;
}

int encontrarQuadroLivre(Quadro *memoria, unsigned int quantidade_quadros){
    for(unsigned int i=0;i<quantidade_quadros;i++){
        if(memoria[i].ocupado == 0)
            return i;
    }

    return -1;
}

Quadro* inicializarMemoria(unsigned int quantidade_quadros){
    Quadro *memoria = malloc(quantidade_quadros * sizeof(Quadro));

    if(memoria == NULL){
        printf("Falha ao alocar memória");
        return NULL;
    }

    for(unsigned int i=0;i<quantidade_quadros;i++){
        memoria[i].ocupado = 0;
        memoria[i].sujo = 0;
    }

    return memoria;
}

void liberarMemoria(Quadro *memoria){
    free(memoria);
}