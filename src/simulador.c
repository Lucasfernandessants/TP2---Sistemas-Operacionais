#include <math.h>
#include <stdio.h>
#include "simulador.h"
#include "memoria.h"
#include "arquivo.h"
#include "tabela_paginas.h"

unsigned int calcularDeslocamento(unsigned int tamanho_pagina){
    return log2(tamanho_pagina * 1024);
}

unsigned int determinarPagina(unsigned int endereco, unsigned int s){
    return endereco >> s;
}

void executarSimulacao(FILE* arquivo, Quadro* memoria, Configuracao configuracao, unsigned int qtd_quadros, Algoritmo algoritmo, TabelaPaginas tabela){
    Estatisticas estatisticas = {0};
    Acesso acesso;
    int resultado = 2;
    int quadro;
    unsigned int quadros_usados = 0, deslocamento, pagina;

    tabela.tabela_atual = tabela.inicializar(configuracao.tamanho_pagina, qtd_quadros);
    if (tabela.tabela_atual == NULL) {
        printf("Falha ao inicializar tabela!");
        return;
    }

    deslocamento = calcularDeslocamento(configuracao.tamanho_pagina);

    if(tabela.tabela_atual == NULL){
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
        quadro = tabela.consultar(tabela.tabela_atual, pagina);

        if(quadro == -1){
            estatisticas.faults++;

            if(quadros_usados < qtd_quadros)
                quadro = quadros_usados++;
            else{
                quadro = algoritmo.selecionarVitima(memoria, qtd_quadros);
                if(memoria[quadro].sujo) 
                    estatisticas.escritas++;
                tabela.remover(tabela.tabela_atual, memoria[quadro].pagina);
            }

            tabela.inserir(tabela.tabela_atual, pagina, quadro);
            memoria[quadro].ocupado = 1;
            memoria[quadro].sujo = (acesso.operacao == 'W');
            memoria[quadro].pagina = pagina;
            algoritmo.carregarPagina(memoria, quadro, estatisticas.acessos);

        }else{
            if (acesso.operacao == 'W') memoria[quadro].sujo = 1;
            algoritmo.acessarPagina(memoria, quadro, estatisticas.acessos);
        }

    }
    tabela.liberar(tabela.tabela_atual);
    printf("Acessos: %llu\n Escritas: %llu\n Faults: %llu\n", estatisticas.acessos, estatisticas.escritas,estatisticas.faults);
}
