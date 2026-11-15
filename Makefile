
CC = gcc

CFLAGS = -Iinclude -Wall -Wextra

LDLIBS = -lm

TARGET = simulador

SOURCES = src/main.c \
          src/simulador.c \
          src/memoria.c \
          src/arquivo.c \
          src/tabela_densa.c \
		  src/algoritmos/algoritmo.c \
		  src/algoritmos/lru.c \
		  src/algoritmos/random.c \
		  src/algoritmos/clock.c \
		  src/tabela_hierarquica_2_niveis.c \
		  src/tabela_paginas.c \
		  src/tabela_hierarquica_3_niveis.c \
		  src/tabela_invertida.c
all: $(TARGET)

$(TARGET): $(SOURCES) | output
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET) $(LDLIBS)

output:
	mkdir output

clean:
	rm -f $(TARGET)

.PHONY: all clean
