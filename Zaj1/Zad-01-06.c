#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char* argv[]) {

    unsigned int BUF_SIZE = 255;

    if (argc != 3) {
        return -1;
    }

    int file_src = open(argv[1], O_RDONLY);
    if (file_src == -1) {
        perror("Error while opening source file");
        return -1;
    }

    int file_tg = open(argv[2], O_RDWR | O_TRUNC | O_CREAT);
    if (file_tg == -1) {
        perror("Error while opening target file");
        return -1;
    }

    char buf[BUF_SIZE + 1];
    int e_r = 0, e_w = 0;

    do {
        e_r = read(file_src, buf, BUF_SIZE);
        if (e_r == -1) {
            perror("Error reading from source file");
            return -1;
        }
        e_w = write(file_tg, buf, e_r);
        if (e_w == -1) {
            perror("Error writing to target file");
            return -1;
        }
    } while (e_r == BUF_SIZE);

    if (close(file_src) == -1) {
        perror("Error closing source file");
    }
    if (close(file_tg) == -1) {
        perror("Error closing target file");
    }

    return 0;
}