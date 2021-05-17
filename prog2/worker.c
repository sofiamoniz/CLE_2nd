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
#include <math.h>

/**
 *  \brief Function computeValue.
 *
 *  Formula to calculate the circular cross-correlation of a point
 *
 */
double computeValue(int nElements, double X[], double Y[], int point) 
{
    double value = 0.0;               /* will be the result for this point */
    
    for (int k = 0; k<nElements; k++) 
        value = value + (X[k] * Y[(point+k) % nElements]);              /* formula */
    
    return value;
}

