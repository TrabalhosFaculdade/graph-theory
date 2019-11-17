CC=gcc
CFLAGS+= -I./include -Wall -Wextra -std=c89 -pedantic-errors #-Werror  
BIN_DIR=bin
SRC_DIR=src
BIN_NAME=grafo

SRC_FILES=$(SRC_DIR)/grafo.c

all:
	mkdir -p bin
	$(CC) $(CFLAGS) $(SRC_FILES) -o bin/$(BIN_NAME)

project:
	mkdir -p bin
	$(CC) $(CFLAGS) $(SRC_FILES) -o bin/$(BIN_NAME)

.PHONY: clean

clean:
	rm -rf $(BIN_DIR)

run:
	./$(BIN_DIR)/$(BIN_NAME)

debug:
	mkdir -p bin
	$(CC) $(CFLAGS) -g $(SRC_FILES) -o bin/$(BIN_NAME)