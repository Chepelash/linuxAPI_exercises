#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include "../helpers/helper.h"

/*
Это упражнение демонстрирует необходимость атомарности, гарантированной при
открытии файла с флагом O_APPEND. Напишите программу, получающую до трех ар-
гументов командной строки:
$ atomic_append filename num-bytes [x]
Эта программа должна открыть файл с именем, указанным в аргументе filename
(создав его при необходимости), и дополнить его количеством байтов, заданным в ар-
гументе num-bytes, используя вызов write() для побайтовой записи. По умолчанию
программа должна открыть файл с флагом O_APPEND, но, если есть третий аргумент ко-
мандной строки (x), флаг O_APPEND должен быть опущен. При этом, вместо того чтобы
добавлять байты, программа должна выполнять перед каждым вызовом write() вызов
lseek(fd, 0, SEEK_END) . Запустите одновременно два экземпляра этой программы без
аргумента x для записи одного миллиона байтов в один и тот же файл:
$ atomic_append f1 1000000 & atomic_append f1 1000000
Повторите те же действия, ведя запись в другой файл, но на этот раз с указанием
аргумента x:
$ atomic_append f2 1000000 x & atomic_append f2 1000000 x
Выведите на экран размеры файлов f1 и f2 , воспользовавшись командой ls –l , и объ-
ясните разницу между ними.

*/

/*
O_APPEND
              The file is opened in append mode.  Before each write(2),
              the file offset is positioned at the end of the file, as
              if with lseek(2).  The modification of the file offset and
              the write operation are performed as a single atomic step.

              O_APPEND may lead to corrupted files on NFS filesystems if
              more than one process appends data to a file at once.
              This is because NFS does not support appending to a file,
              so the client kernel has to simulate it, which can't be
              done without a race condition.

So file without O_APPEND will be smaller, 
becouse between lseek and write bytes can be written and write without APPEND will not update offset
*/

int main(int argc, char *argv[]) {
    static char *HELP_MSG = "Usage: %s fname numbytes [x]\n";
    char *end;
    char *fname;
    size_t numbytes;
    int fd;
    int flag = O_WRONLY | O_CREAT;
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;

    if (argc < 3  || argc > 4) 
        UERR_EXIT(HELP_MSG, argv[0]);
    
    if (argc == 3)
        flag |= O_APPEND;
    
    fname = argv[1];

    numbytes = strtol(argv[2], &end, 10);
    if (*end != '\0')
        UERR_EXIT(HELP_MSG, argv[0]);
    // open
    if ((fd = open(fname, flag, mode)) == -1)
        PERR_EXIT("open fd");
    // write
    for (int i = 0; i < numbytes; i++) {
        lseek(fd, 0, SEEK_END);
        if (write(fd, "B", 1) == -1)
            PERR_EXIT("write error");
    }
    //close
    if (close(fd) == -1)
        PERR_EXIT("closing fd");
    
    printf("It's done\n");    
    exit(EXIT_SUCCESS);
}