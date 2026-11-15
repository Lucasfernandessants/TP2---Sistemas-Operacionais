#include "tabela_invertida.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

TabelaInvertida* inicializarTI(unsigned int quantidade_quadros){
    TabelaInvertida* tabela = malloc(sizeof(TabelaInvertida));
    tabela->memoria_ocupada = sizeof(TabelaInvertida);

    if(tabela == NULL){
        printf("Falha ao alocar tabela!");
        return NULL;
    }

    tabela->quantidade_quadros = quantidade_quadros;
    tabela->paginas = malloc(quantidade_quadros * sizeof(int));
    tabela->memoria_ocupada += (quantidade_quadros * sizeof(int));

    if(tabela->paginas == NULL){
        printf("Falha ao alocar paginas!");
        free(tabela);
        return NULL;
    }

    for(unsigned int i = 0; i < tabela->quantidade_quadros; i++){
        tabela->paginas[i] = -1;
    }

    return tabela;
}

int consultarPaginaTI(TabelaInvertida* tabela, unsigned int pagina_virtual){
    for (unsigned int i = 0; i < tabela->quantidade_quadros; ++i) {
        if (tabela->paginas[i] == (int)pagina_virtual) {
            return (int)i;
        }
    }
    return -1;
}

void inserirPaginaTI(TabelaInvertida* tabela, unsigned int pagina_virtual, int quadro_fisico){
    if (quadro_fisico < 0 || quadro_fisico >= (int)tabela->quantidade_quadros) {
        return;
    }

    tabela->paginas[quadro_fisico] = (int)pagina_virtual;
}

void removerPaginaTI(TabelaInvertida* tabela, unsigned int pagina_virtual){
    for (unsigned int i = 0; i < tabela->quantidade_quadros; ++i) {
        if (tabela->paginas[i] == (int)pagina_virtual) {
            tabela->paginas[i] = -1;
        }
    }
}

void liberarTI(TabelaInvertida* tabela){
    if(tabela == NULL)
        return;

    free(tabela->paginas);
    free(tabela);
}
