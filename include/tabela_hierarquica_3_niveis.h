#ifndef TABELA_HIERARQUICA_3_NIVEIS_H
#define TABELA_HIERARQUICA_3_NIVEIS_H

typedef struct {
    int ***paginas;
    unsigned int bits_para_pagina_N1;
    unsigned int bits_para_pagina_N2;
    unsigned int bits_para_pagina_N3;
    unsigned int quantidade_paginas_N1;
    unsigned int quantidade_paginas_N2;
    unsigned int quantidade_paginas_N3;

} TabelaHierarquica3;

TabelaHierarquica3* inicializarTH3(unsigned int tamanho_pagina);

int consultarPaginaTH3(TabelaHierarquica3* tabela, unsigned int pagina_virtual);

void inserirPaginaTH3(TabelaHierarquica3* tabela, unsigned int pagina_virtual, int quadro_fisico);

void removerPaginaTH3(TabelaHierarquica3* tabela, unsigned int pagina_virtual);

void liberarTH3(TabelaHierarquica3* tabela);

#endif