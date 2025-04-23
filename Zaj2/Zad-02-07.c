#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdbool.h>

bool is_trusted_char(unsigned char *c) {
    return (((*c >= 32) && (*c <= 126)) || (*c == '\n') || (*c == '\r') || (*c == '\t'));
}

int main(int argc, char *argv[])
{
    if(argc != 3) {
        printf("Bledna liczba argumentow (oczekiwano 2)");
        return 1;
    }

    int sock;
    int rc;         // "rc" to skrót słów "result code"
    ssize_t cnt;    // wyniki zwracane przez read() i write() są tego typu
    
    int port;
    sscanf(argv[2], "%d", &port);
    

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("socket");
        return 1;
    }

    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_addr = { .s_addr = htonl(0x7F000001) },   // 127.0.0.1
        .sin_port = htons(port)
    };

    inet_pton(AF_INET, argv[1], &(addr.sin_addr));

    rc = connect(sock, (struct sockaddr *) & addr, sizeof(addr));
    if (rc == -1) {
        perror("connect");
        return 1;
    }

    //printf("connected\n");

    unsigned char buf[16];

    do {
        cnt = read(sock, buf, 16);
        if (cnt == -1) {
            perror("read");
            return 1;
        }
        unsigned char *c = buf;
        //printf("read %zd bytes\n", cnt);
        while (c < buf+cnt) {
            //printf("debug - analyzing character (%c)\n", *c);
            if(is_trusted_char(c)) {
                printf("%c", *c);
            }
            c++;
        }
    } while (cnt != 0);

    rc = close(sock);
    if (rc == -1) {
        perror("close");
        return 1;
    }

    return 0;
}
