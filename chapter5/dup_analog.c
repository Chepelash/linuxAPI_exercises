#include <stdlib.h> 
#include <stdio.h>  
#include <sys/stat.h> 
#include <fcntl.h> 
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "../helpers/helper.h"

/*
Реализуйте функции dup() и dup2(), используя функцию fcntl() и, там где это не-
обходимо, функцию close(). (Тот факт, что dup2() и fcntl() в некоторых случаях
возникновения ошибок возвращают различные значения errno, можно проигнориро-
вать.) Для dup2() не забудьте учесть особый случай, когда oldfd равен newfd. В этом
случае нужно проверить допустимость значения oldfd, что можно сделать, к примеру,
проверкой успешности выполнения вызова fcntl(oldfd, F_GETFL). Если значение
oldfd недопустимо, функция должна возвратить –1, а значение errno должно быть
установлено в EBADF.
*/

#define HELP_MSG "Usage: %s {dup|dup2} oldfd [newfd]\n"

int dup_analog(int oldfd) {
    return fcntl(oldfd, F_DUPFD, 0);
}

int dup2_analog(int oldfd, int newfd) {
    int genfd = -1;
    
    if (oldfd == newfd) {
        if (fcntl(oldfd, F_GETFL) == -1) {
            errno = EBADF;
            return -1;
        }
        genfd = oldfd;        
    } else {
        close(newfd);
        errno = 0;
        if ((genfd = fcntl(oldfd, F_DUPFD, newfd)) == -1)
            return -1;
    }
    return genfd;
}



int main(int argc, char *argv[]) {
    int genfd, oldfd, newfd;
    char *end;
    if (argc < 3 || argc > 4 || argv[1] == "-h") 
        UERR_EXIT(HELP_MSG, argv[0]);
    
    oldfd = strtol(argv[2], &end, 10);
    if (*end != '\0')
        PERR_EXIT("Cannot get oldfd");
    
    if (argc == 4) {
        newfd = strtol(argv[3], &end, 10);
        if (*end != '\0')
            PERR_EXIT("Cannot get newfd");
    }

    if (strcmp(argv[1], "dup") == 0) {
        if ((genfd = dup_analog(oldfd)) == -1)
            PERR_EXIT("Dup error");
    } else if (strcmp(argv[1], "dup2") == 0) {
        if (argc != 4)
            UERR_EXIT(HELP_MSG, argv[0]);
        if ((genfd = dup2_analog(oldfd, newfd)) == -1)
            PERR_EXIT("Dup2 error");
    }
    printf("Oldfd: %d; newfd: %d; genfd: %d\n", oldfd, newfd, genfd);
    printf("It's fine\n");
    exit(EXIT_SUCCESS);
}
