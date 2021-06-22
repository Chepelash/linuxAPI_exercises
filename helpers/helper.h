#define PERR_EXIT(msg)   \
({                      \
    perror(msg);        \
    exit(EXIT_FAILURE); \
})