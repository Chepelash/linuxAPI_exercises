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
