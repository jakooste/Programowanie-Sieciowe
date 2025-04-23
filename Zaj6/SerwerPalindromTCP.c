#define _GNU_SOURCE

#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <sys/epoll.h>
#include <sys/syscall.h>

// Pomocnicze

int listening_socket_tcp_ipv4(in_port_t port)
{
    int s;

    if ((s = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        return -1;
    }

    struct sockaddr_in a = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = htonl(INADDR_ANY),
        .sin_port = htons(port)
    };

    if (bind(s, (struct sockaddr *) &a, sizeof(a)) == -1) {
        perror("bind");
        goto close_and_fail;
    }

    if (listen(s, 10) == -1) {
        perror("listen");
        goto close_and_fail;
    }

    return s;

close_and_fail:
    close(s);
    return -1;
}

void log_printf(const char * fmt, ...)
{
    // bufor na przyrostowo budowany komunikat, len mówi ile już znaków ma
    char buf[1024];
    int len = 0;

    struct timespec date_unix;
    struct tm date_human;
    long int usec;
    if (clock_gettime(CLOCK_REALTIME, &date_unix) == -1) {
        perror("clock_gettime");
        return;
    }
    if (localtime_r(&date_unix.tv_sec, &date_human) == NULL) {
        perror("localtime_r");
        return;
    }
    usec = date_unix.tv_nsec / 1000;

    // getpid() i gettid() zawsze wykonują się bezbłędnie
    pid_t pid = getpid();
    pid_t tid = syscall(SYS_gettid);

    len = snprintf(buf, sizeof(buf), "%02i:%02i:%02i.%06li PID=%ji TID=%ji ",
                date_human.tm_hour, date_human.tm_min, date_human.tm_sec,
                usec, (intmax_t) pid, (intmax_t) tid);
    if (len < 0) {
        return;
    }

    va_list ap;
    va_start(ap, fmt);
    int i = vsnprintf(buf + len, sizeof(buf) - len, fmt, ap);
    va_end(ap);
    if (i < 0) {
        return;
    }
    len += i;

    // zamień \0 kończące łańcuch na \n i wyślij całość na stdout, czyli na
    // deskryptor nr 1; bez obsługi błędów aby nie komplikować przykładu
    buf[len] = '\n';
    write(1, buf, len + 1);
}

void log_perror(const char * msg)
{
    log_printf("%s: %s", msg, strerror(errno));
}

void log_error(const char * msg, int errnum)
{
    log_printf("%s: %s", msg, strerror(errnum));
}

int accept_verbose(int srv_sock)
{
    struct sockaddr_in a;
    socklen_t a_len = sizeof(a);

    log_printf("calling accept() on descriptor %i", srv_sock);
    int rv = accept(srv_sock, (struct sockaddr *) &a, &a_len);
    if (rv == -1) {
        log_perror("accept");
    } else {
        char buf[INET_ADDRSTRLEN];
        if (inet_ntop(AF_INET, &a.sin_addr, buf, sizeof(buf)) == NULL) {
            log_perror("inet_ntop");
            strcpy(buf, "???");
        }
        log_printf("new client %s:%u, new descriptor %i",
                        buf, (unsigned int) ntohs(a.sin_port), rv);
    }
    return rv;
}

ssize_t read_verbose(int fd, void * buf, size_t nbytes)
{
    log_printf("calling read() on descriptor %i", fd);
    ssize_t rv = read(fd, buf, nbytes);
    if (rv == -1) {
        log_perror("read");
    } else {
        log_printf("received %zi bytes on descriptor %i", rv, fd);
    }
    return rv;
}

ssize_t write_verbose(int fd, void * buf, size_t nbytes)
{
    log_printf("calling write() on descriptor %i", fd);
    ssize_t rv = write(fd, buf, nbytes);
    if (rv == -1) {
        log_perror("write");
    } else if (rv < nbytes) {
        log_printf("partial write on %i, wrote only %zi of %zu bytes",
                        fd, rv, nbytes);
    } else {
        log_printf("wrote %zi bytes on descriptor %i", rv, fd);
    }
    return rv;
}

int close_verbose(int fd)
{
    log_printf("closing descriptor %i", fd);
    int rv = close(fd);
    if (rv == -1) {
        log_perror("close");
    }
    return rv;
}

// Przetwarzające

bool is_alpha(const char *c) {
    return (((*c >= 'a') && (*c <= 'z')) || ((*c >= 'A') && (*c <= 'Z')));
}

char to_lowercase(const char *c) {
    if ((*c >= 'A') && (*c <= 'Z')) {
        return *c - 'A' + 'a';
    }
    else return *c;
}

int palindrom(const char *buf, int buf_len) {
    int i = 0;
    int rv = 1;
    while(i < (buf_len - 1 - i)) {
        if(buf[i] != buf[buf_len - 1 - i]) {
            rv = 0;
            break;
        }
        ++i;
    }
    return rv;
}

