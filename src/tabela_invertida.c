#include "tabela_invertida.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

// Cria uma entrada por quadro físico; cada posição guarda a página nele armazenada.
TabelaInvertida* inicializarTI(unsigned int quantidade_quadros){
    TabelaInvertida* tabela = malloc(sizeof(TabelaInvertida));

    if(tabela == NULL){
        printf("Falha ao alocar tabela!");
        return NULL;
    }

    tabela->quantidade_quadros = quantidade_quadros;
    tabela->paginas = malloc(quantidade_quadros * sizeof(int));

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

// Busca sequencialmente a página virtual e retorna o índice do quadro onde ela está.
int consultarPaginaTI(TabelaInvertida* tabela, unsigned int pagina_virtual){
    for (unsigned int i = 0; i < tabela->quantidade_quadros; ++i) {
        if (tabela->paginas[i] == (int)pagina_virtual) {
            return (int)i;
        }
    }
    return -1;
}

// Associa a página virtual ao quadro físico indicado.
void inserirPaginaTI(TabelaInvertida* tabela, unsigned int pagina_virtual, int quadro_fisico){
    if (quadro_fisico < 0 || quadro_fisico >= (int)tabela->quantidade_quadros) {
        return;
    }

    tabela->paginas[quadro_fisico] = (int)pagina_virtual;
}

// Procura a página e marca sua entrada como vazia.
void removerPaginaTI(TabelaInvertida* tabela, unsigned int pagina_virtual){
    for (unsigned int i = 0; i < tabela->quantidade_quadros; ++i) {
        if (tabela->paginas[i] == (int)pagina_virtual) {
            tabela->paginas[i] = -1;
        }
    }
}

// Libera o vetor de páginas e a estrutura da tabela invertida.
void liberarTI(TabelaInvertida* tabela){
    if(tabela == NULL)
        return;

    free(tabela->paginas);
    free(tabela);
}