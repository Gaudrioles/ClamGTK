.PHONY: all clean

# Variables #
CC = gcc
BIN = ClamGTK
SRC= $(wildcard *.c)
OBJ= $(SRC:.c=.o)

# Flags pour le Compilateur et l'editeur de liens #
CFLAGS = -Wall -Werror -pedantic-errors -O3 `pkg-config --cflags gtk+-3.0`
LFLAGS = -pthread `pkg-config --libs gtk+-3.0`
 
# Construction du programme #

all : $(BIN)

%.o: %.c *.h
	$(CC) -c $< $(CFLAGS) -o $@

%.o: %.c
	$(CC) -c $< $(CFLAGS) -o $@

$(BIN): $(OBJ)
	$(CC) -s $^ $(LFLAGS) -o $@

# Nettoyage #
clean:
	rm -f $(OBJ) $(BIN)