void * client_thread(void * arg)
{
    log_printf("thread started");
    int s = *( (int *) arg );

    int in_buf_size = 4096;
    int word_maxlen = 4096;

    // setup automat
    char in_buf[in_buf_size];
    char *curr = in_buf;
    ssize_t bytes_read = 0;
    char word_buf[word_maxlen];
    int word_len;
    bool err;
    int wc;
    int pc;

    int state = 0;
    /*
    STANY AUTOMATU:
    0 - początkowy
    1 - w środku słowa
    2 - wczytano spację
    3 - wczytano /r
    4 - wystąpił błąd w zapytaniu
    */

    while(true) { // jedno zapytanie
        
        // reset automat
        err = false;
        wc = 0;
        pc = 0;
        word_len = 0;
        state = 0;
        
        while(true) { // wczytuj po jednym
            
            if(curr >= in_buf+bytes_read) {
                bytes_read = read_verbose(s, in_buf, in_buf_size);
                if(bytes_read <= 0) {
                    goto cleanup_thread;
                }
                curr = in_buf;
            }

            //log_printf("state: %d; reading byte: (%c)", state, *curr);

            if(state == 0) {
                if(is_alpha(curr)) {
                    if(word_len < word_maxlen) {
                        word_buf[word_len++] = to_lowercase(curr);
                        state = 1;
                    }
                    else {
                        err = true;
                        state = 4;
                    }
                }
                else if(*curr == '\r') {
                    state = 3;
                }
                else {
                    err = true;
                    state = 4;
                }
            }
            else if(state == 1) {
                if(is_alpha(curr)) {
                    if(word_len < word_maxlen) {
                        word_buf[word_len++] = to_lowercase(curr);
                        state = 1;
                    }
                    else {
                        err = true;
                        state = 4;
                    }
                }
                else if(*curr == ' ') {
                    wc += 1;
                    pc += palindrom(word_buf, word_len);
                    word_len = 0;
                    state = 2;
                }
                else if(*curr == '\r') {
                    state = 3;
                }
                else {
                    err = true;
                    state = 4;
                }
            }
            else if(state == 2) {
                if(is_alpha(curr)) {
                    if(word_len < word_maxlen) {
                        word_buf[word_len++] = to_lowercase(curr);
                        state = 1;
                    }
                    else {
                        err = true;
                        state = 4;
                    }
                }
                else if(*curr == '\r') {
                    err = true;
                    state = 3;
                }
                else {
                    err = true;
                    state = 4;
                }
            }
            else if(state == 3) {
                if(*curr == '\n') {
                    if(!err && word_len > 0) {
                        wc += 1;
                        pc += palindrom(word_buf, word_len);
                        word_len = 0;
                    }
                    ++curr;
                    break;
                }
                else if(*curr == '\r') {
                    err = true;
                    state = 3;
                }
                else {
                    err = true;
                    state = 4;
                }
            }
            else if(state == 4) {
                if(*curr == '\r') {
                    state = 3;
                }
            }
            else {
                log_printf("error in state machine, aborting");
                goto cleanup_thread;
            }
            
            ++curr;
        }

        int resp_len = sprintf(word_buf, (err ? "ERROR\r\n" : "%d/%d\r\n"), pc, wc);

        ssize_t bytes_sent = write_verbose(s, word_buf, resp_len);
        if(bytes_sent < 0) {
            goto cleanup_thread;
        }

        //log_printf("request processed, reply sent\n");
    }

cleanup_thread:

    close_verbose(s);

    log_printf("thread exits");
    free(arg);
    return NULL;
}

void multithreaded_loop(int srv_sock)
{
    pthread_attr_t attr;
    int errnum;

    errnum = pthread_attr_init(&attr);
    if (errnum != 0) {
        log_error("pthread_attr_init", errnum);
        return;
    }
    errnum = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    if (errnum != 0) {
        log_error("pthread_attr_setdetachstate", errnum);
        goto cleanup_attr;
    }

    while (true) {
        int s = accept_verbose(srv_sock);
        if (s == -1) {
            break;
        }

        int * p = (int *) malloc(sizeof(int));
        if (p == NULL) {
            log_printf("cannot allocate memory for thread argument");
            goto cleanup_sock;
        }
        *p = s;

        log_printf("attempting to create a thread");
        pthread_t thr;
        errnum = pthread_create(&thr, &attr, client_thread, p);
        if (errnum != 0) {
            log_error("pthread_create", errnum);
            goto cleanup_sock;
        }

        continue;

cleanup_sock:
        close_verbose(s);
    }

cleanup_attr:
    errnum = pthread_attr_destroy(&attr);
    if (errnum != 0) {
        log_error("pthread_attr_destroy", errnum);
    }
}

// MAIN

int main(int argc, char * argv[])
{
    long int srv_port = 2020;
    int srv_sock;

    // Stwórz gniazdko i uruchom pętlę odbierającą przychodzące połączenia.

    if ((srv_sock = listening_socket_tcp_ipv4(srv_port)) == -1) {
        goto fail;
    }

    log_printf("starting main loop");
    multithreaded_loop(srv_sock);
    log_printf("main loop done");

    if (close(srv_sock) == -1) {
        log_perror("close");
        goto fail;
    }

    return 0;

fail:
    return 1;
}
