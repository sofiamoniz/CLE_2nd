/**
 *  \file main_functions.h (functions file)
 *
 *  \brief Problem: Assignment 2 - MPI (circular cross-correlation)
 *
 *  Functions used to convert multibyte chars to unibyte and verify if a char is an end of word.
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
 *  \brief Function convert_multibyte.
 *
 *   make conversion multibyte to singlebyte: 
 *   if it's already singlebyte, it breaks
 * 
 *  @param c char to be converted
 *  @return c in singlebyte
 *
 */
char convert_multibyte(wchar_t c)
{
    switch (c) {
        /* left or right single quotion marks to apostrophe */
        case L'`': c=0x27; break;
        case L'’': c=0x27; break;
        case L'‘': c=0x27; break;
        /* á–à–â–ã to a : */
        case L'à': c=L'a'; break;
        case L'á': c=L'a'; break;
        case L'â': c=L'a'; break;
        case L'ã': c=L'a'; break;
        /* Á–À–Â–Ã to A: */
        case L'Á': c=L'A'; break;
        case L'À': c=L'A'; break;
        case L'Â': c=L'A'; break;
        case L'Ã': c=L'A'; break;
        /* é–è–ê to e : */
        case L'è': c=L'e';break;
        case L'é': c=L'e';break;
        case L'ê': c=L'e';break;
        /* É–È–Ê to E: */
        case L'É': c=L'E';break;
        case L'È': c=L'E';break;
        case L'Ê': c=L'E';break;
        /* í–ì to i */
        case L'í': c=L'i';break;
        case L'ì': c=L'i';break;
        /* Í–Ì to I */
        case L'Í': c=L'I';break;
        case L'Ì': c=L'I';break;
        /* ó–ò–ô–õ to o */
        case L'ó': c=L'o'; break;
        case L'ò': c=L'o'; break;
        case L'ô': c=L'o'; break;
        case L'õ': c=L'o'; break;
        /* Ó–Ò–Ô–Õ to O */
        case L'Ó': c=L'O'; break;
        case L'Ò': c=L'O'; break;
        case L'Ô': c=L'O'; break;
        case L'Õ': c=L'O'; break;
        /* ú–ù-ü to u */
        case L'ú': c=L'u';break;
        case L'ù': c=L'u';break;
        case L'ü': c=L'u';break;
        /* Ú–Ù to U */
        case L'Ú': c=L'U';break;
        case L'Ù': c=L'U';break;
        /* Ç–ç to C */
        case L'Ç': c=L'C';break;
        case L'ç': c=L'C';break;
        /* left and right double quotation marks to double quotation mark */
        case L'«': c=L'"';break;
        case L'»': c=L'"';break;

        /* dash to hyphen */
        case L'‒': c=L'-';break;
        case L'–': c=L'-';break;
        case L'—': c=L'-';break;

        /* ellipsis to full point */
        case L'…': c=L'.';break;

        /* double quotion to " */
        case L'”': c=L'"'; break;
        case L'“': c=L'"'; break;

        default:    break;
    }
    return c;
}


/**
 *  \brief Function is_end_of_word.
 *
 *   check if is end of word : space, separation, punctiation or apostrophe.
 * 
 *  @param c char to be checked
 *  @return 1 if it is end of word, 0 otherwise.
 *
 */
int is_end_of_word(unsigned char c)
{
    if(c==' ' || c ==0xa || c==0xa){ /* space */
        return 1;
    }
    else if ((c=='-') || (c=='"') || (c=='[')||(c==']')||(c=='(')||(c==')')){ /* separation */
        return 1;
    }
    else if(c=='.' || c == ',' || c==':' || c==';' || c == '?' || c =='!'){ /* punctuation */
        return 1;
    }
    else{
        return 0;
    }
}

int is_apostrophe_merge(unsigned char c)
{
    if(c==0x27){ /* apostrophe */
        return 1;
    }
    else return 0;
}