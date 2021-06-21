
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>

#define HELP_MSG "%s file {r<length>|R<length>|w<string>|s<offset>}...\n"


int main(int argc, char *argv[]) {
    size_t len;
    off_t offset;
    int fd, ap, j;
    char *buf;
    ssize_t numRead, numWritten;
    char *end;

    if (argc < 3 || strcmp(argv[1], "--help") == 0) {
        printf(HELP_MSG, argv[0]);
        exit(EXIT_FAILURE);
    }

    fd = open(argv[1], O_RDWR | O_CREAT,
              S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    if (fd == -1) {
        perror("Cannot open file");
        exit(EXIT_FAILURE);
    }

    len = strtol(argv[2], &end, 10);
    if (*end != '\0') {
        perror("cannot get offset");
        exit(EXIT_FAILURE);
    }
    if(lseek(fd, len, SEEK_CUR) == -1) {
        perror("Error lseek");
        exit(EXIT_FAILURE);
    }
    if(write(fd, argv[3], strlen(argv[3])) == -1) {
        perror("Error writing");
        exit(EXIT_FAILURE);
    }
    if (close(fd) == -1) {
        perror("Error closing fd");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}