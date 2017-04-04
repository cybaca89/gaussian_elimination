# parallel hw3
NAME = aa
CFLAGS = -std=c99 -g -Wall -Wshadow -Wextra -Werror -W -pedantic -Wundef\
-Wpointer-arith -Wwrite-strings -Wconversion -Wunreachable-code
IINC := -openmp
GINC := -fopenmp

$(NAME):main.c
	gcc  -o $@ $(CFLAGS) $<
	@echo "COMPILE SUCCESS!!\n"

gauss:main.c
	gcc  -o $(NAME) $(CFLAGS) $< -DNDEBUG
	@echo "COMPILE SUCCESS!!\n"

#$(NAME):main.c
#	gcc  -o $@ $(CFLAGS) $< -D CACHE_LINE_SIZE=$(shell getconf LEVEL1_DCACHE_LINESIZE)
#	@echo "compile success!!\n"

asm.s:main.c
	mpicc -S -fverbose-asm -g -o asm.s $(CFLAGS) $< -D CACHE_LINE_SIZE=$(shell getconf LEVEL1_DCACHE_LINESIZE)
	@echo "compile success!!\n"

asmO2.s:main.c
	mpicc -S -fverbose-asm -g -O2 -o asmO2.s $(CFLAGS) $< -D CACHE_LINE_SIZE=$(shell getconf LEVEL1_DCACHE_LINESIZE)
	@echo "compile success!!\n"

.PHONY: clean
clean:
	$(RM) $(NAME) *.o *.s
