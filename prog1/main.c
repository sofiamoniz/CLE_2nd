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

#include "dispatcher.h"
#include "partfileinfo.h"

/** \brief number of workers */
int nWorkers;

/** \brief variables used to construct the chunks */
int MAX_SIZE_WRD = 50;
int MAX_BYTES_READ = 12;


void dispatcher(char *filenames[], int nFiles, char *buf) 
{
    
    int workerId;
   
    //partfileinfos = (PARTFILEINFO*)malloc(sizeof(PARTFILEINFO) * nFiles);   /* memory allocation for the partial infos per file*/
    PARTFILEINFO partfileinfos[nFiles];
    

    int fileCurrentlyProcessed = 0;       /* file currently being processed */
    bool workToBeDone = true;       /* to control the end of work */
    int lastWorkerReceivingInfo = 1;       /* last worker that received a chunk of data to process */

    //if ( c == WEOF)  { /* if last character of current file */
      //  partfileinfos[fileCurrentlyProcessed].done = true;   /* done processing current file */
    //}

    while(workToBeDone){
        if (partfileinfos[fileCurrentlyProcessed].done == true) {
            if (fileCurrentlyProcessed == nFiles - 1) 
            {       /* if current file is the last file to be processed */
                workToBeDone = false;
                for (int i = 1; i <= nWorkers; i++) 
                    MPI_Send(&workToBeDone, 1, MPI_C_BOOL, i, 0, MPI_COMM_WORLD);       /* tell workers there is no more work to be done */
                break;                                                    /* end */
            }
            fileCurrentlyProcessed++;       /* next file to process */
        }
        //*partialInfo = partfileinfos[fileCurrentlyProcessed];
        /* send infos to the workers in a parallelized way */
        for (workerId=1; workerId <= nWorkers; workerId++) 
        {
            if (partfileinfos[fileCurrentlyProcessed].done == true) 
                break;

            lastWorkerReceivingInfo = workerId;

            /* tell worker there is work to be done */
            MPI_Send(&workToBeDone, 1, MPI_C_BOOL, workerId, 0, MPI_COMM_WORLD);

            /* send buffer */
            MPI_Send(buf, MAX_BYTES_READ+MAX_SIZE_WRD, MPI_CHAR, workerId, 0, MPI_COMM_WORLD);
        
        }

    }

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

    PartFileInfo partfileinfos;

    while (true)
    {
        MPI_Recv(&workToBeDone, 1, MPI_C_BOOL, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        if (!workToBeDone)  /* no more work to be done by workers */
        {     
            //printf("Worker with rank %d terminated...\n", rank);
            return;
        }
        /* buffer has size of MAX_BYTES_TO_READ bytes + MAX_SIZE_WORD -> this way,
        we prevent the case where the last word that was readen is not complete. It will be a set of complete words. */
        char buf[MAX_BYTES_READ+MAX_SIZE_WRD];

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

    nWorkers = size - 1; /* number of processes/workers */


    if (rank == 0) { /* dispatcher */

        char *filenames[argc-2];                         /* file names */

        for(int i=1; i<argc; i++) {  /* get file names */
            filenames[i-1] = argv[i];
        }

        dispatcher(filenames, argc - 1,buf);

    }
    else{
        worker();
    }

    MPI_Finalize();
    return EXIT_SUCCESS;
}

