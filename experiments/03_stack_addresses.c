#include <stdio.h>

void set_secret(void) {
    int secret = 211;
    printf("secret lives at %p\n", (void*)&secret);
}

void peek(void) {
    int x;
    printf("x lives at      %p\n", (void*)&x);
    printf("%d\n", x);
}

int main(void) {
    set_secret();
    peek();
    return 0;
}