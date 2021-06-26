#define PERR_EXIT(msg)  \
({                      \
    perror(msg);        \
    exit(EXIT_FAILURE); \
}) 

#define UERR_EXIT(msg, pname)   \
({                              \
    printf(msg, pname);         \
    exit(EXIT_FAILURE);         \
}) 

#define AERR_EXIT(msg)          \
({                              \
    printf("%s\n", msg);        \
    exit(EXIT_FAILURE);         \
}) 

#define OPEN_FD(fname, flags, mode)     \
({                                      \
    int fd = open(fname, flags, mode);  \
    if (fd == -1)                       \
        PERR_EXIT("Cannot open fd");    \
    fd;                                 \
})

#define CLOSE_FD(fd)                    \
({                                      \
    if(close(fd) == -1)                 \
        PERR_EXIT("Cannot close fd");   \
})

#define READ_FD(fd, buf, count)               \
({                                            \
    ssize_t read_size;                        \
    read_size = read(fd, buf, count);         \
    if(read_size == -1) PERR_EXIT("read fd"); \
    read_size;                                \
})

#define WRITE_FD(fd, buf, count)                \
({                                              \
    ssize_t write_size;                         \
    write_size = write(fd, buf, count);         \
    if(write_size == -1) PERR_EXIT("write fd"); \
    write_size;                                 \
})

#define STRTOL(nptr, base)          \
({                                  \
    char *end;                      \
    ssize_t res;                    \
    res = strtol(nptr, &end, base); \
    if (*end != '\0') {             \
        errno = EINVAL;             \
        PERR_EXIT("strtol error");  \
    }                               \
    res;                            \
})

#define GET_FSTAT(fd, stat_addr)                      \
({                                                    \
    if(fstat(fd, stat_addr) == -1) PERR_EXIT("fstat");\
})
