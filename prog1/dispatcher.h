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



extern void loadFilesInfo(int nFiles, char *filenames[]);
extern int getDataChunk(char *buf);
extern void savePartialResults(PartFileInfo partfileinfo);
extern void printProcessingResults();

#endif