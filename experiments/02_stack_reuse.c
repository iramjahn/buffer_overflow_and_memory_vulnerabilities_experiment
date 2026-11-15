#include <stdio.h>

void set_secret(void) { int secret = 211; }

void peek(void) {
    int x;
    printf("%d\n", x);
}

int main(void) {
    set_secret();
    peek();
    return 0;
}