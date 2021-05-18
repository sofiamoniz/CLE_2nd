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
#include <time.h>
#include <mpi.h>

#include "dispatcher.h"
#include "worker.h"

/** \brief number of workers */
int nWorkers;

/**
 *  \brief Dispatcher life cycle.
 *
 *  Sends to workers chunks of data to be processed, waits fortheir processing, saves partial results
 *  and, when all work is done, lets them know of the fact and prints the results of the whole processing.
 * 
 */
void dispatcher(char *filenames[], int nFiles) 
{

    int workerId;       /* will be the worker ids */

    Signal signals[nFiles];       /* will store all info about signals, per file */

    loadFilesInfo(nFiles, filenames, signals);      /* load data from files to signal structures */

    int fileCurrentlyProcessed = 0;       /* file currently being processed */
    bool workToBeDone = true;       /* to control the end of work */
    int lastWorkerReceivingInfo = 1;       /* last worker that received a chunk of data to process */

    while (workToBeDone) 
    {
        if (signals[fileCurrentlyProcessed].done == true)  /* if no more work to be done in current file */
        {      
            if (fileCurrentlyProcessed == nFiles - 1)  /* if current file is the last file to be processed */
            {      
                workToBeDone = false;
                for (int i = 1; i <= nWorkers; i++) 
                    MPI_Send(&workToBeDone, 1, MPI_C_BOOL, i, 0, MPI_COMM_WORLD);       /* tell workers there is no more work to be done */
                break;                                                    /* end */
            }
            fileCurrentlyProcessed++;       /* next file to process */
        } 

        for (workerId=1; workerId <= nWorkers; workerId++)   /* send chunks of data to the workers in a parallelized way */
        {    
            if (signals[fileCurrentlyProcessed].done == true) 
                break;
            
            lastWorkerReceivingInfo = workerId;

            MPI_Send(&workToBeDone, 1, MPI_C_BOOL, workerId, 0, MPI_COMM_WORLD);

            int pointAndSignalLength[2];     /* will store the point of process and length of signals */

            pointAndSignalLength[0] = signals[fileCurrentlyProcessed].point;
            pointAndSignalLength[1] = signals[fileCurrentlyProcessed].signalLength;

            MPI_Send(pointAndSignalLength, 2, MPI_INT, workerId, 0, MPI_COMM_WORLD);

            double x[signals[fileCurrentlyProcessed].signalLength];    /* will store the signal x */
            double y[signals[fileCurrentlyProcessed].signalLength];   /* will store the signal y */

            for (int i = 0; i < signals[fileCurrentlyProcessed].signalLength; i++) 
            {
                x[i] = signals[fileCurrentlyProcessed].x[i];
                y[i] = signals[fileCurrentlyProcessed].y[i];
            }
            
            MPI_Send(x, signals[fileCurrentlyProcessed].signalLength, MPI_DOUBLE, workerId, 0, MPI_COMM_WORLD);
            MPI_Send(y, signals[fileCurrentlyProcessed].signalLength, MPI_DOUBLE, workerId, 0, MPI_COMM_WORLD);
            
            signals[fileCurrentlyProcessed].point++;              /* increment point of process */

            if (signals[fileCurrentlyProcessed].point == signals[fileCurrentlyProcessed].signalLength) 
                signals[fileCurrentlyProcessed].done = true; /* if last point of process */      
        }

        for (workerId=1; workerId <= lastWorkerReceivingInfo; workerId++) /* received partial info computed by workers */
        {      
            double xyAndPoint[2];   /* will store the computed xy and the respective point */

            MPI_Recv(xyAndPoint, 2, MPI_DOUBLE, workerId, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            savePartialResult(workerId, fileCurrentlyProcessed, signals, xyAndPoint[0], xyAndPoint[1]);    /* save that xy */
        }
        
    }

    checkProcessingResults(nFiles, signals);   /* check the results */

}

/**
 *  \brief Worker life cycle.
 *
 *  Processes the received chunk of data and delivers the results back to the dispatcher.
 * 
 */
void worker() 
{
    
    bool workToBeDone;      /* info received by dispatcher */

    while (true) 
    {
        MPI_Recv(&workToBeDone, 1, MPI_C_BOOL, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        if (!workToBeDone)  /* no more work to be done by workers */
        {     
            //printf("Worker with rank %d terminated...\n", rank);
            return;
        }

        int pointAndSignalLength[2];      /* will store the point of process and the lenght of signal */

        MPI_Recv(pointAndSignalLength, 2, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        double x[pointAndSignalLength[1]];      /* will store the x signal */
        double y[pointAndSignalLength[1]];      /* will store the y signal */

        MPI_Recv(x, pointAndSignalLength[1], MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(y, pointAndSignalLength[1], MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        double xy[2];       /* will store the computed xy and the respective point */
        
        xy[0] = computeValue(pointAndSignalLength[1], x, y, pointAndSignalLength[0]);
        xy[1] = pointAndSignalLength[0];
        
        MPI_Send(xy, 2, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);       /* send partial info computed to dispatcher */  
    }
}

/**
 *  \brief Main function.
 *
 *  Main thread that runs the program / executes the processes.
 * 
 */
int main(int argc, char **argv) 
{

    int rank;
    int size;  

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    nWorkers = size - 1; /* number of workers */

    if (rank == 0) /* dispatcher */
    { 
        double tStart = ((double) clock()) / CLOCKS_PER_SEC;    /* start of execution time */

        char *filenames[argc-2];                         /* file names */

        for(int i=1; i<argc; i++)   /* get file names */
            filenames[i-1] = argv[i];

        dispatcher(filenames, argc - 1);    /* dispatcher life cycle */

        printf("All processes terminated. \n");

        double tStop = ((double) clock()) / CLOCKS_PER_SEC;          /* end of execution time */
        printf ("\nElapsed time = %.6f s\n", tStop - tStart);
    }

    else 
        worker(); /* worker, worker life cycle */

    MPI_Finalize();

    return EXIT_SUCCESS;
}




