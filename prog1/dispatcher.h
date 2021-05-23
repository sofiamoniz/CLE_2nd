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
#include "partfileinfo.h"

#ifndef DISPATCHER_H
#define DISPATCHER_H


/** \brief The dispatcher loads all necessary data to the PartFileInfo struct, for each file.  */
extern void loadFilesInfo(int nFiles, char *filenames[]);

/** \brief Obtain next data chunk (buffer) of the current file being processed.  */
extern int getDataChunk(char *buf);

/** \brief Makes all the final calculations and prints the final results.  */
extern void savePartialResults(PartFileInfo partfileinfo);

/** \brief Makes all the final calculations and prints the final results.  */
extern void printProcessingResults();

#endif