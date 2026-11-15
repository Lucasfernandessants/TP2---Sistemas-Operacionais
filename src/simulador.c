#define _POSIX_C_SOURCE 199309L
#include <math.h>
#include <stdio.h>
#include <time.h>
#include "simulador.h"
#include "memoria.h"
#include "arquivo.h"
#include "tabela_paginas.h"

static double calcularTempo(struct timespec inicio, struct timespec fim) {
    return (fim.tv_sec - inicio.tv_sec) * 1000.0 + (fim.tv_nsec - inicio.tv_nsec) / 1e6;
}

unsigned int calcularDeslocamento(unsigned int tamanho_pagina){
    return log2(tamanho_pagina * 1024);
}

unsigned int determinarPagina(unsigned int endereco, unsigned int s){
    return endereco >> s;
}

void executarSimulacao(FILE* arquivo
    , Quadro* memoria
    , Configuracao configuracao
    , unsigned int qtd_quadros
    , Algoritmo algoritmo
    , TabelaPaginas tabela
) {
    
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

    struct timespec inicio_global, fim_global, inicio_consulta, fim_consulta;
    double tempo_consultas_ms = 0.0;

    clock_gettime(CLOCK_MONOTONIC, &inicio_global);
    
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

        clock_gettime(CLOCK_MONOTONIC, &inicio_consulta);
        quadro = tabela.consultar(tabela.tabela_atual, pagina);
        clock_gettime(CLOCK_MONOTONIC, &fim_consulta);
        tempo_consultas_ms += calcularTempo(inicio_consulta, fim_consulta);

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

    clock_gettime(CLOCK_MONOTONIC, &fim_global);
    double tempo_global = calcularTempo(inicio_global, fim_global);
    size_t memoria_ocupada = tabela.obterMemoria(tabela.tabela_atual);

    tabela.liberar(tabela.tabela_atual);
    printf("Acessos: %llu\n Escritas: %llu\n Faults: %llu\n Memória: %zu Bytes\n Tempo Consulta: %.3f ms\n Tempo Total: %.3f ms\n", estatisticas.acessos, estatisticas.escritas, estatisticas.faults, memoria_ocupada, tempo_consultas_ms, tempo_global);
}
