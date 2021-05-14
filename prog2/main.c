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
#include <mpi.h>


int nWorkers;

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

/** \brief all signals */
static Signal * signals;


void dispatcher(char *filenames[], int nFiles) {
    int workerId;
    int lastWorkerReceivingInfo;

    signals = (Signal*)malloc(sizeof(Signal) * nFiles);

    for (int i=0; i<nFiles; i++) {

        FILE *fp;                                                     /* file to process */
        fp = fopen(filenames[i], "rb");  
        if (fp == NULL) { 
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
        
        double elementsX[nElements];                                           /* signal X */
        fread(elementsX, sizeof (double), nElements, fp);
        for(int t=0; t<nElements; t++) signals[i].x[t] = elementsX[t];

        double elementsY[nElements];                                  /* signal Y */
        fread(elementsY, sizeof (double), nElements, fp);
        for(int t=0; t<nElements; t++) signals[i].y[t] = elementsY[t];
        
        fclose(fp);

        FILE *fpResult;                                  /* file with correct result */
        char fileResult[16];
        sprintf(fileResult, "newSigVal0%c.bin", filenames[i][7]);
        fpResult= fopen(fileResult, "rb");
        if (fpResult == NULL) { 
            printf("Error %s",fileResult);
            exit(0); 
        } 

        fseek(fpResult, sizeof (int) + 2 * (nElements * sizeof (double)), SEEK_SET );  /* fg to where XY is stored */
        
        double elementsXY[nElements];                                  /* correct signal XY */
        fread(elementsXY, sizeof (double), nElements, fp);
        for(int t=0; t<nElements; t++) signals[i].xyCorrect[t] = elementsXY[t];
        
        fclose(fpResult);

    }

    int fileCurrentlyProcessed = 0;
    bool workToBeDone = true;

    while (workToBeDone) {

        if (signals[fileCurrentlyProcessed].done == true) {
            if (fileCurrentlyProcessed == nFiles - 1) {       /* if current file is the last file to be processed */
                workToBeDone = false;
                break;                                                    /* end */
            }
            fileCurrentlyProcessed++;       /* next file to process */
        } 

        // send infos to the workers in a parallelized way
        for (workerId=1; workerId <= nWorkers; workerId++) {

            // tell worker there is work to be done
            MPI_Send(&workToBeDone, 1, MPI_C_BOOL, workerId, 0, MPI_COMM_WORLD);

            int point[1];
            point[0] = signals[fileCurrentlyProcessed].point;

            MPI_Send(point, 1, MPI_INT, workerId, 0, MPI_COMM_WORLD);

            //printf("worker: %d, point: %d\n", threadId, signals[fileCurrentlyProcessed].point);
            
            double x[signals[fileCurrentlyProcessed].signalLength];
            double y[signals[fileCurrentlyProcessed].signalLength];

            for (int i = 0; i < signals[fileCurrentlyProcessed].signalLength; i++) {
                x[i] = signals[fileCurrentlyProcessed].x[i];
                y[i] = signals[fileCurrentlyProcessed].y[i];
            }

            MPI_Send(x, signals[fileCurrentlyProcessed].signalLength, MPI_DOUBLE, workerId, 0, MPI_COMM_WORLD);
            MPI_Send(y, signals[fileCurrentlyProcessed].signalLength, MPI_DOUBLE, workerId, 0, MPI_COMM_WORLD);

            signals[fileCurrentlyProcessed].point++;                                 /* increment point of process */

            if (signals[fileCurrentlyProcessed].point == signals[fileCurrentlyProcessed].signalLength) signals[fileCurrentlyProcessed].done = true; /* if last point of process */

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
void main(int argc, char **argv) {

    int rank;
    int size;  

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    nWorkers = size; /* number of processes/workers */

    if (rank == 0) { /* dispatcher */

        char *filenames[argc-2];                         /* file names */

        for(int i=1; i<argc; i++) {  /* get file names */
            filenames[i-1] = argv[i];
        }

        dispatcher(filenames, argc - 1);

    }
    //else  /* worker */
        //worker(rank);

    MPI_Finalize();
    return EXIT_SUCCESS;
}




