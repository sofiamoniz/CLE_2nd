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



extern void loadFilesInfo(int nFiles, char *filenames[], PartFileInfo *partfileinfos);
extern void getDataChunk(int fileCurrenltyProcessed, PartFileInfo *partfileinfos, char *buf);
void savePartialResults(int fileCurrentlyProcessed, PartFileInfo *partfileinfos, PartFileInfo partfileinforeceived);
extern void printProcessingResults(PartFileInfo *partfileinfos);

#endif