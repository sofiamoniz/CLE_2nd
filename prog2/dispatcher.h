 /**
 *  \file dispatcher.h
 *
 *  \brief Problem: Assignment 2 - MPI (circular cross-correlation)
 *
 *  Dispatcher header file.
 *
 *  \author Alina Yanchuk e Ana Sofia Moniz Fernandes
 */

#include <stdbool.h>

#ifndef DISPATCHER_H
#define DISPATCHER_H

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

void loadFilesInfo(int nFiles, char *filenames[], Signal *signals);

extern void savePartialResult(int workerId, int fileId, Signal *signals, double val, int point);

extern void checkProcessingResults(int nFiles, Signal *signals);

#endif
