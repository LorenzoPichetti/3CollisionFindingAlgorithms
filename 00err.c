#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <time.h>
#include <math.h>
#include <openssl/sha.h>
#include "lib.h"

int main(int argc, char* argv[])
{
    int seed = atoi(argv[2]), i, n = atoi(argv[1]), k;
    char *s, *h;
    
    srand(seed);
    
    
    for(i=0;i<50;i++){
        s = randhexstring(n);
        if((i/2)%2==0){
            k = rand()%n;
            s[k] = 0;
        }
        h = bytehash(n, s);
        
        
        printhash(n, s);
        printf("-->");
        printhash(n, h);
        printf(" %d\n", find00(n,s));
    }
    
    
    return 0;
}
