#define _POSIX_C_SOURCE 200809L
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

char ascii_to_letter(const char *c) {
    if((*c >= 65) && (*c <= 90)) {
        return *c;
    }
    if ((*c >= 97) && (*c <= 122)) {
        return *c - 97 + 65;
    }
    if (*c == ' ') {return 0;}
    return -1;
}

bool is_palindrome(const char *first, const char *last) {
    int i = 0;
    while(first+i < last-i) {
        if(*(first+i) != *(last-i)) {
            return false;
        }
        i++;
    }
    return true;
}

int number_of_palindromes(char* buf, int buf_len) {
    bool in_word = false;
    bool flag_rn = false;
    char *first, *last, *curr = buf;
    int result_palindromes = 0, result_words = 0;

    if((buf_len >= 2) && (*(buf+buf_len-2) == '\r') && (*(buf+buf_len-1) == '\n')) {
        flag_rn = true;
        buf_len-=2;
        printf("Detected RN\n");
    }
    else {
        if((buf_len >= 1) && (*(buf+buf_len-1) == '\n')) {
            flag_rn = true;
            buf_len-=1;
            printf("Detected N\n");
        }
    }

    if(buf_len == 0) {
        return sprintf(buf, (flag_rn ? "%d/%d\r\n" : "%d/%d"), result_palindromes, result_words);
    }

    while (curr<buf+buf_len) {
        *curr = ascii_to_letter(curr);
        if(*curr == -1) {
            printf("ERROR - unaccepted character\n");
            return sprintf(buf, "ERROR");
        }
        if (in_word) {
            if (*curr == 0) {
                last = curr - 1;
                result_palindromes += is_palindrome(first, last);
                result_words += 1;
                in_word = false;
            }
        }
        else {
            if (*curr == 0) {
                printf("ERROR - too many spaces between words\n");
                return sprintf(buf, "ERROR");
            }
            else {
                first = curr;
                in_word = true;
            }
        }
        curr++;
    }
    if (in_word) {
        last = curr - 1;
        result_palindromes += is_palindrome(first, last);
        result_words += 1;
        in_word = false;
    }
    else {
        printf("ERROR - trailing space\n");
        return sprintf(buf, "ERROR");
    }

    return sprintf(buf, (flag_rn ? "%d/%d\r\n" : "%d/%d"), result_palindromes, result_words);
}

int main(int argc, char *argv[])
{
    int port = 2020;

    int sock;   // gniazdko
    int rc;         // "rc" to skrót słów "result code"
    ssize_t cnt;    // wyniki zwracane przez read() i write() są tego typu

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == -1) {
        perror("socket");
        return 1;
    }

    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_addr = { .s_addr = htonl(INADDR_ANY) },
        .sin_port = htons(port)
    };

    rc = bind(sock, (struct sockaddr *) & addr, sizeof(addr));
    if (rc == -1) {
        perror("bind");
        return 1;
    }

    bool keep_on_handling_clients = true;
    while (keep_on_handling_clients) {

        char buf[1024];
        struct sockaddr_in clnt_addr;
        socklen_t clnt_addr_len = sizeof(clnt_addr);

        printf("Listening...\n");

        cnt = recvfrom(sock, buf, 1024, 0, (struct sockaddr *) &clnt_addr, &clnt_addr_len);
        if (cnt == -1) {
            perror("recvfrom");
            return 1;
        }

        printf("received %zi bytes\n", cnt);

        cnt = number_of_palindromes(buf, cnt);

        cnt = sendto(sock, buf, cnt, 0, (struct sockaddr *) &clnt_addr, clnt_addr_len);
        if (cnt == -1) {
            perror("sendto");
            return 1;
        }
        printf("sent %zi bytes\n", cnt);
    }

    rc = close(sock);
    if (rc == -1) {
        perror("close");
        return 1;
    }

    return 0;
}
