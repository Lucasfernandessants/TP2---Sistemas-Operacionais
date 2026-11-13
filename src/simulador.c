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

void executarSimulacao(FILE* arquivo, Quadro* memoria, Configuracao configuracao, unsigned int qtd_quadros, Algoritmo algoritmo){
    Estatisticas estatisticas = {0};
    Acesso acesso;
    int resultado = 2;
    int quadro;
    unsigned int quadros_usados = 0, deslocamento, pagina;
    TabelaDensa* tabela_densa = inicializarTabelaDensa(configuracao.tamanho_pagina);
    deslocamento = calcularDeslocamento(configuracao.tamanho_pagina);

    if(tabela_densa == NULL){
        printf("Falha ao inicializar tabela!\n");
        return;
    }
    
    while(resultado  == 2){
        resultado = lerAcesso(arquivo, &acesso);

        if(resultado == EOF){
            printf("End of file!\n");
            break;
        }
        if(resultado != 2){
            printf("wrong number of arguments!\n");
            break;
        }

        estatisticas.acessos++;
        pagina = determinarPagina(acesso.endereco, deslocamento);
        quadro = consultarPagina(tabela_densa, pagina);

        if(quadro == -1){
            estatisticas.faults++;

            if(quadros_usados < qtd_quadros)
                quadro = quadros_usados++;
            else{
                quadro = algoritmo.selecionarVitima(memoria, qtd_quadros);
                if(memoria[quadro].sujo) 
                    estatisticas.escritas++;
                removerPagina(tabela_densa, memoria[quadro].pagina);
            }

            inserirPagina(tabela_densa, pagina, quadro);
            memoria[quadro].ocupado = 1;
            memoria[quadro].sujo = (acesso.operacao == 'W');
            memoria[quadro].pagina = pagina;
            algoritmo.carregarPagina(memoria, quadro, estatisticas.acessos);

        }else{
            if (acesso.operacao == 'W') memoria[quadro].sujo = 1;
            algoritmo.acessarPagina(memoria, quadro, estatisticas.acessos);
        }

    }
    liberarTabelaDensa(tabela_densa);
    printf("Acessos: %llu\n Escritas: %llu\n Faults: %llu\n", estatisticas.acessos, estatisticas.escritas,estatisticas.faults);
}
