#include "consts.h"
typedef struct {
    int  fileId;    /* file with data */  
    int n_words;    /* number words */
    int n_chars; /* number chars */
    int n_chars_readen; /* number chars already read */
    int n_consonants;    /* number consonants */
    int in_word;     /* to control the reading of a word */
    int max_chars;   /* max chars found in a word */
    int **counting_array;    /*  to store and process the final countings */
    unsigned char chars_read[WORD_SIZE * N_TOKENS];
    bool done;        /* to control the end of processing */ 
} PARTFILEINFO;
