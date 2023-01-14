#include <stdlib.h>
#include <stdio.h>

int main () { 
    int* pointer;
    pointer = NULL;
    printf("%d\n", *pointer);
    free(pointer);
    return 0;
}