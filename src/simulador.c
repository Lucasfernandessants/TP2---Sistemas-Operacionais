#include <math.h>
#include <stdio.h>
#include "simulador.h"
#include "memoria.h"
#include "arquivo.h"
#include "tabela_paginas.h"

// Calcula a quantidade de bits necessária para representar o deslocamento dentro da página.
unsigned int calcularDeslocamento(unsigned int tamanho_pagina){
    return log2(tamanho_pagina * 1024);
}

// Obtém o número da página deslocando o endereço pelos bits de deslocamento.
unsigned int determinarPagina(unsigned int endereco, unsigned int s){
    return endereco >> s;
}

// Exibe o estado dos quadros de memória durante a depuração.
static void imprimirQuadros(Quadro* memoria, unsigned int quadros_usados){
    for(unsigned int i = 0; i < quadros_usados; i++){
        printf("    quadro %u: pagina 0x%x %s | ultimo_acesso=%llu bit_ref=%d proximo_uso=%u\n",
               i, memoria[i].pagina, memoria[i].sujo ? "suja" : "limpa",
               memoria[i].ultimo_acesso, memoria[i].bit_ref, memoria[i].proximo_uso);
    }
}

void executarSimulacao(FILE* arquivo, Quadro* memoria, Configuracao configuracao, unsigned int qtd_quadros, Algoritmo algoritmo, TabelaPaginas tabela){
    Estatisticas estatisticas = {0}; // Inicializa todas as estatísticas com zero.
    Acesso acesso;
    int resultado = 2;
    int quadro;
    unsigned int quadros_usados = 0, deslocamento, pagina;

    // Inicializa a tabela de páginas com o tamanho da página e a quantidade de quadros.
    tabela.tabela_atual = tabela.inicializar(configuracao.tamanho_pagina, qtd_quadros);
    if (tabela.tabela_atual == NULL) {
        printf("Falha ao inicializar tabela!");
        return;
    }

    // Calcula quantos bits devem ser deslocados para obter o número da página.
    deslocamento = calcularDeslocamento(configuracao.tamanho_pagina);

    if(tabela.tabela_atual == NULL){
        printf("Falha ao inicializar tabela!\n");
        return;
    }

    // Processa os acessos ao arquivo enquanto houver registros válidos.
    while(resultado  == 2){
        resultado = lerAcesso(arquivo, &acesso);

        if(resultado == EOF){
            if(configuracao.debug)
                printf("End of file!\n");
            break;
        }
        if(resultado != 2){
            printf("wrong number of arguments!\n");
            break;
        }

        estatisticas.acessos++; // Contabiliza o acesso atual.
        pagina = determinarPagina(acesso.endereco, deslocamento); // Identifica a página acessada.
        quadro = tabela.consultar(tabela.tabela_atual, pagina); // Verifica se a página está na memória.

        // Exibe os dados do acesso atual quando o modo debug está ativo.
        if(configuracao.debug)
            printf("#%llu %08x %c -> pagina 0x%x: ",
                   estatisticas.acessos, acesso.endereco, acesso.operacao, pagina);

        if(quadro == -1){
            estatisticas.faults++; // Contabiliza uma falta de página.

            if(quadros_usados < qtd_quadros){
                quadro = quadros_usados++; // Utiliza o próximo quadro livre.
                if(configuracao.debug)
                    printf("FAULT, quadro livre %d\n", quadro);
            }else{
                // Seleciona uma página para substituição quando a memória está cheia.
                quadro = algoritmo.selecionarVitima(memoria, qtd_quadros);
                if(configuracao.debug)
                    printf("FAULT, memoria cheia, vitima: quadro %d (pagina 0x%x%s)\n",
                           quadro, memoria[quadro].pagina,
                           memoria[quadro].sujo ? ", suja: escrita em disco" : "");
                if(memoria[quadro].sujo)
                    estatisticas.escritas++; // Contabiliza a escrita da página suja em disco.
                tabela.remover(tabela.tabela_atual, memoria[quadro].pagina); // Remove a página substituída da tabela.
            }

            // Registra a nova página no quadro selecionado.
            tabela.inserir(tabela.tabela_atual, pagina, quadro);
            memoria[quadro].ocupado = 1;
            memoria[quadro].sujo = (acesso.operacao == 'W');
            memoria[quadro].pagina = pagina;
            algoritmo.carregarPagina(memoria, quadro, estatisticas.acessos); // Atualiza o estado do algoritmo após carregar a página.

        }else{
            // Página encontrada na memória: ocorre um HIT.
            if(configuracao.debug)
                printf("HIT, quadro %d%s\n", quadro,
                       acesso.operacao == 'W' ? " (escrita: marcada suja)" : "");
            if (acesso.operacao == 'W') memoria[quadro].sujo = 1; // Marca a página como suja em caso de escrita.
            algoritmo.acessarPagina(memoria, quadro, estatisticas.acessos); // Atualiza o estado do algoritmo após o acesso.
        }

        // Exibe o estado da memória após cada acesso no segundo nível de depuração.
        if(configuracao.debug >= 2)
            imprimirQuadros(memoria, quadros_usados);
    }

    // Libera a tabela de páginas e apresenta as estatísticas finais da simulação.
    tabela.liberar(tabela.tabela_atual);
    printf("Executando o simulador...\n");
    printf("Arquivo de entrada: %s\n", configuracao.arquivo);
    printf("Tamanho da memória: %u KB\n", (unsigned int)configuracao.tamanho_memoria);
    printf("Tamanho das páginas: %u KB\n", (unsigned int)configuracao.tamanho_pagina);
    printf("Técnica de reposição: %s\n", configuracao.algoritmo);
    printf("Total de acessos à memória: %llu\n", estatisticas.acessos);
    printf("Páginas lidas: %llu\n", estatisticas.faults);
    printf("Páginas escritas: %llu\n", estatisticas.escritas);
}