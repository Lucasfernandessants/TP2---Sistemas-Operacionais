#include "tabela_hierarquica_3_niveis.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

/* Alocamos a primeira tabela e todos os endereços como NULL pois criaremos as tabelas de nível 2 e 3
 * apenas sob demanda
 */
TabelaHierarquica3* inicializarTH3(unsigned int tamanho_pagina){
    TabelaHierarquica3* tabela_hierarquica_3 = malloc(sizeof(TabelaHierarquica3));
    tabela_hierarquica_3->memoria_ocupada = sizeof(TabelaHierarquica3);

    if(tabela_hierarquica_3 == NULL){
        printf("Falha ao alocar tabela!");
        return NULL;
    }

    unsigned int bits_offset = log2(tamanho_pagina * 1024);
    unsigned int bits_para_paginas = 32 - bits_offset;

    tabela_hierarquica_3->bits_para_pagina_N1 = bits_para_paginas / 3;
    tabela_hierarquica_3->bits_para_pagina_N2 = bits_para_paginas / 3;
    tabela_hierarquica_3->bits_para_pagina_N3 = bits_para_paginas 
                                                - tabela_hierarquica_3->bits_para_pagina_N1 
                                                - tabela_hierarquica_3->bits_para_pagina_N2;

    tabela_hierarquica_3->quantidade_paginas_N1 = pow(2, tabela_hierarquica_3->bits_para_pagina_N1);
    tabela_hierarquica_3->quantidade_paginas_N2 = pow(2, tabela_hierarquica_3->bits_para_pagina_N2); 
    tabela_hierarquica_3->quantidade_paginas_N3 = pow(2, tabela_hierarquica_3->bits_para_pagina_N3); 

    tabela_hierarquica_3->paginas = malloc(tabela_hierarquica_3->quantidade_paginas_N1 * sizeof(int**));
    tabela_hierarquica_3->memoria_ocupada += (tabela_hierarquica_3->quantidade_paginas_N1 * sizeof(int**));

    if(tabela_hierarquica_3->paginas == NULL){
        printf("Falha ao alocar quadros!");
        free(tabela_hierarquica_3);
        return NULL;
    }

    for(unsigned int i = 0; i < tabela_hierarquica_3->quantidade_paginas_N1; i++){
        tabela_hierarquica_3->paginas[i] = NULL;
    }

    return tabela_hierarquica_3;
}

int consultarPaginaTH3(TabelaHierarquica3* tabela_hierarquica_3, unsigned int pagina_virtual){
    // Jogando os bit N - 2 * N / 3 vezes para a direita
    unsigned int pagina_N1 = pagina_virtual 
                                >> tabela_hierarquica_3->bits_para_pagina_N2
                                >> tabela_hierarquica_3->bits_para_pagina_N3;
    // Jogando os bit N - N / 3 vezes para direita e pegando apenas os apenas os N - N / 3 bits últimos bits 
    unsigned int pagina_N2 = pagina_virtual >> tabela_hierarquica_3->bits_para_pagina_N3 & (((1U << tabela_hierarquica_3->bits_para_pagina_N2) - 1));
    // Pegando apenas os N - N / 3 últimos bits
    unsigned int pagina_N3 = pagina_virtual & ((1U << tabela_hierarquica_3->bits_para_pagina_N3) - 1);

    if (pagina_N1 >= tabela_hierarquica_3->quantidade_paginas_N1 || pagina_N2 >= tabela_hierarquica_3->quantidade_paginas_N2 || pagina_N3 >= tabela_hierarquica_3->quantidade_paginas_N3) {
        return -1;
    }

    if (tabela_hierarquica_3->paginas[pagina_N1] == NULL) {
        return -1;
    }

    if (tabela_hierarquica_3->paginas[pagina_N1][pagina_N2] == NULL) {
        return -1;
    }

    return tabela_hierarquica_3->paginas[pagina_N1][pagina_N2][pagina_N3];
}

