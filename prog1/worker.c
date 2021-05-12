#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>
#include <ctype.h>
#include <libgen.h>
#include "partfileinfo.h"
#include <string.h>

void process_data(PARTFILEINFO *partfileinfos) {
        wchar_t c;
        partfileinfos->n_words = 0;
        partfileinfos->n_chars = 0;
        partfileinfos->n_consonants = 0;
        partfileinfos->in_word = 0;
        partfileinfos->max_chars = 0;
        partfileinfos->counting_array = (int **)calloc(50, sizeof(int *));
		for (int j = 0; j<50; j++){
			partfileinfos->counting_array[j] = (int *)calloc(j+2, sizeof(int));
		}

        for (int i = 0; i < partfileinfos->n_chars_readen; i++) {
            c = partfileinfos->chars_read[i]; /* next char in file */
            char converted_char = convert_multibyte(c);
            if(!partfileinfos->in_word){
                if(is_alpha_underscore(converted_char)){
                    partfileinfos->in_word = 1;
                    partfileinfos->n_words++;
                    partfileinfos->n_chars++;
                    partfileinfos->n_consonants = partfileinfos->n_consonants + !is_vowel(converted_char);
                }
                else if(is_apostrophe(converted_char) || is_space_separation_punctuation(converted_char)){
                    continue;
                }
            }
            else{
                if(is_alpha_underscore(converted_char)){
                    partfileinfos->n_chars++;
                    partfileinfos->n_consonants = partfileinfos->n_consonants + !is_vowel(converted_char);
                }
                else if(is_apostrophe(converted_char)){
                    continue;
                }
                else if(is_space_separation_punctuation(converted_char)){
                    partfileinfos->in_word = 0;
                    partfileinfos->counting_array[partfileinfos->n_chars-1][partfileinfos->n_consonants]++;
                    if(partfileinfos->n_chars > partfileinfos->max_chars){
                        partfileinfos->max_chars = partfileinfos->n_chars;
                    }
                    partfileinfos->n_chars = 0;
                    partfileinfos->n_consonants = 0;
                }
            }

        
        }

        
}