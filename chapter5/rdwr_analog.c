#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/uio.h>
#include <string.h>

#include "../helpers/helper.h"

/*
Реализуйте функции readv() и writev(), используя системные вызовы read(), write()
и подходящие функции из пакета malloc
*/

#define HELP_MSG "/*\n"                                                         \
 "* usage: rdwr_analog writev src_file1 src_file2 .... dest_file\n"      \
 "* OR\n"                                                                       \
 "* usage: rdwr_analog readv src_file dest_file1 dest_file2 ...\n"       \
 "* src_file will be devided equally\n"                                         \
 "*/\n"                                                                          

#define MANDATORY_ARGV_NUM 3
#define MANDATORY_ARGV_OFFSET 2

ssize_t readv_analog(int fd, const struct iovec *iov, int iovcnt) {
    ssize_t totalbytes;
    ssize_t bytesread;
    for (int i = 0; i < iovcnt; i++) {
        if ((bytesread = read(fd, iov[i].iov_base, iov[i].iov_len)) == -1) 
            return -1;
        totalbytes += bytesread;
    }
    return totalbytes;
}
ssize_t writev_analog(int fd, const struct iovec *iov, int iovcnt) {
    ssize_t totalbytes = 0;
    ssize_t byteswritten = 0;
    ssize_t rest;
    for (int i = 0; i < iovcnt; i++) {        
        do {
            if ((rest = write(fd, iov[i].iov_base, iov[i].iov_len)) == -1) 
                return -1;
            byteswritten += rest;
        } while (byteswritten < iov[i].iov_len);
        totalbytes += byteswritten;
        byteswritten = 0;
    }
    return totalbytes;
}

int main(int argc, char *argv[]) {
    int fd_read, fd_write;
    int open_flags;
    mode_t open_mode;
    struct iovec *iov;    
    char **buf;
    int file_num;
    struct stat fstat_buf;
    ssize_t bytes_read, bytes_write;
    size_t psize, preminder, flen;
    
    if(argc < 4 || strcmp(argv[1], "-h") == 0) UERR_EXIT(HELP_MSG, argv[0]);
    if(!(strcmp(argv[1], "writev") == 0) && !(strcmp(argv[1], "readv") == 0))
        UERR_EXIT(HELP_MSG, argv[0]);
    
    file_num = argc - MANDATORY_ARGV_NUM;

    iov = malloc(file_num * sizeof(struct iovec));
    if(iov == NULL) AERR_EXIT("iov malloc");

    buf = malloc(file_num * sizeof(*buf));
    if(buf == NULL) AERR_EXIT("**buf malloc");

    if(strcmp(argv[1], "writev") == 0){
        open_flags = O_RDONLY;
        for(int i = 0; i < file_num; i++){
            fd_read = OPEN_FD(argv[i+MANDATORY_ARGV_OFFSET], open_flags, open_mode);            
            GET_FSTAT(fd_read, &fstat_buf);
            buf[i] = malloc(fstat_buf.st_size);
            if(buf[i] == NULL) AERR_EXIT("*buf malloc");
            READ_FD(fd_read, buf[i], fstat_buf.st_size);
            CLOSE_FD(fd_read);
            iov[i].iov_base = buf[i];
            iov[i].iov_len = fstat_buf.st_size;            
        }
        open_flags = O_WRONLY | O_CREAT | O_TRUNC;
        open_mode = S_IRUSR | S_IWUSR;
        fd_write = OPEN_FD(argv[argc-1], open_flags, open_mode);
        bytes_write = writev_analog(fd_write, iov, file_num);
        if(bytes_write == -1) AERR_EXIT("writev");        
        CLOSE_FD(fd_write);
    } else {
        open_flags = O_RDONLY;
        fd_read = OPEN_FD(argv[MANDATORY_ARGV_OFFSET], open_flags, open_mode);
        GET_FSTAT(fd_read, &fstat_buf);        
        
        psize = fstat_buf.st_size / file_num;
        preminder = fstat_buf.st_size - psize * file_num;        
        
        for(int i = 0; i < file_num; i++){
            if(i == file_num - 1)
                flen = psize + preminder;
            else
                flen = psize;
            
            buf[i] = malloc(flen);
            if(buf[i] == NULL) AERR_EXIT("*buf malloc");

            iov[i].iov_base = buf[i];
            iov[i].iov_len = flen;
        }
        bytes_read = readv_analog(fd_read, iov, file_num);
        if(bytes_read == -1) AERR_EXIT("readv_analog");
        CLOSE_FD(fd_read);
        
        open_flags = O_WRONLY | O_CREAT | O_TRUNC;
        open_mode = S_IRUSR | S_IWUSR;
        
        for(int i = 0; i < file_num; i++){
            fd_write = OPEN_FD(argv[MANDATORY_ARGV_NUM+i], open_flags, open_mode);
            bytes_write = WRITE_FD(fd_write, iov[i].iov_base, iov[i].iov_len);
            if(bytes_write < iov[i].iov_len) AERR_EXIT("bytes_write < iov[i].iov_len");
            CLOSE_FD(fd_write);
        }
    }

    for(int i = 0; i < file_num; i++)
        free(buf[i]);
    free(buf);
    free(iov);
    exit(EXIT_SUCCESS);
}
