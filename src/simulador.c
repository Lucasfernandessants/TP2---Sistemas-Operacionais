#include <math.h>
#include <stdio.h>
#include "simulador.h"
#include "memoria.h"
#include "arquivo.h"
#include "tabela_densa.h"

unsigned int calcularDeslocamento(unsigned int tamanho_pagina){
    return log2(tamanho_pagina * 1024);
}

unsigned int determinarPagina(unsigned int endereco, unsigned int s){
    return endereco >> s;
}

void executarSimulacao(FILE* arquivo, Quadro* memoria, Configuracao configuracao, unsigned int qtd_quadros){
    Acesso acesso;
    int resultado = 2;
    int count = 0;
    unsigned int deslocamento, pagina;
    TabelaDensa* tabela_densa = inicializarTabelaDensa(configuracao.tamanho_pagina);
    deslocamento = calcularDeslocamento(configuracao.tamanho_pagina);

    if(tabela_densa == NULL){
        printf("Falha ao inicializar tabela!");
        return;
    }
    
    while(resultado  == 2){
        resultado = lerAcesso(arquivo, &acesso);

        if(resultado == EOF){
            printf("End of file!");
            break;
        }
        if(resultado != 2){
            printf("wrong number of arguments!");
            break;
        }

        pagina = determinarPagina(acesso.endereco, deslocamento);
        int quadro = consultarPagina(tabela_densa, pagina);

        if(quadro == -1){
            int quadro_livre = encontrarQuadroLivre(memoria, qtd_quadros);
            if(quadro_livre == -1){
                printf("Sem quadros disponiveis!");
                break;
            }
            inserirPagina(tabela_densa, pagina, quadro_livre);
            memoria[quadro_livre].ocupado = 1;
            if(acesso.operacao == 'W')
                memoria[quadro_livre].sujo = 1;
            else
                memoria[quadro_livre].sujo = 0;
            memoria[quadro_livre].pagina = pagina;

        }else if(acesso.operacao == 'W'){
            memoria[quadro].sujo = 1;
        }

        count++;
    }
    liberarTabelaDensa(tabela_densa);
}
