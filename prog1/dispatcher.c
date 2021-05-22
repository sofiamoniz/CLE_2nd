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

int fileCurrentlyProcessed = 0;

int * final_array;


/** \brief to control the position of file reading */
static long pos;

/** \brief total number of filenames retrieved */
static int nFiles;

/** \brief pointer that contains the all the filenames passed in the arguments */
static char ** filenames;

/** \brief all partial file infos */
static PartFileInfo * partfileinfos;

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
void loadFilesInfo(int numberFiles, char *inputFilenames[]) 
{

    setlocale(LC_CTYPE, "");

    nFiles = numberFiles;

    filenames = malloc(nFiles * sizeof(char*));

    partfileinfos = (PartFileInfo*)malloc(sizeof(PartFileInfo) * nFiles);
    
    for (int i=0; i<nFiles; i++) 
    {

        filenames[i] = malloc((12) * sizeof(char));       /* memory allocation for the filenames*/
        strcpy(filenames[i], inputFilenames[i]);
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
        partfileinfos[i].done = false;
        partfileinfos[i].firstProcessing = true;

        fclose(f);
    }

    //printf("Dispatcher done loading files.\n");
}

/**
 *  \brief Obtain next data chunk (buffer) of the current file being processed
 *
 *  Operation carried out by workers.
 * 
 *  @param fileCurrentlyProcessed file that is currently being processed
 *  @param buf responsible for carrying the data chunks. Buf (buffer) has size of MAX_BYTES_TO_READ bytes + MAX_SIZE_WORD -> this way,
    we prevent the case where the last word that was readen is not complete. It will be a set of complete words
 * 
 */
int getDataChunk(char *buf)
{

    if (partfileinfos[fileCurrentlyProcessed].done == true) {     /* if no more data to process in current file */  
        if (fileCurrentlyProcessed == nFiles - 1) {       /* if current file is the last file to be processed */
            return 1;                                                      /* end */
        }
        
        fileCurrentlyProcessed++;       /* next file to process */
    }  

    int readen_chars = 0;

    FILE *f = fopen(filenames[fileCurrentlyProcessed], "r"); 
    memset(buf, 0, MAX_BYTES_READ+MAX_SIZE_WRD); // clean array

    while(true)
    {

        if (partfileinfos[fileCurrentlyProcessed].firstProcessing==false) fseek(f, pos, SEEK_SET );  /* go to position where stopped read last time */
        else partfileinfos[fileCurrentlyProcessed].firstProcessing = false;

        wchar_t c;
        c = fgetwc(f);    /* get next char */
        pos = ftell(f);   /* current position of file reading */

        /*first, we do the conversion - if char is not
        multibyte, it will remain unibyte*/
        char converted_char = convert_multibyte(c);

        if(readen_chars < MAX_BYTES_READ) 
        {
            buf[readen_chars] = converted_char;
            readen_chars++;

            if(is_end_of_word(converted_char) == 1){
                break;
            }
        }
        else {
            buf[readen_chars] = converted_char;
            if(is_end_of_word(converted_char) == 1){
                break;
            }
            else 
                readen_chars++;
        }

        if (c == WEOF) {
            partfileinfos[fileCurrentlyProcessed].done = true;
            break;
        }
     

        /* otherwise, there are two cases that can happen:
            1 - the char is not end of word -> we don't want to break words, so we add it to the array (using the extra space
            MAX_SIZE_WRD, that is there for this cases where the word is still not completed) 
            2- the char is end of word -> the buffer needs to be emptied and another word is starting
        */
    }

    fclose(f);

    return 0;
}


void savePartialResults(PartFileInfo partfileinfo, int* array_1D) {

    partfileinfos[fileCurrentlyProcessed].n_words+=partfileinfo.n_words;
    partfileinfos[fileCurrentlyProcessed].n_chars+=partfileinfo.n_chars;
    partfileinfos[fileCurrentlyProcessed].n_consonants+=partfileinfo.n_consonants;
    if(partfileinfo.max_chars > partfileinfos[fileCurrentlyProcessed].max_chars) partfileinfos[fileCurrentlyProcessed].max_chars=partfileinfo.max_chars;
    final_array = malloc(sizeof(int) * 50 * 52);
    
    /*
     * Copy all elements from source array to dest array
     */
    int loop;
    for(loop = 0; loop < sizeof(array_1D); loop++) {
      final_array[loop] = array_1D[loop];
   }

}


/**
 *  \brief Print all final results.
 *
 *  Operation carried out by main thread.
 * 
 *  @param partfileinfos PartFileInfo struct
 */

void printProcessingResults() {

    for (int i=0; i<nFiles; i++) {                  /* each partial file info */

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
                if(j == 0){
				    ind_sum = ind_sum + final_array[k];
                }
				ind_sum = ind_sum + final_array[j+k+j];
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
					double cell = (double)final_array[k];
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

