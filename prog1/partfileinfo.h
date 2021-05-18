/**
 *  \file partfileinfo.h
 *
 *  \brief Problem: Assignment 2 - MPI (circular cross-correlation)
 *
 *  Struct for problem 1.
 *
 *  \author Alina Yanchuk e Ana Sofia Moniz Fernandes
 */

#ifndef PARTFILEINFO_H_
#define PARTFILEINFO_H_

/** \brief struct to store data of one file*/
typedef struct {
   int  fileId;    /* file with data */  
   int n_words;    /* number words */
   int n_chars; /* number chars */
   int n_consonants;    /* number consonants */
   int in_word;     /* to control the reading of a word */
   int max_chars;   /* max chars found in a word */
   int **counting_array;    /*  to store and process the final countings */
   bool done;        /* to control the end of processing */ 
} PartFileInfo;

#endif