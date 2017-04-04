#CSCI 551 Parallel Computing
# HW1 Trapezoidal Interpolation
NAME = aa

CC = gcc
LIBS = -lm -lquadmath
CPPFLAGS = -std=gnu99 -g
WARNINGS = -Werror -Wall -Wshadow -Wextra -Wconversion -Wunreachable-code -Wundef -pedantic
#OBJ := $(patsubst %.c,%.o,$(wildcard *.c))
#TRACE args will spit out (!machine code(machine code is lowest level, below assebly, strictly numerical)) assembly instructions
#TRACE = -Wl,--trace

$(NAME):main.c
	$(CC) -o $@ $(CPPFLAGS) $(WARNINGS) $(LIBS) $<

#%.o:%.c
	#$(CC) $(CPPFLAGS) -c $< -o $@

clean:
	$(RM) $(NAME)

#### -Wall contains:
# Waddress, Wbool-compare, Wc++11-compat, Wc++14-compat Wchar-subscriptsa,
# Wcomment, Wduplicate-decl-specifier, Wenum-compare, Wformat,
# Wint-in-bool-context Wimplicit Wimplicit-int, Wimplicit-function-declaration,
# Wlogical-not-parentheses, Wmain, Wmaybe-uninitialized,
# Wmemset-elt-size, Wmemset-transposed-args, Wmisleading-indentation,
# Wmissing-braces, Wnonnull Wnonnull-compare, Wopenmp-simd, Wparentheses,
# Wpointer-sign, Wreorder, Wreturn-type, Wsequence-point,
# Wsizeof-pointer-memaccess, Wstrict-aliasing, Wstrict-overflow=1, Wswitch,
# Wtautological-compare, Wtrigraphs, Wuninitialized, Wunknown-pragmas,
# Wunused-function, Wunused-label, Wunused-value, Wunused-variable,
# Wvolatile-register-var

#### -Wextra contains:
# Wclobbered, Wempty-body, Wignored-qualifiers, Wmissing-field-initializers,
# Wmissing-parameter-type, Wold-style-declaration,
# Wtype-limits, Wuninitialized, Wshift-negative-value, Wunused-parameter,
# Wunused-but-set-parameter, Woverride-init, Wsign-compare,


# enalbled by default, check for security
#   -fshort-enums
#   -fsplit-ivs-unroller
#   -ftrapping-math
