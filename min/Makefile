NAME = aa
CC = gcc
FLAGS = -std=c99 -pedantic
PPFLAGS = -Wl,--build-id=sha1
WARNINGS := -Wall -Wshadow -Wextra -Werror -Wundef -Wpointer-arith\
-Wwrite-strings -Wconversion -Wunreachable-code

$(NAME): main.c
	$(CC) -o $@ $(FLAGS) $(WARNINGS) $< -lm

.PHONY: clean
clean:
	$(RM) $(NAME) *.o *.s