// Alocamos somente sob demanda a segunda tabela e o mesmo vale para a terceira tabela;
void inserirPaginaTH3(TabelaHierarquica3* tabela_hierarquica_3, unsigned int pagina_virtual, int quadro_fisico){
    // Jogando os bit N - 2 * N / 3 vezes para a direita
    unsigned int pagina_N1 = pagina_virtual 
                                >> tabela_hierarquica_3->bits_para_pagina_N2
                                >> tabela_hierarquica_3->bits_para_pagina_N3;
    // Jogando os bit N - N / 3 vezes para direita e pegando apenas os apenas os N - N / 3 bits últimos bits 
    unsigned int pagina_N2 = pagina_virtual >> tabela_hierarquica_3->bits_para_pagina_N3 & (((1U << tabela_hierarquica_3->bits_para_pagina_N2) - 1));
    // Pegando apenas os N - N / 3 últimos bits
    unsigned int pagina_N3 = pagina_virtual & ((1U << tabela_hierarquica_3->bits_para_pagina_N3) - 1);

    if (pagina_N1 >= tabela_hierarquica_3->quantidade_paginas_N1 || pagina_N2 >= tabela_hierarquica_3->quantidade_paginas_N2 || pagina_N3 >= tabela_hierarquica_3->quantidade_paginas_N3) {
        return;
    }

    // Alocação sob demanda da segunda tabela
    if (tabela_hierarquica_3->paginas[pagina_N1] == NULL) {
        tabela_hierarquica_3->paginas[pagina_N1] = malloc(tabela_hierarquica_3->quantidade_paginas_N2 * sizeof(int*));
        tabela_hierarquica_3->memoria_ocupada += (tabela_hierarquica_3->quantidade_paginas_N2 * sizeof(int*));
        if (tabela_hierarquica_3->paginas[pagina_N1] == NULL) {
            return;
        }
        for (unsigned int i = 0; i < tabela_hierarquica_3->quantidade_paginas_N2; ++i) {
            tabela_hierarquica_3->paginas[pagina_N1][i] = NULL;
        }
    }
    
    // Alocação sob demanda da terceira tabela
    if (tabela_hierarquica_3->paginas[pagina_N1][pagina_N2] == NULL) {
        tabela_hierarquica_3->paginas[pagina_N1][pagina_N2] = malloc(tabela_hierarquica_3->quantidade_paginas_N3 * sizeof(int));
        tabela_hierarquica_3->memoria_ocupada += (tabela_hierarquica_3->quantidade_paginas_N3 * sizeof(int));
        if (tabela_hierarquica_3->paginas[pagina_N1][pagina_N2] == NULL) {
            return;
        }
        for (unsigned int i = 0; i < tabela_hierarquica_3->quantidade_paginas_N3; ++i) {
            tabela_hierarquica_3->paginas[pagina_N1][pagina_N2][i] = -1;
        }
    }

    tabela_hierarquica_3->paginas[pagina_N1][pagina_N2][pagina_N3] = quadro_fisico;
}

void removerPaginaTH3(TabelaHierarquica3* tabela_hierarquica_3, unsigned int pagina_virtual){
    // Jogando os bit N - 2 * N / 3 vezes para a direita
    unsigned int pagina_N1 = pagina_virtual 
                                >> tabela_hierarquica_3->bits_para_pagina_N2
                                >> tabela_hierarquica_3->bits_para_pagina_N3;
    // Jogando os bit N - N / 3 vezes para direita e pegando apenas os apenas os N - N / 3 bits últimos bits 
    unsigned int pagina_N2 = pagina_virtual >> tabela_hierarquica_3->bits_para_pagina_N3 & (((1U << tabela_hierarquica_3->bits_para_pagina_N2) - 1));
    // Pegando apenas os N - N / 3 últimos bits
    unsigned int pagina_N3 = pagina_virtual & ((1U << tabela_hierarquica_3->bits_para_pagina_N3) - 1);

    if (pagina_N1 >= tabela_hierarquica_3->quantidade_paginas_N1 || pagina_N2 >= tabela_hierarquica_3->quantidade_paginas_N2 || pagina_N3 >= tabela_hierarquica_3->quantidade_paginas_N3) {
        return;
    }

    if (tabela_hierarquica_3->paginas[pagina_N1] == NULL) {
        return;
    }
    if (tabela_hierarquica_3->paginas[pagina_N1][pagina_N2] == NULL) {
        return;
    }
    if (tabela_hierarquica_3->paginas[pagina_N1][pagina_N2][pagina_N3] == -1) {
        return;
    }
    
    tabela_hierarquica_3->paginas[pagina_N1][pagina_N2][pagina_N3] = -1;
}

void liberarTH3(TabelaHierarquica3* tabela_hierarquica_3){
    if(tabela_hierarquica_3 == NULL)
        return;

    for (unsigned int i = 0; i < tabela_hierarquica_3->quantidade_paginas_N1; ++i) {
        if (tabela_hierarquica_3->paginas[i] != NULL) {
            for (unsigned int j = 0; j < tabela_hierarquica_3->quantidade_paginas_N2; ++j) {
                if (tabela_hierarquica_3->paginas[i][j] != NULL) {
                    free(tabela_hierarquica_3->paginas[i][j]);
                }
            }
            free(tabela_hierarquica_3->paginas[i]);
        }
    }

    free(tabela_hierarquica_3->paginas);
    free(tabela_hierarquica_3);
}
