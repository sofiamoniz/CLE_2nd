/**
 *  \file dispatcher.c
 *
 *  \brief Problem: Assignment 2 - MPI (circular cross-correlation)
 *
 *  Implements all the methods that will be called by the dispatcher.
 *
 *  \author Alina Yanchuk e Ana Sofia Moniz Fernandes
 */



#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <wchar.h>
#include <locale.h>

#include "dispatcher_functions.h"
#include "partfileinfo.h"

/** \brief variables used to construct the chunks */
int MAX_SIZE_WRD = 50;
int MAX_BYTES_READ = 12;
int readen_chars;

/** \brief to control the position of file reading */
static long pos;

/** \brief total number of filenames retrieved */
int num_files;

/** \brief pointer that contains the all the filenames passed in the arguments */
static char ** filenames;

/**
 *  \brief Function loadFilesInfo.
 *
 *   Loads all necessary data to the PartFileInfo struct, for each file.
 * 
 *  @param nFiles num of files passed as argument
 *  @param inputFilenames names of the files passed as argument
 *  @param partfileinfos PartFileInfo struct
 *
 */
void loadFilesInfo(int nFiles, char *inputFilenames[], PartFileInfo *partfileinfos) 
{
    printf("\n loadfilesinfo");
    setlocale(LC_CTYPE, "");

    filenames = malloc(nFiles * sizeof(char*));

    for (int i=0; i<nFiles; i++) 
    {
        printf("\n vou-me brekar");
        printf("\n inputFilenames %s", inputFilenames[1] );
        filenames[i] = malloc((12) * sizeof(char));       /* memory allocation for the filenames*/
        strcpy(filenames[i], inputFilenames[i]);
        printf("\n cheguei aqui");
        printf(" \n sou o ficheiro %s", inputFilenames[i]);
        FILE *f;                                                     /* file to process */
        f = fopen(inputFilenames[i], "r");  
        if (f == NULL) 
        { 
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
        partfileinfos[i].done = false;
        partfileinfos[i].firstProcessing = true;

        fclose(f);
    }
    printf("terminei loadfilesinfo");
}

/**
 *  \brief Obtain next data chunk (buffer) of the current file being processed
 *
 *  Operation carried out by workers.
 * 
 *  @param fileCurrentlyProcessed file that is currently being processed
 *  @param partfileinfos PartFileInfo struct
 *  @param buf responsible for carrying the data chunks. Buf (buffer) has size of MAX_BYTES_TO_READ bytes + MAX_SIZE_WORD -> this way,
    we prevent the case where the last word that was readen is not complete. It will be a set of complete words
 * 
 */
void getDataChunk(int fileCurrentlyProcessed, PartFileInfo *partfileinfos, char *buf)
{
    readen_chars = 0;

    FILE *f = fopen(filenames[fileCurrentlyProcessed], "r"); 

    if (partfileinfos[fileCurrentlyProcessed].firstProcessing==false) fseek(f, pos, SEEK_SET );  /* go to position where stopped read last time */
    else partfileinfos[fileCurrentlyProcessed].firstProcessing = false;

    wchar_t c;
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
    
    if (c == WEOF)  { /* if last character of current file */
        partfileinfos[fileCurrentlyProcessed].done = true;   /* done processing current file */
    }

    
}


void savePartialResults(int fileCurrentlyProcessed, PartFileInfo *partfileinfos, PartFileInfo *partfileinforeceived) {

        partfileinfos[fileCurrentlyProcessed].n_words += partfileinforeceived->n_words;
        partfileinfos[fileCurrentlyProcessed].n_chars += partfileinforeceived->n_chars;
        partfileinfos[fileCurrentlyProcessed].n_consonants += partfileinforeceived->n_consonants;
        partfileinfos[fileCurrentlyProcessed].n_words += partfileinforeceived->n_words;
        partfileinfos[fileCurrentlyProcessed].done = partfileinforeceived->done;
        partfileinfos[fileCurrentlyProcessed].firstProcessing = partfileinforeceived->firstProcessing;
        partfileinfos[fileCurrentlyProcessed].max_chars = partfileinforeceived->max_chars;

}


/**
 *  \brief Print all final results.
 *
 *  Operation carried out by main thread.
 * 
 *  @param partfileinfos PartFileInfo struct
 */

void printProcessingResults(PartFileInfo *partfileinfos) 
{

    for (int i=0; i<num_files; i++) /* each partial file info */
    {                  

        printf("\nFile name: %s\n", filenames[i]);

        printf("Total number of words = %d\n", partfileinfos[i].n_words);

        printf("Word lenght\n");

		printf("   ");
		for(int j = 0; j<partfileinfos[i].max_chars; j++){
			printf("%5d ", j+1);
		}
		printf("\n");

        //Print  number words each word length
		printf("   ");
		int *soma = (int *)calloc(partfileinfos[i].max_chars, sizeof(int));
		int tmp = 0;
		for(int j = 0; j<partfileinfos[i].max_chars; j++)
        {
			int ind_sum = 0;
			for(int k = 0; k<j+2; k++)
            {
				ind_sum = ind_sum + partfileinfos[i].counting_array[j][k];
			}
			tmp = tmp + ind_sum;
			soma[j] = ind_sum;
			printf("%5d ", soma[j]);
		}
		printf("\n");


		//FINAL PRINT
		printf("   ");
		for(int j = 0; j<partfileinfos[i].max_chars; j++){
			double s = (double)soma[j];
			double st = (double)tmp;
			double r = (double)(s/st*100);
			printf("%5.2f ", r);
		}
		printf("\n");

	
		for(int j = 0; j<partfileinfos[i].max_chars+1; j++)
        { 
			printf("%2d ", j);
			for(int k = 0; k<partfileinfos[i].max_chars; k++)
            { 
				if(k<j-1)
                {
					printf("      ");
				}
				else if(soma[k]==0)
                { 
					double r = 0;
					printf("%5.1f ", r);
				}
				else
                {
					double cell = (double)partfileinfos[i].counting_array[k][j];
					double s = (double)soma[k];
					double r = (double)(cell/s*100);
					printf("%5.1f ", r);
				}
			}
			printf("\n");
		}
		printf("\n");
    }
}