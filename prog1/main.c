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
#include <math.h>
#include <time.h>
#include <mpi.h>

#include "dispatcher.h"
#include "worker.h"
#include "partfileinfo.h"


/** \brief number of workers */
int nWorkers;

/** \brief variables used to construct the chunks */
int MAX_SIZE_WORD = 50;
int MAX_BYTES_TO_READ = 12;


/**
 *  \brief Dispatcher life cycle.
 *
 *  Sends to workers chunks of data to be processed, waits for their processing, saves partial results
 *  and, when all work is done, lets them know of the fact and prints the results of the whole processing.
 * 
 *  @param filenames names of the files to be processed
 *  @param nFiles num of files to be processed
 */
void dispatcher(char *filenames[], int nFiles) 
{
    
    int workerId;
   
    //partfileinfos = (PARTFILEINFO*)malloc(sizeof(PARTFILEINFO) * nFiles);   /* memory allocation for the partial infos per file*/
    PartFileInfo partfileinfos[nFiles];
    

    int fileCurrentlyProcessed = 0;       /* file currently being processed */
    bool workToBeDone = true;       /* to control the end of work */
    int lastWorkerReceivingInfo = 1;       /* last worker that received a chunk of data to process */
    char buf[MAX_BYTES_TO_READ+MAX_SIZE_WORD];

    loadFilesInfo(nFiles, filenames, partfileinfos);

    while(workToBeDone)
    {
        if (partfileinfos[fileCurrentlyProcessed].done == true)
        {
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

            getDataChunk(fileCurrentlyProcessed, partfileinfos, buf);

            if (partfileinfos[fileCurrentlyProcessed].done == true) 
                break;

            lastWorkerReceivingInfo = workerId;

            /* tell worker there is work to be done */
            MPI_Send(&workToBeDone, 1, MPI_C_BOOL, workerId, 0, MPI_COMM_WORLD);

            MPI_Send(buf, MAX_BYTES_TO_READ+MAX_SIZE_WORD, MPI_CHAR, workerId, 0, MPI_COMM_WORLD);

            MPI_Send(&partfileinfos[fileCurrentlyProcessed], sizeof(PartFileInfo), MPI_BYTE, workerId, 0, MPI_COMM_WORLD);

        }
        
        for (workerId=1; workerId <= lastWorkerReceivingInfo; workerId++) /* received partial info computed by workers */
        {      
            PartFileInfo partfileinforeceived;

            MPI_Recv(&partfileinforeceived, sizeof(PartFileInfo), MPI_BYTE, workerId, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            savePartialResults(fileCurrentlyProcessed, partfileinfos, partfileinforeceived);
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
        char buf[MAX_BYTES_TO_READ+MAX_SIZE_WORD];
        PartFileInfo partfileinfo;

        MPI_Recv(buf, MAX_BYTES_TO_READ+MAX_SIZE_WORD , MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        MPI_Recv(&partfileinfo, sizeof(PartFileInfo), MPI_BYTE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        processDataChunk(buf, &partfileinfo);

        MPI_Send(&partfileinfo, sizeof(PartFileInfo), MPI_BYTE, 0, 0, MPI_COMM_WORLD);

        printf("Number words: %d\n", partfileinfo.n_words);

    }
    

}

/**
 *  \brief Main function.
 *
 *  Main thread that runs the program / executes the processes.
 * 
 *  @param argc
 *  @param argv
 * 
 */
int main(int argc, char **argv) 
{

    int rank;
    int size;  

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    nWorkers = size - 1; /* number of processes/workers */


    if (rank == 0) /* dispatcher */
    { 

        char *filenames[argc-2];                         /* file names */

        for(int i=1; i<argc; i++) /* get file names */
        {  
            filenames[i-1] = argv[i];
        }

        dispatcher(filenames, argc - 1);

    }
    else
    {
        worker();
    }

    MPI_Finalize();
    return EXIT_SUCCESS;
}

