#ifndef ARQUIVO_H
#define ARQUIVO_H

#include <stdio.h>

typedef struct {
    unsigned int endereco;
    char operacao;
} Acesso;

int lerAcesso(FILE *arquivo, Acesso *acesso);

#endif