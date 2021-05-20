/**
 *  \file worker.h
 *
 *  \brief Problem: Assignment 2 - MPI (circular cross-correlation)
 *
 *  Worker header file.
 *
 *  \author Alina Yanchuk e Ana Sofia Moniz Fernandes
 */

#include "partfileinfo.h"

#ifndef WORKER_H
#define WORKER_H

 /** \brief Processing of a data chunk. The approach given by the professor was followed. */
extern void processDataChunk(char *buf, PartFileInfo partialInfo); 

#endif
