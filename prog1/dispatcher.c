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
#include <wchar.h>
#include <string.h>

#include "dispatcher_functions.h"

#include "partfileinfo.h"

/** \brief variables used to construct the chunks */
int MAX_SIZE_WRD = 50;
int MAX_BYTES_READ = 12;

/** \brief to control the position of file reading */
long pos = -1;

/** \brief total number of filenames retrieved */
int num_files;

/** \brief pointer that contains the all the filenames passed in the arguments */
static char ** filenames;

/** \brief pointer that saves the total number of words found in each file. */
int *total_n_words;

/** \brief pointer that saves the total number of chars found in each file. */
int *total_max_chars;

/** \brief index which represents the current opened file. */
int files_idx = -1;

/** \brief pointer that saves the counting array of each file. */
int **gbl_counting_array;



void loadFilesInfo(int nFiles, char *inputFilenames[], PartFileInfo *partfileinfos) 
{

    filenames = malloc(nFiles * sizeof(char*));

    for (int i=0; i<nFiles; i++) {
        filenames[i] = malloc((12) * sizeof(char));       /* memory allocation for the filenames*/
        strcpy(filenames[i], inputFilenames[i]);
        FILE *f;                                                     /* file to process */
        f = fopen(inputFilenames[i], "r");  
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
        partfileinfos[i].done = false;
        partfileinfos[i].firstProcessing = true;
    }
}

void getDataChunk(int fileCurrentlyProcessed, PartFileInfo *partfileinfos, char *buf)
{
    FILE *f = fopen(filenames[fileCurrentlyProcessed], "r");

    int readen_chars = 0;

    if (partfileinfos[fileCurrentlyProcessed].firstProcessing==false) fseek(f, pos, SEEK_SET );  /* go to position where stopped read last time */
    if (partfileinfos[fileCurrentlyProcessed].firstProcessing==true) partfileinfos[fileCurrentlyProcessed].firstProcessing = false;


    wchar_t c = fgetwc(f);    /* get next char */
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

    if ( c == WEOF)  { /* if last character of current file */
    partfileinfos[fileCurrentlyProcessed].done = true;   /* done processing current file */
    }
    
}




/**
 *  \brief Print all final results.
 *
 *  Operation carried out by main thread.
 */

void printProcessingResults(PartFileInfo *partfileinfos) {

    for (int i=0; i<num_files; i++) {                  /* each partial file info */

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
		for(int j = 0; j<partfileinfos[i].max_chars; j++){
			int ind_sum = 0;
			for(int k = 0; k<j+2; k++){
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

	
		for(int j = 0; j<partfileinfos[i].max_chars+1; j++){ 
			printf("%2d ", j);
			for(int k = 0; k<partfileinfos[i].max_chars; k++){ 
				if(k<j-1){
					printf("      ");
				}
				else if(soma[k]==0){ 
					double r = 0;
					printf("%5.1f ", r);
				}
				else{
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