#include <stdio.h>
#include <string.h>

int check_login(const char* typed) {
    char password[64];
    strcpy(password, typed); /* keep a working copy */
    int ok = (strcmp(password, "letmein") == 0);
    explicit_bzero(password, sizeof(password));
    return ok;
}

void send_reply(void) {
    char reply[64];
    strcpy(reply, "OK");
    fwrite(reply, 1, sizeof(reply), stdout);
    printf("\n");
}

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("usage: ./prog <password>\n");
        return 1;
    }
    check_login(argv[1]);
    send_reply();
    return 0;
}