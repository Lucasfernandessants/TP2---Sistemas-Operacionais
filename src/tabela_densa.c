#include "tabela_densa.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

TabelaDensa* inicializarTabelaDensa(unsigned int tamanho_pagina){
    TabelaDensa* tabela_densa = malloc(sizeof(TabelaDensa));

    if(tabela_densa == NULL){
        printf("Falha ao alocar tabela!");
        return NULL;
    }

    tabela_densa->quantidade_paginas = pow(2,32)/(tamanho_pagina*1024);
    tabela_densa->quadros = malloc(tabela_densa->quantidade_paginas * sizeof(int));

    if(tabela_densa->quadros == NULL){
        printf("Falha ao alocar quadros!");
        free(tabela_densa);
        return NULL;
    }

    for(unsigned int i = 0; i < tabela_densa->quantidade_paginas; i++){
        tabela_densa->quadros[i] = -1;
    }

    return tabela_densa;
}

int consultarPagina(TabelaDensa* tabela_densa, unsigned int pagina_virtual){
    if(pagina_virtual >= tabela_densa->quantidade_paginas)
        return -1;

    return tabela_densa->quadros[pagina_virtual];
}

void inserirPagina(TabelaDensa* tabela_densa, unsigned int pagina_virtual, int quadro_fisico){
    if(pagina_virtual >= tabela_densa->quantidade_paginas)
        return;

    tabela_densa->quadros[pagina_virtual] = quadro_fisico;
}

void removerPagina(TabelaDensa* tabela_densa, unsigned int pagina_virtual){
    if(pagina_virtual >= tabela_densa->quantidade_paginas)
        return;

    tabela_densa->quadros[pagina_virtual] = -1;
}

void liberarTabelaDensa(TabelaDensa* tabela_densa){
    if(tabela_densa == NULL)
        return;

    free(tabela_densa->quadros);
    free(tabela_densa);
}