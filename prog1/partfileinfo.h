/**
 *  \file partfileinfo.h
 *
 *  \brief Problem: Assignment 2 - MPI (circular cross-correlation)
 *
 *  Struct used to store partial infos.
 *
 *  \author Alina Yanchuk e Ana Sofia Moniz Fernandes
 */

#include <stdbool.h>

#ifndef PARTFILEINFO_H
#define PARTFILEINFO_H

/** \brief struct to store data of one file*/
typedef struct {
   int  fileId;    /* file with data */  
   int n_words;    /* number words */
   int n_chars; /* number chars */
   int n_consonants;    /* number consonants */
   int in_word;     /* to control the reading of a word */
   int max_chars;   /* max chars found in a word */
   int counting_array[50][51];    /*  to store and process the final countings  -> counting_array[MAX_SIZE_WORD][MAX_SIZE_WORD+1]*/
   bool firstProcessing; /* indicates wether it is the first time processing that file or not */
   bool done;        /* to control the end of processing */ 
} PartFileInfo;

#endif