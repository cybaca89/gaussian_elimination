/*
 * print out sizes of different c datatypes
 */

#include <quadmath.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

typedef struct float64 float64;
typedef struct float80 float80;
typedef struct type_tc type_tc;

void printSizes();

struct float64 {
    long unsigned int mantissa: 52;
    long unsigned int exponent: 11;
    long unsigned int sign_bit: 1;
};

struct float80 {
    long unsigned int mantissa: 63;
    long unsigned int state_bit: 1;
    long unsigned int exponent: 15;
    long unsigned int sign_bit: 1;
};

struct type_tc {
    short a; // 2 bytes
    char b;  // 1 byte
    char c;  // 1
    char d;  // 1
    char e;  // 1
    char f;  // 1
}; // if a is a char, sizeof says 6
   // if a is a short, sizeof says 8

int main()
{
    //__float128 dme; // double machine epsilon
    //quadmath_snprintf(hbuf, 128, "%.19Qe", h_quad);
    printSizes();
    printf("\n\n");
    struct _player {
        float *restrict a
            , *restrict b
            , *restrict c
            , *restrict d
            , *restrict e
            , *restrict f
            ;
    } player;

    printf(
    "    struct _player {  offsets:\n"
        "\tfloat *restrict a;        : %zd\n"
        "\tfloat *restrict b;        : %zd\n"
        "\tfloat *restrict c;        : %zd\n"
        "\tfloat *restrict d;        : %zd\n"
        "\tfloat *restrict e;        : %zd\n"
        "\tfloat *restrict f;        : %zd\n"
        "    }player;\n\n"
        ,offsetof(struct _player, a)
        ,offsetof(struct _player, b)
        ,offsetof(struct _player, c)
        ,offsetof(struct _player, d)
        ,offsetof(struct _player, e)
        ,offsetof(struct _player, f)
        );
    printf("    total size of struct _player: %zd\n", sizeof(struct _player));
    printf("    total size of struct player: %zd\n", sizeof(player));
    printf("    sum of sizes of individual types: %zd\n"
        ,
        + sizeof(player.a)
        + sizeof(player.b)
        + sizeof(player.c)
        + sizeof(player.d)
        + sizeof(player.e)
        + sizeof(player.f)
        );
    /*
    printf("\n\n");
    struct _player {
        float mat[16];
        float a[3]
            , b[3]
            , c[3]
            , d[3]
            , e[3]
            , f[3]
            ;
    } player = {
          { 33.363f }
        , { 1.0f, 2.0f, 3.0f } // a
        , { 4.0f, 5.0f, 6.0f } // b
        , { 7.0f, 8.0f, 9.0f } // c
        , { 10.0f, 11.0f, 12.0f } // d
        , { 13.0f, 14.0f, 15.0f } // e
        , { 16.0f, 17.0f, 18.0f } // f
    };

#   define VIEW_M 0
#   define POS 16
#   define FRONT 19
#   define RIGHT 22
#   define UP 25
#   define WORLD 28
#   define DATA 31
#   define X 0
#   define Y 1
#   define Z 2
    float *ptr = &player.mat[0];
    // printf("%g, %g, %g\n", ptr[X], ptr[Y], ptr[Z]);
    printf("%g\n", (ptr + POS)[X]);
    printf("%g\n", (ptr + FRONT)[X]);
    printf("%g\n", (ptr + RIGHT)[X]);
    printf("%g\n", (ptr + UP)[X]);
    printf("%g\n", (ptr + WORLD)[X]);
    printf("%g\n", (ptr + DATA)[X]);

    printf(
    "    struct _player {  offsets:\n"
        "\tfloat mat[16];     : %zd\n"
        "\tfloat a[3];        : %zd\n"
        "\tfloat b[3];        : %zd\n"
        "\tfloat c[3];        : %zd\n"
        "\tfloat d[3];        : %zd\n"
        "\tfloat e[3];        : %zd\n"
        "\tfloat f[3];        : %zd\n"
        "    }player;\n\n"
        ,offsetof(struct _player, mat)
        ,offsetof(struct _player, a)
        ,offsetof(struct _player, b)
        ,offsetof(struct _player, c)
        ,offsetof(struct _player, d)
        ,offsetof(struct _player, e)
        ,offsetof(struct _player, f)
        );
    printf("    total size of struct _player: %zd\n", sizeof(struct _player));
    printf("    total size of struct player: %zd\n", sizeof(player));
    printf("    sum of sizes of individual types: %zd\n"
        ,
        + sizeof(player.mat)
        + sizeof(player.a)
        + sizeof(player.b)
        + sizeof(player.c)
        + sizeof(player.d)
        + sizeof(player.e)
        + sizeof(player.f)
        );
    */

    /*
    printf("\n\n\n");
    struct s {
        int i;
        char *c;
        double d;
        char a[];
    };

    printf("    struct s {\n"
        "\tint i;        offset: %zd\n"
        "\tchar *c;      offset: %zd\n"
        "\tdouble d;     offset: %zd\n"
        "\tchar a[]      offset: %zd\n"
        "    };\n\n"
        ,offsetof(struct s, i)
        ,offsetof(struct s, c)
        ,offsetof(struct s, d)
        ,offsetof(struct s, a));
    printf("    total size of struct s: %zd\n", sizeof(struct s));
    printf("    sum of sizes of individual types: %zd\n"
        , sizeof(int) + sizeof(char*) + sizeof(double) + sizeof(char));
    */

    exit(EXIT_SUCCESS);
    //printf("offsets: i=%zd; c=%zd; d=%zd; a=%zd\n",
}

