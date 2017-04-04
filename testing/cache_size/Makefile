NAME = cache_info
CC = gcc
CPPFLAGS = -std=c99 -g
WARNINGS := -Werror -Wall -Wshadow -Wextra\
-Wconversion -Wunreachable-code -Wundef -pedantic

$(NAME):main.c
	$(CC) $(CPPFLAGS) $(WARNINGS) -o $@ $<
	@echo READY FOR ACTION

.PHONY: clean
clean:
	$(RM) $(NAME)
