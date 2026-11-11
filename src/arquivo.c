#include <stdio.h>
#include "arquivo.h"

int lerAcesso(FILE *arquivo, Acesso *acesso){
    int resultado = fscanf(arquivo, "%x %c",
                       &acesso->endereco,
                       &acesso->operacao);
    return resultado;
}