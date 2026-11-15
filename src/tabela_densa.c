#include "tabela_densa.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

// Cria uma entrada para cada página virtual possível e marca todas como não mapeadas.
TabelaDensa* inicializarTabelaDensa(unsigned int tamanho_pagina){
    TabelaDensa* tabela_densa = malloc(sizeof(TabelaDensa));

    if(tabela_densa == NULL){
        printf("Falha ao alocar tabela!");
        return NULL;
    }

    // O espaço de endereçamento de 32 bits determina o total de páginas possíveis.
    tabela_densa->quantidade_paginas = pow(2,32)/(tamanho_pagina*1024);
    tabela_densa->paginas = malloc(tabela_densa->quantidade_paginas * sizeof(int));
    tabela_densa->memoria_ocupada = sizeof(tabela_densa) + (tabela_densa->quantidade_paginas * sizeof(int));

    if(tabela_densa->paginas == NULL){
        printf("Falha ao alocar paginas!");
        free(tabela_densa);
        return NULL;
    }

    for(unsigned int i = 0; i < tabela_densa->quantidade_paginas; i++){
        tabela_densa->paginas[i] = -1;
    }

    return tabela_densa;
}

// Consulta diretamente a posição correspondente à página virtual.
int consultarPagina(TabelaDensa* tabela_densa, unsigned int pagina_virtual){
    if(pagina_virtual >= tabela_densa->quantidade_paginas)
        return -1;

    return tabela_densa->paginas[pagina_virtual];
}

// Registra em qual quadro físico a página virtual está armazenada.
void inserirPagina(TabelaDensa* tabela_densa, unsigned int pagina_virtual, int quadro_fisico){
    if(pagina_virtual >= tabela_densa->quantidade_paginas)
        return;

    tabela_densa->paginas[pagina_virtual] = quadro_fisico;
}

// Marca a página virtual como não mapeada.
void removerPagina(TabelaDensa* tabela_densa, unsigned int pagina_virtual){
    if(pagina_virtual >= tabela_densa->quantidade_paginas)
        return;

    tabela_densa->paginas[pagina_virtual] = -1;
}

// Libera o vetor de mapeamentos e a estrutura da tabela.
void liberarTabelaDensa(TabelaDensa* tabela_densa){
    if(tabela_densa == NULL)
        return;

    free(tabela_densa->paginas);
    free(tabela_densa);
}
