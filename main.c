/*
 *  mm_test.c
 *
 *  A simple program for your custom malloc. Only the most basic cases are tested, 
 *  you are encouraged to add more test cases to ensure your functions work correctly. 
 *  Having everything in this file working does not indicate that you have fully
 *  completed the assignment. You need to implement everything specified in the brief.
 *  
 *  'passed' does not indicate that the function works entirely correctly, only that
 *  the operation did not create fatal errors
 */

#include "mm_alloc.h"
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>

int
main(int argc, char **argv)
{
    int *data,i;
    double *ddata;
    bool passed;

    printf("Basic malloc....");
    ddata = (double*) mm_malloc(sizeof(double));
    *ddata = 12345.6789;
    printf("%s\n",(*ddata)==12345.6789?"'passed'":"failed!");

    printf("Array malloc....");  
    data = (int*) mm_malloc(1028*sizeof(int));
    for(i = 0; i < 1028; i++) data[i] = i;
    passed = true;
    for(i = 0; i < 1028; i++) passed &= (data[i]==i);
    printf("%s\n",passed?"'passed'":"failed!");
    
    printf("Basic free......");  
    mm_free(ddata);
    printf("'passed'\n");

    printf("Array free......");  
    mm_free(data);
    printf("'passed'\n");
    
    printf("Basic realloc...");
    ddata = (double*) mm_malloc(sizeof(double));
    *ddata = 12345.6789;
    double* old = ddata;
    ddata = (double*) mm_realloc(ddata,1000*sizeof(double));
    passed = ((old<ddata)&&((*ddata)==12345.6789));
    mm_free(ddata);
    printf("%s\n",passed?"'passed'":"failed!");
    
    return 0;
}