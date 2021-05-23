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
#include <string.h>
#include <wchar.h>
#include <locale.h>

#include "dispatcher_functions.h"
#include "partfileinfo.h"

/** \brief variables used to construct the chunks */
int MAX_SIZE_WRD = 50;
int MAX_BYTES_READ = 12;

/** \brief file currentçy being processed */
int fileCurrentlyProcessed = 0;

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
 *
 */
void loadFilesInfo(int numberFiles, char *inputFilenames[]) 
{
    setlocale(LC_CTYPE, "");

    nFiles = numberFiles;

    filenames = malloc(nFiles * sizeof(char*));

    partfileinfos = (PartFileInfo*)malloc(sizeof(PartFileInfo) * nFiles);   /* memory allocation for the array with partial infos per file */
    
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
        
        /* initialize variables of the structure*/
        partfileinfos[i].fileId = i;    
        partfileinfos[i].n_words = 0;
        partfileinfos[i].n_chars = 0;
        partfileinfos[i].n_consonants = 0;
        partfileinfos[i].in_word = 0;
        partfileinfos[i].max_chars = 0;
        partfileinfos[i].done = false;
        partfileinfos[i].firstProcessing = true;
        for (int j = 0; j<50; j++)
        {
            for(int k=0; k<51; k++) 
            {
                partfileinfos[i].counting_array[j][k]=0;
            }
        }

        fclose(f);
    }
}

/**
 *  \brief Function getDataChunk. 
 *
 *  Obtain next data chunk (buffer) of the current file being processed.
 * 
 *  @param fileCurrentlyProcessed file that is currently being processed
 *  @param buf responsible for carrying the data chunks. Buf (buffer) has size of MAX_BYTES_TO_READ bytes + MAX_SIZE_WORD -> this way,
 *  we prevent the case where the last word that was readen is not complete. It will be a set of complete words
 * 
 *  @return 1 if there is no more data to process, 0 otherwise.
 * 
 */
int getDataChunk(char *buf)
{

    if (partfileinfos[fileCurrentlyProcessed].done == true)   /* if no more data to process in current file */  
        {     
        if (fileCurrentlyProcessed == nFiles - 1)    /* if current file is the last file to be processed */
        {       
            return 1;              /* end */
        }
        
        fileCurrentlyProcessed++;       /* next file to process */
    }  

    int readen_chars = 0;     /* count chars readen */

    FILE *f = fopen(filenames[fileCurrentlyProcessed], "r"); 
    if (f == NULL) 
    { 
        printf("Cannot open file \n"); 
        exit(0); 
    } 

    memset(buf, 0, MAX_BYTES_READ+MAX_SIZE_WRD);  /*  clean buffer */

    while(true)
    {

        if (partfileinfos[fileCurrentlyProcessed].firstProcessing==false) fseek(f, pos, SEEK_SET );  /* go to position where stopped read last time */
        else partfileinfos[fileCurrentlyProcessed].firstProcessing = false;

        wchar_t c;
        c = fgetwc(f);    /* get next char */
        pos = ftell(f);   /* current position of file reading */

        /* first, we do the conversion - if char is not
        multibyte, it will remain unibyte */
        char converted_char = convert_multibyte(c);

        if(is_apostrophe_merge(converted_char) == 1) 
            continue;      /* apostrophe merges two words */

    
        if(readen_chars < MAX_BYTES_READ) 
        {
            buf[readen_chars] = converted_char;
            readen_chars++;
            if(is_end_of_word(converted_char) == 1)  /* word completed */
                break;
        }
        else    /* use extra space if word is not completed */
        {
            buf[readen_chars] = converted_char;
            if(is_end_of_word(converted_char) == 1)    /* word completed */
                break;
            else 
                readen_chars++;
        }

        if (c == WEOF)    /* end of file */
        {
            partfileinfos[fileCurrentlyProcessed].done = true; /* end of the processing of the current file */
            break;
        }

    }

    fclose(f);

    return 0;
}

/**
 *  \brief Function savePartialResults.
 *
 *  Save partial results of workers so that later the final opeartions can be done.
 * 
 *  @param partfileinfo structure containing the partial results from that worker.
 * 
 */

void savePartialResults(PartFileInfo partfileinfo) 
{

    partfileinfos[fileCurrentlyProcessed].n_words += partfileinfo.n_words;
    partfileinfos[fileCurrentlyProcessed].n_chars += partfileinfo.n_chars;
    partfileinfos[fileCurrentlyProcessed].n_consonants += partfileinfo.n_consonants;
    if(partfileinfo.max_chars > partfileinfos[fileCurrentlyProcessed].max_chars) partfileinfos[fileCurrentlyProcessed].max_chars = partfileinfo.max_chars;
    for (int j = 0; j<50; j++)
    {
        for(int k=0; k<51; k++) 
        {
            partfileinfos[fileCurrentlyProcessed].counting_array[j][k] += partfileinfo.counting_array[j][k];
        }
    }

}


/**
 *  \brief Print all final results.
 *
 *  Makes all the final calculations and prints the final results.
 * 
 */

void printProcessingResults() 
{

    for (int i=0; i<nFiles; i++) 
    {                  /* for each file */

        printf("\nFile name: %s\n", filenames[i]);

        printf("Total number of words = %d\n", partfileinfos[i].n_words);

        printf("Word lenght\n");

		printf("   ");
		for(int j = 0; j<partfileinfos[i].max_chars; j++)
        {
			printf("%5d ", j+1);
		}
		printf("\n");

		printf("   ");   /* Print  number words each word length */
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

		printf("   ");     /* final print */
		for(int j = 0; j<partfileinfos[i].max_chars; j++)
        {
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

