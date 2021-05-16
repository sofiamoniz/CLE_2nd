/**
 *  \file main.c
 *
 *  \brief Problem: Assignment 2 - MPI (circular cross-correlation)
 *
 *  Multiprocess message passing - Main program
 *
 *  \author Alina Yanchuk e Ana Sofia Moniz Fernandes
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>
#include <math.h>
#include <time.h>
#include "main_functions.h"
#include <mpi.h>

int nWorkers;

/** \brief variables used to construct the chunks */
int MAX_SIZE_WRD = 50;
int MAX_BYTES_READ = 12;

/** \brief to control the position of file reading */
static long pos;

/** \brief struct to store data of one file*/
typedef struct {
   int  fileId;    /* file with data */  
   int n_words;    /* number words */
   int n_chars; /* number chars */
   int n_consonants;    /* number consonants */
   int in_word;     /* to control the reading of a word */
   int max_chars;   /* max chars found in a word */
   int **counting_array;    /*  to store and process the final countings */
   bool done;        /* to control the end of processing */ 
} PARTFILEINFO;

/** \brief all partial file infos */
static PARTFILEINFO * partfileinfos;

void dispatcher(char *filenames[], int nFiles, char *buf, PARTFILEINFO *partialInfo) {
    int workerId;
    //int lastWorkerReceivingInfo;
    wchar_t c;

    partfileinfos = (PARTFILEINFO*)malloc(sizeof(PARTFILEINFO) * nFiles);   /* memory allocation for the partial infos per file*/

    for (int i=0; i<nFiles; i++) {
        FILE *f;                                                     /* file to process */
        f = fopen(filenames[i], "r");  
        if (f == NULL) { 
            printf("Cannot open file \n"); 
            exit(0); 
        } 
        partfileinfos[i].fileId = i; /* initialize variables */
        partfileinfos[i].n_words = 0;
        partfileinfos[i].n_chars = 0;
        partfileinfos[i].n_consonants = 0;
        partfileinfos[i].in_word = 0;
        partfileinfos[i].max_chars = 0;
        partfileinfos[i].counting_array = (int **)calloc(50, sizeof(int *));
		for (int j = 0; j<50; j++){
			partfileinfos[i].counting_array[j] = (int *)calloc(j+2, sizeof(int));
		}

        int readen_chars = 0;
        fseek(f, pos, SEEK_SET ); 

        c = fgetwc(f);    /* get next char */
        pos = ftell(f);   /* current position of file reading */

        /*first, we do the conversion - if char is not
        multibyte, it will remain unibyte*/
        char converted_char = convert_multibyte(c);

        /* if the number of chars read are still less than MAX_BYTES_READ, they can go directly to the buffer */
        if(readen_chars<MAX_BYTES_READ){
            buf[readen_chars] = converted_char;
            readen_chars++;
        }
        /* otherwise, there are two cases that can happen:
            1 - the char is not end of word -> we don't want to break words, so we add it to the array (using the extra space
            MAX_SIZE_WRD, that is there for this cases where the word is still not completed) 
            2- the char is end of word -> the buffer needs to be emptied and another word is starting
        */
        else{
            if(is_end_of_word(converted_char) == 0){
                buf[readen_chars] = converted_char;
                readen_chars++;
            }
            else{
                memset(buf, 0, MAX_BYTES_READ+MAX_SIZE_WRD);
                readen_chars = 0;
                buf[readen_chars] = converted_char;
                readen_chars++;
            }
        }

        fclose(f);
    }

    int fileCurrentlyProcessed = 0;
    bool workToBeDone = true;

    if ( c == WEOF)  { /* if last character of current file */
        partfileinfos[fileCurrentlyProcessed].done = true;   /* done processing current file */
    }

    while(workToBeDone){
        if (partfileinfos[fileCurrentlyProcessed].done == true) {
            if (fileCurrentlyProcessed == nFiles - 1) {       /* if current file is the last file to be processed */
                workToBeDone = false;
                break;                                                    /* end */
            }
            fileCurrentlyProcessed++;       /* next file to process */
        }
        *partialInfo = partfileinfos[fileCurrentlyProcessed];
        /* send infos to the workers in a parallelized way */
        for (workerId=1; workerId <= nWorkers; workerId++) {
            partfileinfos[fileCurrentlyProcessed].done = false;

            /* tell worker there is work to be done */
            MPI_Send(&workToBeDone, 1, MPI_C_BOOL, workerId, 0, MPI_COMM_WORLD);

            /* send buffer */
            MPI_Send(buf, MAX_BYTES_READ+MAX_SIZE_WRD, MPI_CHAR, workerId, 0, MPI_COMM_WORLD);
        
        }

    }

}

/**
 *  \brief Main thread.
 *
 *  Its role is store the filenames in the shared region; starting the simulation by generating the intervening workers and
 *  waiting for their termination; and also check and show the results;
 * 
 */
int main(int argc, char **argv) {

    int rank;
    int size;  

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    nWorkers = size; /* number of processes/workers */

    /* buffer has size of MAX_BYTES_TO_READ bytes + MAX_SIZE_WORD -> this way,
    we prevent the case where the last word that was readen is not complete. It will be a set of complete words. */
    char buf[MAX_BYTES_READ+MAX_SIZE_WRD];

    PARTFILEINFO partialInfo;     /* struct to store partial info of current file being processed */

    if (rank == 0) { /* dispatcher */

        char *filenames[argc-2];                         /* file names */

        for(int i=1; i<argc; i++) {  /* get file names */
            filenames[i-1] = argv[i];
        }

        dispatcher(filenames, argc - 1,buf,&partialInfo);

    }

    MPI_Finalize();
    return EXIT_SUCCESS;
}

