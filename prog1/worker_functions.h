/**
 *  \file worker_functions.h (functions file)
 *
 *  \brief Problem: Assignment 2 - MPI (circular cross-correlation)
 *
 *  Functions used to make the counting of the characters and other calculations in each worker
 *
 *  \author Alina Yanchuk e Ana Sofia Moniz Fernandes
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <wchar.h>
#include <locale.h>


/**
 *  \brief Function is_vowel.
 *
 *   check if is vowel
 * 
 *  @param c char to be checked
 *  @return 1 if it is vowel, 0 otherwise.
 *
 */
int is_vowel(unsigned char c)
{
    if(c=='a' || c=='e' || c=='i' || c=='o' || c=='u' || c=='y')
    {
        return 1;
    }
    else if(c=='A' || c=='E' || c=='I' || c=='O' || c=='U' || c=='Y')
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/**
 *  \brief Function is_alpha_underscore.
 *
 *   check if is alpha or underscore
 * 
 *  @param c char to be checked
 *  @return 1 if it is alpha or underscore, 0 otherwise.
 *
 */
int is_alpha_underscore(unsigned char c)
{
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) 
    {
        return 1;
    }
    else if((c>='0')&&(c<='9'))
    {
        return 1;
    }
    else if(c=='_')
    {
        return 1;
    }
    else{
        return 0;
    }
}

/**
 *  \brief Function is_space_separation_punctuation.
 *
 *   check if is space, separation or punctuation. 
 * 
 *  @param c char to be checked
 *  @return 1 if it is space, separation or punctuation, 0 otherwise.
 *
 */
int is_space_separation_punctuation(unsigned char c)
{
    if(c==' ' || c==0xa){ /* space */
        return 1;
    }
    else if ((c=='-') || (c=='"') || (c=='[')||(c==']')||(c=='(')||(c==')')) /* separation */
    { 
        return 1;
    }
    else if(c=='.' || c == ',' || c==':' || c==';' || c == '?' || c =='!') /* punctuation */
    { 
        return 1;
    }
    else{
        return 0;
    }
}

/**
 *  \brief Function is_apostrophe.
 *
 *   check if is apostrophe. 
 * 
 *  @param c char to be checked
 *  @return 1 if it is apostrophe, 0 otherwise.
 *
 */
int is_apostrophe(unsigned char c)
{
    if(c==0x27)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/**
 *  \brief Function size_of_array.
 *
 *   Returns size of an array
 * 
 *  @param c array to be checked
 *  @return i, the size of the array
 *
 */
int size_of_array(char *char_array)
{
    int i = 0;
    while (char_array[i] != NULL) 
    {
        i++;
    }
    return i;
}