void printSizes()
{
    type_tc tc;
    tc.a = 1;
    tc.b = 1;
    tc.c = 1;
    tc.d = 1;
    tc.e = 1;
    tc.f = 1;
    printf("Register sizes of various data types:\n");
    const int CNT = 21;
    const long unsigned int sizes[][2] = {
        { sizeof(char),                   sizeof(char) * 8                   },
        { sizeof(unsigned char),          sizeof(unsigned char) * 8          },
        { sizeof(short),                  sizeof(short) * 8                  },
        { sizeof(unsigned short),         sizeof(unsigned short) * 8         },
        { sizeof(int),                    sizeof(int) * 8                    },
        { sizeof(unsigned int),           sizeof(unsigned int) * 8           },
        { sizeof(long int),               sizeof(long int) * 8               },
        { sizeof(long unsigned int),      sizeof(long unsigned int) * 8      },
        { sizeof(long long int),          sizeof(long long int) * 8          },
        { sizeof(long long unsigned int), sizeof(long long unsigned int) * 8 },
        { sizeof(size_t)                , sizeof(size_t) * 8                 },
        { sizeof(float),                  sizeof(float) * 8                  },
        { sizeof(double),                 sizeof(double) * 8                 },
        { sizeof(long double),            sizeof(long double) * 8            },
        { sizeof(__float128),             sizeof(__float128) * 8             },
        { sizeof(int *),                  sizeof(int *) * 8                  },
        { sizeof(void *),                 sizeof(void *) * 8                 },
        { sizeof(float64),                sizeof(float64) * 8                },
        { sizeof(float80),                sizeof(float80) * 8                },
        { sizeof(type_tc),                sizeof(type_tc) * 8                },
        { sizeof tc,                      (sizeof tc) * 8                    }
    }; // 21 entries
    const char *types[] = {
        "char",           "unsigned char",  // can be placed on any memory address  
        "short",          "unsigned short",// must start on an even address
        "int",            "unsigned int",   // must start on address divisible by 4  
        "long int",       "long unsigned int",// start on address divisible by 8 
        "long long int",  "long long unsigned int",// who fucking knows
        "size_t",
        "float",        // must start on address divisible by 4
        "double",       // "" 8
        "long double",  // 16
        "__float128",   // 16
        "int *",        // 8, pointers take priority over other types, aka, pointers
        "void *",       //    will always be ordered on the boundary before other types
        "float64",      // struct with bit fields, 64
        "float80",      // bit fields add up to 80, 128
        "type_tc",      // 6 bits
        "tc"
        }; // 21 total
    printf("\t%18sbytes bits\n", " ");
    printf("\t%18s----- -----\n", " ");
    int i;
    for (i = 0; i < CNT; i++)
        printf("%22s:%4lu %7lu\n", types[i], sizes[i][0], sizes[i][1]);
}
