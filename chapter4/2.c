#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "../helpers/helper.h"
/*
*Напишите программу, похожую на cp, которая при использовании для копирования
обычного файла, содержащего дыры (последовательности нулевых байтов), будет
также создавать соответствующие дыры в целевом файле.
*/

#define HELP_MSG "Usage: %s srcfile destfile"

int main(int argc, char *argv[]) {
    struct stat statbuf;
    int srcfd, destfd;
    char *buf = NULL;

    if (argc != 3 || argv[1] == "-h") {
        printf(HELP_MSG, argv[0]);
        exit(EXIT_FAILURE);
    }

    if (stat(argv[1], &statbuf) == -1) 
        PERR_EXIT("Stst error");        
    
    // opening
    if ((srcfd = open(argv[1], O_RDONLY)) == -1) 
        PERR_EXIT("srcfd");

    if ((destfd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 
                       S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)) == -1) 
        PERR_EXIT("destfd");
    
    //copying
    if((buf = malloc(sizeof(char) * statbuf.st_size)) == NULL) 
        PERR_EXIT("malloc");
    // reading
    if (read(srcfd, buf, statbuf.st_size) == -1) 
        PERR_EXIT("read");
    // writing
    if (write(destfd, buf, statbuf.st_size) == -1) 
        PERR_EXIT("write");
    // closing
    if (close(srcfd) == -1) 
        PERR_EXIT("close srcfd");
    if (close(destfd) == -1) 
        PERR_EXIT("close destfd");

    exit(EXIT_SUCCESS);
}