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
    
    bool workToBeDone = true;       /* to control the end of work */
    int lastWorkerReceivingInfo = 1;       /* last worker that received a chunk of data to process */
    char buf[MAX_BYTES_TO_READ+MAX_SIZE_WORD];
    
    loadFilesInfo(nFiles, filenames);

    while(workToBeDone)
    {  
        /* send infos to the workers in a parallelized way */
        for (workerId=1; workerId <= nWorkers; workerId++) 
        {   

            if (getDataChunk(buf) == 1) {
                workToBeDone = false;
                break; 
            };

            lastWorkerReceivingInfo = workerId;

            /* tell worker there is work to be done */
            MPI_Send(&workToBeDone, 1, MPI_C_BOOL, workerId, 0, MPI_COMM_WORLD);

            MPI_Send(buf, MAX_BYTES_TO_READ+MAX_SIZE_WORD, MPI_CHAR, workerId, 0, MPI_COMM_WORLD);
        
        }

        for (workerId=1; workerId <= lastWorkerReceivingInfo; workerId++) /* received partial info computed by workers */
        {      

            if(!workToBeDone) break;

            PartFileInfo partfileinforeceived;

            MPI_Recv(&partfileinforeceived, sizeof(PartFileInfo), MPI_BYTE, workerId, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            
            //printf("%d",partfileinforeceived.counting_array[0][1]); ESTE ARRAY TA TODO BUGADO, NEM PASSA

            savePartialResults(partfileinforeceived);
       
        }
 
    }
 
    for (int i = 1; i <= nWorkers; i++) {
        //printf("entrei");
        MPI_Send(&workToBeDone, 1, MPI_C_BOOL, i, 0, MPI_COMM_WORLD);       /* tell workers there is no more work to be done */
    }

    printProcessingResults();
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
    char buf[MAX_BYTES_TO_READ+MAX_SIZE_WORD];
    PartFileInfo partfileinfo;
    
    while (true)
    {
        MPI_Recv(&workToBeDone, 1, MPI_C_BOOL, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        if (!workToBeDone)  /* no more work to be done by workers */
        {     
            //printf("Worker with rank %d terminated...\n", rank);
            return;
        }

        partfileinfo.fileId = 0; /* initialize variables */
        partfileinfo.n_words = 0;
        partfileinfo.n_chars = 0;
        partfileinfo.n_consonants = 0;
        partfileinfo.in_word = 0;
        partfileinfo.max_chars = 0;
        partfileinfo.done = false;
        partfileinfo.firstProcessing = false;
        
        MPI_Recv(buf, MAX_BYTES_TO_READ+MAX_SIZE_WORD , MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        processDataChunk(buf, &partfileinfo);

        MPI_Send(&partfileinfo, sizeof(PartFileInfo), MPI_BYTE, 0, 0, MPI_COMM_WORLD);

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

