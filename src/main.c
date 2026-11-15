#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "simulador.h"
#include "memoria.h"
#include "algoritmos/algoritmo.h"
#include "configuracao.h"
#include "tabela_paginas.h"

int main(int argc, char *argv[]){
    // Verifica se a quantidade de argumentos recebidos está correta.
    if(argc < 5 || argc > 7){
        printf("Argumentos incorretos: ./simulador <algoritmo> <arquivo> <tam_pagina> <tam_memoria> [densa|th2|th3|invertida] [debug|debug2]\n");
        return 1;
    }

    Configuracao configuracao;
    const char* tipo_tabela = "th2"; // Define a tabela de páginas padrão.
    configuracao.debug = 0; // Desativa o modo de depuração por padrão.

    // Identifica os argumentos opcionais de tabela e depuração.
    for(int i = 5; i < argc; i++){
        if(strcmp(argv[i], "debug") == 0)
            configuracao.debug = 1;
        else if(strcmp(argv[i], "debug2") == 0)
            configuracao.debug = 2;
        else
            tipo_tabela = argv[i];
    }

    // Armazena os parâmetros obrigatórios recebidos pela linha de comando.
    configuracao.algoritmo = argv[1];
    configuracao.arquivo = argv[2];
    configuracao.tamanho_pagina = atoi(argv[3]);

    // Verifica se o tamanho da página está dentro dos limites permitidos.
    if(configuracao.tamanho_pagina > 64 || configuracao.tamanho_pagina < 2)
    {
        printf("Tamanho da página fora dos limites permitidos!\n");
        return 1;
    }

    configuracao.tamanho_memoria = atoi(argv[4]);

    // Verifica se o tamanho da memória está dentro dos limites permitidos.
    if (configuracao.tamanho_memoria > 16384 || configuracao.tamanho_memoria < 128)
    {
        printf("Tamanho da memória fora dos limites permitidos!\n");
        return 1;
    }

    // Abre o arquivo de acessos à memória para leitura.
    FILE *arquivo = fopen(configuracao.arquivo, "r");
    if(arquivo == NULL){
        printf("Falha ao ler o arquivo!\n");
        return 1;
    }

    // Calcula quantos quadros cabem na memória física.
    unsigned int qtd_quadros = calcularQuantidadeQuadros(configuracao.tamanho_memoria, configuracao.tamanho_pagina);

    // Inicializa os quadros da memória física.
    Quadro* memoria = inicializarMemoria(qtd_quadros);
    if(memoria == NULL){
        printf("Falha ao alocar memória!\n");
        return 1;
    }

    // Exibe as configurações da simulação quando o modo debug está ativo.
    if(configuracao.debug)
        printf("[debug] algoritmo=%s tabela=%s pagina=%uKB memoria=%uKB quadros=%u\n",
               configuracao.algoritmo, tipo_tabela,
               (unsigned int)configuracao.tamanho_pagina,
               (unsigned int)configuracao.tamanho_memoria, qtd_quadros);

    // Seleciona e inicializa o algoritmo de substituição de páginas.
    Algoritmo algoritmo = escolherAlgoritmo(configuracao.algoritmo);
    algoritmo.inicializar(qtd_quadros, configuracao);

    // Seleciona a implementação da tabela de páginas.
    TabelaPaginas tabela = escolherTabela(tipo_tabela);

    // Executa a simulação utilizando as configurações e estruturas selecionadas.
    executarSimulacao(arquivo, memoria, configuracao, qtd_quadros, algoritmo, tabela);

    // Libera os recursos utilizados pela simulação.
    algoritmo.liberar();
    liberarMemoria(memoria);
    fclose(arquivo);

    return 0;
}