/**
 *  \file worker.c
 *
 *  \brief Problem: Assignment 2 - MPI (circular cross-correlation)
 *
 *  Implements all the methods that will be called by the worker.
 *
 *  \author Alina Yanchuk e Ana Sofia Moniz Fernandes
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <ctype.h>
#include <libgen.h>
#include <string.h>

#include "worker_functions.h"
#include "partfileinfo.h"

/**
 *  \brief Function processDataChunk.
 *
 *  Processing of a data chunk. The approach given by the professor was followed:
 *      -If in_word is false:
 *          -if the char is alphanumeric or underscore, in_word is set to true, and we increment the total words, total
 *              num of chars and the number of consonants
 *          -if the char is space/separation/punctiation, in_word remains set to false
 *      -If in_word is true:
 *          -if the char is alpha or underscore, we increment chars and consonants;
 *          -if char is space/separation/punctiation we update the word couting and return
 *
 * @param buf contains a data chunk - set of chars that form complete words
 * @param partialInfo will store the partial info computed
 */

void processDataChunk(char *buf, PartFileInfo *partialInfo) 
{

    int buf_size = size_of_array(buf);

    for(int i=0; i<buf_size;i++)
    {
        char converted_char = buf[i];
    
        if((*partialInfo).in_word==0)
        {
            if(is_alpha_underscore(converted_char))
            {
                (*partialInfo).in_word = 1;
                (*partialInfo).n_words++;
                (*partialInfo).n_chars++;
                (*partialInfo).n_consonants = (*partialInfo).n_consonants + !is_vowel(converted_char);
            }
            else if(is_space_separation_punctuation(converted_char))
            {
                return;
            }
        }
        else
        {
            if(is_alpha_underscore(converted_char))
            {
                (*partialInfo).n_chars++;
                (*partialInfo).n_consonants = (*partialInfo).n_consonants + !is_vowel(converted_char);
            }
            else if(is_space_separation_punctuation(converted_char))
            {
                (*partialInfo).counting_array[(*partialInfo).n_chars-1][(*partialInfo).n_consonants]++;
                
                if((*partialInfo).n_chars > (*partialInfo).max_chars)
                {
                    (*partialInfo).max_chars = (*partialInfo).n_chars;
                }
                return;
            }
        }
    }

}