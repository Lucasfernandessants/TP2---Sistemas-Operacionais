#include "algoritmos/optimal.h"
#include "arquivo.h"
#include <stdlib.h>
#include <stdio.h>
#include "simulador.h"

Algoritmo criarOpt(){
    printf("Algoritmo escolhido: opt!\n");
    Algoritmo algoritmo;
    algoritmo.estado = NULL;
    algoritmo.inicializar = optInicializar;
    algoritmo.acessarPagina = optAcessarPagina;
    algoritmo.carregarPagina = optCarregarPagina;
    algoritmo.selecionarVitima = optSelecionarVitima;
    algoritmo.liberar = optLiberar;
    return algoritmo;
}

static unsigned int *proximos;

void optInicializar(unsigned int qtd_quadros, Configuracao configuracao){

    FILE *arquivo = fopen(configuracao.arquivo, "r");
    if(arquivo == NULL){
        printf("Falha ao ler o arquivo!\n");
        return;
    }

    Acesso acesso;
    int resultado = 2, contador = 0;
    unsigned int deslocamento = calcularDeslocamento(configuracao.tamanho_pagina);

    while(1){
        resultado = lerAcesso(arquivo, &acesso);

        if(resultado == EOF){
            break;
        }
        if(resultado != 2){
            break;
        }
        contador++;
    }
    rewind(arquivo);

    resultado = 2;
    unsigned int qtd_acessos = contador;
    unsigned int *paginas = malloc(qtd_acessos * sizeof(unsigned int));
    contador = 0;

    while(resultado  == 2){
        resultado = lerAcesso(arquivo, &acesso);

        if(resultado == EOF){
            break;
        }
        if(resultado != 2){
            break;
        }
        paginas[contador] = determinarPagina(acesso.endereco, deslocamento);
        contador++;
    }

    fclose(arquivo);

    unsigned int qtd_paginas = (1ULL << 32) / (configuracao.tamanho_pagina * 1024);
    unsigned int *ultima = malloc(qtd_paginas * sizeof(unsigned int));
    proximos = malloc(qtd_acessos * sizeof(unsigned int));

    for(unsigned int i = 0; i<qtd_paginas; i++){
        ultima[i] = qtd_acessos;
    }

    for(int i = (int)qtd_acessos; i >= 0; i--){
        proximos[i] = ultima[paginas[i]];
        ultima[paginas[i]] = i;
    }

    free(ultima);
    free(paginas);

}

void optAcessarPagina(Quadro *memoria, int quadro, unsigned long long tempo){
    memoria[quadro].proximo_uso = proximos[tempo-1];
}

void optCarregarPagina(Quadro *memoria, int quadro, unsigned long long tempo){
    memoria[quadro].proximo_uso = proximos[tempo-1];
}

int optSelecionarVitima(Quadro *memoria, unsigned int qtd_quadros){
    int vitima = 0;
    for(unsigned int i = 1; i<qtd_quadros;i++){
        if(memoria[i].proximo_uso > memoria[vitima].proximo_uso)
            vitima = i;
    }
    return vitima;
}

void optLiberar(void){
    free(proximos);
}