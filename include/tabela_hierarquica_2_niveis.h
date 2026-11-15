#ifndef TABELA_HIERARQUICA_2_NIVEIS_H
#define TABELA_HIERARQUICA_2_NIVEIS_H

typedef struct {
    int **paginas;
    unsigned int bits_para_pagina_N1;
    unsigned int bits_para_pagina_N2;
    unsigned int quantidade_paginas_N1;
    unsigned int quantidade_paginas_N2;

} TabelaHierarquica2;

TabelaHierarquica2* inicializarTH2(unsigned int tamanho_pagina);

int consultarPaginaTH2(TabelaHierarquica2* tabela, unsigned int pagina_virtual);

void inserirPaginaTH2(TabelaHierarquica2* tabela, unsigned int pagina_virtual, int quadro_fisico);

void removerPaginaTH2(TabelaHierarquica2* tabela, unsigned int pagina_virtual);

void liberarTH2(TabelaHierarquica2* tabela);

#endif