
CC = gcc

CFLAGS = -Iinclude -Wall -Wextra

LDLIBS = -lm

TARGET = output/main.exe

SOURCES = src/main.c \
          src/simulador.c \
          src/memoria.c \
          src/arquivo.c \
          src/tabela_densa.c \
		  src/algoritmos/algoritmo.c \
		  src/algoritmos/lru.c \

all: $(TARGET)

$(TARGET): $(SOURCES) | output
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET) $(LDLIBS)

output:
	mkdir output

clean:
	rm -f $(TARGET)

.PHONY: all clean
