NAME = parse
CC = gcc
FLAGS = -std=gnu99 -pedantic
PPFLAGS = -Wl,--build-id=sha1
WARNINGS := -Wall -Wshadow -Wextra -Werror -Wundef -Wpointer-arith\
-Wcast-align -Wwrite-strings -Wconversion -Wunreachable-code

OBJ_DIR = ../obj

OBJ = $(OBJ_DIR)/main.o $(OBJ_DIR)/graphs.o

$(NAME):$(OBJ) 
	$(CC) -o $@ $^ -lplot -lpng

$(OBJ_DIR)/main.o:main.c
	$(CC) -c -o $@ $(FLAGS) $(WARNINGS) $<

$(OBJ_DIR)/graphs.o:graphs.c
	$(CC) -c -o $@ $(FLAGS) $(WARNINGS) $<

.PHONY: clean
clean:
	$(RM) $(NAME)
