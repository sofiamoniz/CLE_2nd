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



extern void loadFilesInfo(int nFiles, char *filenames[], PartFileInfo *partfileinfos);
extern void getDataChunk(int fileCurrenltyProcessed, PartFileInfo *partfileinfos, char *buf);
extern void printProcessingResults(PartFileInfo *partfileinfos);

#endif