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


/** \brief struct to store data to process signal of one file*/
typedef struct {
   int  fileId;    /* file with data */  
   int signalLength;  /* length of signals */  
   double *x;   /* signal X */          
   double  *y;   /* signal Y */ 
   double *xy;   /* will store the calculated signal XY */ 
   double *xyCorrect;   /* correct signal XY */ 
   int point;   /* point of processing */ 
   bool done;   /* to control the end of processing */ 
} Signal;


/**
 *  \brief Load all necessary data to the Signal struct, for each file.
 *
 */
void loadFilesInfo(int nFiles, char *filenames[], Signal *signals) 
{
    for (int i=0; i<nFiles; i++) /* save data from all files provided */
    {     
        FILE *fp;                                                     
        fp = fopen(filenames[i], "rb");  
        if (fp == NULL) 
        { 
            printf("Cannot open file \n"); 
            exit(0); 
        } 

        int nElements;                                   /* length of the signals */
        fread(&nElements, sizeof(int), 1, fp);

        signals[i].fileId = i;
        signals[i].signalLength = nElements;
        signals[i].done = false;
        signals[i].point = 0;
        signals[i].x = (double*)malloc(sizeof(double) * nElements);
        signals[i].y = (double*)malloc(sizeof(double) * nElements);
        signals[i].xy = (double*)malloc(sizeof(double) * nElements);
        signals[i].xyCorrect = (double*)malloc(sizeof(double) * nElements);

        fread(signals[i].x, sizeof (double), nElements, fp);   /* signal x */

        fread(signals[i].y, sizeof (double), nElements, fp);     /* signal y */
        
        fclose(fp);

        FILE *fpResult;                                  /* file with correct result */
        char fileResult[16];
        sprintf(fileResult, "newSigVal0%c.bin", filenames[i][7]);
        fpResult= fopen(fileResult, "rb");
        if (fpResult == NULL) 
        { 
            printf("Error %s",fileResult);
            exit(0); 
        } 

        fseek(fpResult, sizeof (int) + 2 * (nElements * sizeof (double)), SEEK_SET );  /* go to where XY is stored */
        
        fread(signals[i].xyCorrect, sizeof (double), nElements, fp);  /* correct signal xy */
        
        fclose(fpResult);
    }

}

/**
 *  \brief Save the convolution point that was processed by a worker.
 *
 */
void savePartialResult(int workerId, int fileId, Signal *signals, double val, int point) 
{
    signals[fileId].xy[point] = val;           /* store value */
}

/**
 *  \brief Check if all the convolution points processed were correctly computed.
 *
 */
void checkProcessingResults(int nFiles, Signal *signals) 
{
    for(int i=0; i<nFiles; i++) /* check results for all files */
    {   
        for(int t=0; t<signals[i].signalLength; t++) 
        {
            if(signals[i].xy[t]!=signals[i].xyCorrect[t]) 
                printf("Incorrect! Correct value: %f  // Computed value: %f\n", signals[i].xyCorrect[t], signals[i].xy[t]);  /* confirm if calculated value is correct */
        }
    }
}