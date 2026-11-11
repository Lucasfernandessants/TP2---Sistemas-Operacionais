#ifndef MEMORIA_H
#define MEMORIA_H

typedef struct {
    unsigned int pagina;
    int ocupado;
    int sujo;
} Quadro;

unsigned int calcularQuantidadeQuadros(unsigned int memoria, unsigned int paginas);

int encontrarQuadroLivre(Quadro *memoria, unsigned int quantidade_quadros);

Quadro* inicializarMemoria(unsigned int quantidade_quadros);

void liberarMemoria(Quadro *memoria);

#endif