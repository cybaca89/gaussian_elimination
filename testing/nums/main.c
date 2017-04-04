#include <stdio.h>

int
main()
{
    int len = 20 * 21;
    int i;
    for (i = 0; i < 10; ++i) {
        if (i % 21 == 0)
            printf("\n");
        printf("   %i", i);
    }
    for (; i < 100; ++i) {
        if (i % 21 == 0)
            printf("\n");
        printf("  %i", i);
    }
    for (; i < len; ++i) {
        if (i % 21 == 0)
            printf("\n");
        printf(" %i", i);
    }
    return 0;
}
        
