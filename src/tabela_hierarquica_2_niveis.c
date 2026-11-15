#include "tabela_hierarquica_2_niveis.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

/* Alocamos a primeira tabela e todos os endereços como NULL pois criaremos as tabelas de nível 2
 * apenas sob demanda
 */
TabelaHierarquica2* inicializarTH2(unsigned int tamanho_pagina){
    TabelaHierarquica2* tabela_hierarquica_2 = malloc(sizeof(TabelaHierarquica2));
    tabela_hierarquica_2->memoria_ocupada = sizeof(TabelaHierarquica2);

    if(tabela_hierarquica_2 == NULL){
        printf("Falha ao alocar tabela!");
        return NULL;
    }

    unsigned int bits_offset = log2(tamanho_pagina * 1024);
    unsigned int bits_para_paginas = 32 - bits_offset;

    tabela_hierarquica_2->bits_para_pagina_N1 = bits_para_paginas / 2;
    tabela_hierarquica_2->bits_para_pagina_N2 = bits_para_paginas - tabela_hierarquica_2->bits_para_pagina_N1;
    tabela_hierarquica_2->quantidade_paginas_N1 = pow(2, tabela_hierarquica_2->bits_para_pagina_N1);
    tabela_hierarquica_2->quantidade_paginas_N2 = pow(2, tabela_hierarquica_2->bits_para_pagina_N2);

    tabela_hierarquica_2->paginas = malloc(tabela_hierarquica_2->quantidade_paginas_N1 * sizeof(int*));
    tabela_hierarquica_2->memoria_ocupada += (tabela_hierarquica_2->quantidade_paginas_N1 * sizeof(int*));

    if(tabela_hierarquica_2->paginas == NULL){
        printf("Falha ao alocar quadros!");
        free(tabela_hierarquica_2);
        return NULL;
    }

    for(unsigned int i = 0; i < tabela_hierarquica_2->quantidade_paginas_N1; i++){
        tabela_hierarquica_2->paginas[i] = NULL;
    }

    return tabela_hierarquica_2;
}

int consultarPaginaTH2(TabelaHierarquica2* tabela_hierarquica_2, unsigned int pagina_virtual){
    // Jogando os bit N - N / 2 vezes para a direita
    unsigned int pagina_N1 = pagina_virtual >> tabela_hierarquica_2->bits_para_pagina_N2;
    // Pegando apenas os N - N / 2 últimos bits 
    unsigned int pagina_N2 = pagina_virtual & ((1U << tabela_hierarquica_2->bits_para_pagina_N2) - 1);

    if (pagina_N1 >= tabela_hierarquica_2->quantidade_paginas_N1 || pagina_N2 >= tabela_hierarquica_2->quantidade_paginas_N2) {
        return - 1;
    }

    if (tabela_hierarquica_2->paginas[pagina_N1] == NULL) {
        return - 1;
    }

    return tabela_hierarquica_2->paginas[pagina_N1][pagina_N2];
}

// Alocamos somente sob demanda a segunda tabela;
void inserirPaginaTH2(TabelaHierarquica2* tabela_hierarquica_2, unsigned int pagina_virtual, int quadro_fisico){
    // Jogando os bit N - N / 2 vezes para a direita
    unsigned int pagina_N1 = pagina_virtual >> tabela_hierarquica_2->bits_para_pagina_N2;
    // Pegando apenas os N - N / 2 últimos bits 
    unsigned int pagina_N2 = pagina_virtual & ((1U << tabela_hierarquica_2->bits_para_pagina_N2) - 1);

    if (pagina_N1 >= tabela_hierarquica_2->quantidade_paginas_N1 || pagina_N2 >= tabela_hierarquica_2->quantidade_paginas_N2) {
        return;
    }

    // Alocação sob demanda
    if (tabela_hierarquica_2->paginas[pagina_N1] == NULL) {
        tabela_hierarquica_2->paginas[pagina_N1] = malloc(tabela_hierarquica_2->quantidade_paginas_N2 * sizeof(int));
        tabela_hierarquica_2->memoria_ocupada += (tabela_hierarquica_2->quantidade_paginas_N2 * sizeof(int));
        if (tabela_hierarquica_2->paginas[pagina_N1] == NULL) {
            return;
        }
        for (unsigned int i = 0; i < tabela_hierarquica_2->quantidade_paginas_N2; ++i) {
            tabela_hierarquica_2->paginas[pagina_N1][i] = -1;
        }
    }

    tabela_hierarquica_2->paginas[pagina_N1][pagina_N2] = quadro_fisico;
}

void removerPaginaTH2(TabelaHierarquica2* tabela_hierarquica_2, unsigned int pagina_virtual){
    // Jogando os bit N - N / 2 vezes para a direita
    unsigned int pagina_N1 = pagina_virtual >> tabela_hierarquica_2->bits_para_pagina_N2;
    // Pegando apenas os N - N / 2 últimos bits 
    unsigned int pagina_N2 = pagina_virtual & ((1U << tabela_hierarquica_2->bits_para_pagina_N2) - 1);

    if (pagina_N1 >= tabela_hierarquica_2->quantidade_paginas_N1 || pagina_N2 >= tabela_hierarquica_2->quantidade_paginas_N2) {
        return;
    }

    if (tabela_hierarquica_2->paginas[pagina_N1] == NULL) {
        return;
    }
    if (tabela_hierarquica_2->paginas[pagina_N1][pagina_N2] == - 1) {
        return;
    }
    
    tabela_hierarquica_2->paginas[pagina_N1][pagina_N2] = -1;
}

void liberarTH2(TabelaHierarquica2* tabela_hierarquica_2){
    if(tabela_hierarquica_2 == NULL)
        return;
    
    for (unsigned int i = 0; i < tabela_hierarquica_2->quantidade_paginas_N1; ++i) {
        if (tabela_hierarquica_2->paginas[i] != NULL) {
            free(tabela_hierarquica_2->paginas[i]);
        }
    }

    free(tabela_hierarquica_2->paginas);
    free(tabela_hierarquica_2);
}