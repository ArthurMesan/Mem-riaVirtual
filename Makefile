# Define o compilador C
CC = gcc

# Define as flags de compilação
# -Wall: Ativa todos os avisos (boa prática)
# -g: Adiciona informações de depuração
# -std=c99: Especifica o padrão C99 (o TP exige C, não C++)
CFLAGS = -Wall -g -std=c99

# Nome do executável
TARGET = simu3lador

# Lista dos seus arquivos-fonte (.c)
# Adicione aqui todos os .c do seu projeto
SOURCES = simulador.c memoria.c

# Lista dos seus arquivos de cabeçalho (.h)
HEADERS = simulador.h memoria.h

# Alvo principal: compila o programa
all: $(TARGET)

# Regra para criar o executável
# 1. Agora depende de TODOS os fontes e cabeçalhos
# 2. O comando de compilação agora inclui TODOS os fontes
$(TARGET): $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES)

# Alvo para limpar os arquivos compilados
clean:
	rm -f $(TARGET)