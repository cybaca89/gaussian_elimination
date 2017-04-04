# parallel hw3
NAME = aa
CFLAGS = -std=c99
WARNINGS = -Wall -Wshadow -Wextra -Werror -W -pedantic -Wundef \
-Wpointer-arith -Wwrite-strings -Wconversion -Wunreachable-code

S_DIR = gauss/serial
P_DIR = gauss/parallel

OBJ_INTEL = obj/main_i.o obj/g_back_substitution_i.o\
obj/g_echelon_i.o obj/output_i.o obj/gauss_i.o\
obj/g_l2_norm_i.o obj/memory_i.o

OBJ_PARALLEL = obj/main_p.o obj/g_back_substitution_p.o\
obj/g_echelon_p.o obj/output_p.o obj/gauss_p.o\
obj/g_l2_norm_p.o obj/memory_p.o

OBJ_DEBUG = obj/memory_d.o obj/main_d.o obj/g_back_substitution_d.o\
obj/g_echelon_d.o obj/output_d.o obj/gauss_d.o\
obj/g_l2_norm_d.o

$(NAME):$(OBJ_DEBUG)
	gcc -o $@ $^ -lm -fopenmp
	@echo "COMPILE SUCCESS!!\n"

parallel:$(OBJ_PARALLEL)
	gcc $(WARNINGS) -o $(NAME) $^ -lm -fopenmp
	@echo "COMPILE SUCCESS!!\n"

serial:serial.c
	gcc -o $(NAME) $< -lm
	@echo "COMPILE SUCCESS!!\n"

intel:$(OBJ_INTEL)
	icc $(WARNINGS) -o $(NAME) $^ -lm -qopenmp
	@echo "COMPILE SUCCESS!!\n"





### INTEL   #################################### INTEL
#
obj/main_i.o:main.c
	icc -c -o $@ $(CFLAGS) $< -DNDEBUG

obj/g_back_substitution_i.o:$(P_DIR)/g_back_substitution.c
	icc -c -o $@ $(CFLAGS) $< -qopenmp -DNDEBUG

obj/g_echelon_i.o:$(P_DIR)/g_echelon.c
	icc -c -o $@ $(CFLAGS) $< -qopenmp -DNDEBUG

obj/output_i.o:$(P_DIR)/output.c
	icc -c -o $@ $(CFLAGS) $< -DNDEBUG

obj/gauss_i.o:$(P_DIR)/gauss.c
	icc -c -o $@ $(CFLAGS) $< -qopenmp -DNDEBUG

obj/g_l2_norm_i.o:$(P_DIR)/g_l2_norm.c
	icc -c -o $@ $(CFLAGS) $< -qopenmp -DNDEBUG

obj/memory_i.o:$(P_DIR)/memory.c
	icc -c -o $@ $(CFLAGS) $< -qopenmp





### PARALLEL################################ PARALLEL
#
obj/main_p.o:main.c
	gcc -c -o $@ $(CFLAGS) $< -DNDEBUG

obj/g_back_substitution_p.o:$(P_DIR)/g_back_substitution.c
	gcc -c -o $@ $(CFLAGS) $< -fopenmp -DNDEBUG

obj/g_echelon_p.o:$(P_DIR)/g_echelon.c
	gcc -c -o $@ $(CFLAGS) $< -fopenmp -DNDEBUG

obj/output_p.o:$(P_DIR)/output.c
	gcc -c -o $@ $(CFLAGS) $< -DNDEBUG

obj/gauss_p.o:$(P_DIR)/gauss.c
	gcc -c -o $@ $(CFLAGS) $< -fopenmp -DNDEBUG

obj/g_l2_norm_p.o:$(P_DIR)/g_l2_norm.c
	gcc -c -o $@ $(CFLAGS) $< -fopenmp -DNDEBUG

obj/memory_p.o:$(P_DIR)/memory.c
	gcc -c -o $@ $(CFLAGS) $< -fopenmp



### PARALLEL DEBUG ############################# DEBUG
#
obj/main_d.o:main.c
	gcc -c -o $@ $(CFLAGS) $<

obj/g_back_substitution_d.o:$(P_DIR)/g_back_substitution.c
	gcc -c -o $@ $(CFLAGS) $< -fopenmp

obj/g_echelon_d.o:$(P_DIR)/g_echelon.c
	gcc -c -o $@ $(CFLAGS) $< -fopenmp

obj/output_d.o:$(P_DIR)/output.c
	gcc -c -o $@ $(CFLAGS) $<

obj/gauss_d.o:$(P_DIR)/gauss.c
	gcc -g -c -o $@ $(CFLAGS) $< -fopenmp

obj/g_l2_norm_d.o:$(P_DIR)/g_l2_norm.c
	gcc -c -o $@ $(CFLAGS) $< -fopenmp

obj/memory_d.o:$(P_DIR)/memory.c
	gcc -c -o $@ $(CFLAGS) $< -fopenmp


#obj/gauss_d.o:$(P_DIR)/gauss.c
#	gcc -O0 -c -o $@ $(CFLAGS) $< -fopenmp

.PHONY: clean
clean:
	$(RM) $(NAME) *.o *.s obj/*.o
