#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
/*
* Команда tee считывает свой стандартный ввод, пока ей не встретится символ конца
файла, записывает копию своего ввода на стандартное устройство вывода и в файл,
указанный в аргументе ее командной строки. (Пример использования этой команды
будет показан при рассмотрении FIFO-устройств в разделе 44.7.) Реализуйте tee, ис-
пользуя системные вызовы ввода-вывода. По умолчанию tee перезаписывает любой
существующий файл с заданным именем. Укажите ключ командной строки –a (tee
–a file), который заставит tee добавлять текст к концу уже существующего файла.
*/
#define HELP_MSG "Usage: tee {-a} filename\n"
#define MAX_BUF 1024

int main(int argc, char *argv[]) {        
    int fd;
    int openFlag = O_WRONLY | O_CREAT;
    mode_t fmode = S_IRWXU | S_IRWXG;
    char buf[MAX_BUF];
    char *fname;
    int bytesWritten = 0;
    // check for errors
    if(argc > 3 || argc == 1 || strcmp(argv[1], "-h") == 0) {
        printf("%s\n", HELP_MSG);
        exit(EXIT_FAILURE);
    }
    // get mode
    if(argc == 3){
        if(strcmp(argv[1], "-a") != 0) {
            printf("%s\n", HELP_MSG);
            exit(EXIT_FAILURE);
        } else {
            openFlag |= O_APPEND;
            fname = argv[2];
        }
    } else {
        openFlag |= O_TRUNC;
        fname = argv[1];
    }
    // open fd
    if((fd = open(fname, openFlag, fmode)) == -1) {
        perror("Error during openning fd");
        exit(EXIT_FAILURE);
    }
    // read
    if((bytesWritten = read(STDIN_FILENO, buf, MAX_BUF)) == -1 && errno != 0){
        perror("Error during read");
        exit(EXIT_FAILURE);
    }
    // write
    if(write(fd, buf, bytesWritten) != bytesWritten) {
        perror("Cannot write whole buffer");
        exit(EXIT_FAILURE);
    }
    // close fd
    if(close(fd) == -1) {
        perror("Error during closing fd");
        exit(EXIT_FAILURE);
    }

    printf("It's fine\n");
    exit(EXIT_SUCCESS);
}
