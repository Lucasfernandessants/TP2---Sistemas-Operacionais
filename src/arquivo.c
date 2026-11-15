#include "arquivo.h"

// Lê do arquivo o endereço hexadecimal e a operação de acesso (leitura ou escrita).
int lerAcesso(FILE *arquivo, Acesso *acesso){
    int resultado = fscanf(arquivo, "%x %c",
                       &acesso->endereco,
                       &acesso->operacao);
    return resultado;
}