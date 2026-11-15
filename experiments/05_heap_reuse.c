#include <stdio.h>
#include <stdlib.h>

void print_ints(const char* label, int* p, int n) {
    printf("%s at %p: ", label, (void*)p);
    for (int i = 0; i < n; i++) {
        printf("%d ", p[i]);
    }
    printf("\n");
}

int main(void) {
    int* fresh = malloc(8 * sizeof(int));
    print_ints("fresh ", fresh, 8);

    for (int i = 0; i < 8; i++) {
        fresh[i] = 211;
    }
    free(fresh);

    int* reused = malloc(8 * sizeof(int));
    print_ints("reused", reused, 8);

    int* zeroed = calloc(8, sizeof(int));
    print_ints("calloc", zeroed, 8);

    free(reused);
    free(zeroed);
    return 0;
}