#include <stdio.h>
#include <string.h>

void check_login(void) {
    char password[64];
    strcpy(password, "hunter2-my-real-password");
    /* pretend we compare it against the stored password here */
}

void send_reply(void) {
    char reply[64];
    strcpy(reply, "OK");
    /* sends all 64 bytes of the buffer, but only 3 were ever written */
    fwrite(reply, 1, strlen(reply), stdout);
    printf("\n");
}

int main(void) {
    check_login();
    send_reply();
    return 0;
}