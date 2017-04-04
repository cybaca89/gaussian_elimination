#SRC = $(wildcard *.c)
#OBJ = $(SRC:%.cpp=%.o)
NAME = aa
CC = gcc
FLAGS = -std=gnu99 -g -pedantic
PPFLAGS = -Wl,--build-id=sha1
WARNINGS := -Wall -Wshadow -Wextra -Werror -Wundef -Wpointer-arith\
-Wcast-align -Wwrite-strings -Wconversion -Wunreachable-code

$(NAME): main.c
	$(CC) -o $@ $(FLAGS) $(WARNINGS) $<

#%.o : %.cpp
#	$(CC) $(FLAGS) $(WARNINGS) -c $< -o $@

asm.s:main.c
	$(CC) -S -fverbose-asm -o $@ $(FLAGS) $<

.PHONY: clean
clean:
	$(RM) $(NAME) *.o *.s
