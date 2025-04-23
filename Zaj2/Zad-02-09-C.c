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
        printf("Bledna liczba argumentow (oczekiwano 2)\n");
        return 1;
    }

    int sock;
    int rc;         // "rc" to skrót słów "result code"
    ssize_t cnt;    // wyniki zwracane przez read() i write() są tego typu
    
    int port;
    sscanf(argv[2], "%d", &port);
    

    sock = socket(AF_INET, SOCK_DGRAM, 0);
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

    unsigned char buf[64];

    cnt = sendto(sock, buf, 0, 0, (struct sockaddr *) &addr, sizeof(addr));
    if (cnt == -1) {
        perror("sendto");
        return 1;
    }
    //printf("empty datagram sent\n");
    //scanf("%s", buf);

    do {
        cnt = recvfrom(sock, buf, 64, 0, NULL, NULL);
        if (cnt == -1) {
            //perror("read");
            return 1;
        }
        unsigned char *c = buf;

        printf("read %zd bytes\n", cnt);
        
        while (c < buf+cnt) {
            //printf("debug - analyzing character (%c)\n", *c);
            if(is_trusted_char(c)) {
                printf("%c", *c);
            }
            c++;
            //printf("moving on\n");
        }
        printf("debug - done analyzing\n");
    } while (cnt > 0);

    printf("Done receiving\n");

    rc = close(sock);
    if (rc == -1) {
        perror("close");
        return 1;
    }

    return 0;
}
