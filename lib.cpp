#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <time.h>
#include <math.h>
#include <openssl/sha.h>

// -------------------------------------------------- Hash Computing --------------------------------------------------

void printhash(int n, unsigned char* h){
    int i;
    
    for(i=0;i<n;i++)
        printf("%02x", h[i]);
    fflush(stdout);
}

unsigned char* randhexstring(int n){
    int i;
    unsigned char *s;
    
    s = (unsigned char*)malloc(sizeof(unsigned char)*(n+1));
    
    for(i = 0; i<n; i++)
        s[i] = (rand()%255) +1;
    s[n] = '\0';
    
    return(s);
}

unsigned char* copyhexstring(unsigned char* in, int n){
    unsigned char* out;
    
    out = (unsigned char*) malloc(sizeof(char)*(n+1));
    memcpy(out, in, n);
    out[n] = '\0';
    
    return(out);
}

unsigned char* bytehash(int n, unsigned char in[]){
    unsigned char temp[20];
    unsigned char *out;
    int i;
    char *s;
    
    SHA1(in, strlen( (const char*)in ), temp);
    
    out = (unsigned char*) malloc(sizeof(unsigned char)*(n+1));
    for(i=0; i<n; i++)
        out[i] = temp[i];
    out[n] = '\0';
    
    return(out);
}

void chainhash (unsigned char** s, unsigned char** h, int hashlength, unsigned long long chainlength) {
    unsigned long long j;
    
    for(j = 0; j < chainlength; j++){
        *h = bytehash(hashlength, *s);
        free(*s);
        *s = *h;
    }
}

int find00(int n, unsigned char* h){
    int i, f=0;
    
    for(i=0;i<n;i++)
        if(h[i] == 0)
            f=1;
    
    return(f);
}


// ---------------------------------------------- Sorting and searching -----------------------------------------------


void swap(unsigned char **vec, long long unsigned k, long long unsigned h){
    unsigned char *temp;
    
    temp = vec[k];
    vec[k] = vec[h];
    vec[h] = temp;
    
}


int partition (unsigned char **vec,unsigned char **vec2,unsigned char **vec3,long long unsigned low,long long unsigned high, int n){
    // pivot (Element to be placed at right position)
    long long unsigned i, j, pivot;
    pivot = high;
    
    i = (low - 1);  // Index of smaller element

    for (j = low; j <= high- 1; j++){
        // If current element is smaller than the pivot
        if (memcmp(vec[j], vec[pivot], n)<0)/*(arr[j] < pivot)*/
        {
            i++;    // increment index of smaller element
            swap(vec, i, j);
            swap(vec2, i, j);      //arr[i] and arr[j]
            if (vec3 != NULL)
                swap(vec3, i, j);
        }
    }
//     swap(vec, vec2, vec3, i+1, high); //arr[i + 1] and arr[high])
    swap(vec, i, j);
    swap(vec2, i, j);      //arr[i + 1] and arr[high])
    if (vec3 != NULL)
        swap(vec3, i, j);
    
    return (i + 1);
}


/* low  --> Starting index,  high  --> Ending index */
void quickSort(unsigned char **vec,unsigned char **vec2,unsigned char **vec3,long long unsigned low,long long unsigned high, int n){
    long long unsigned pi;
    
    //printf("Q*, %llu, %llu\n", low, high);
    
    if(low < high && high != -1){ // siccome high è long long unsigned va messo il -1 a parte
        //printf("entrato, %d\n", low<high);
        /* pi is partitioning index, arr[pi] is now
           at right place */
        pi = partition(vec, vec2, vec3, low, high, n);
        //printf("low = %llu pi = %llu high = %llu\n", low, pi, high);
        
        quickSort(vec, vec2, vec3, low, pi - 1, n);  // Before pi
        quickSort(vec, vec2, vec3, pi + 1, high, n); // After pi
    }
}

/* low  --> Starting index,  high  --> Ending index */
void quickSort2(unsigned char **vec,unsigned char **vec2,long long unsigned low,long long unsigned high, int n){
    long long unsigned pi;
    
    
    if(low < high && high != -1){ // siccome high è long long unsigned va messo il -1 a parte
        //printf("entrato, %d\n", low<high);
        /* pi is partitioning index, arr[pi] is now
           at right place */
//         pi = partition2(vec, vec2, low, high, n);
        pi = partition(vec, vec2, NULL, low, high, n);
        
        quickSort2(vec, vec2, low, pi - 1, n);  // Before pi
        quickSort2(vec, vec2, pi + 1, high, n); // After pi
    }
}

int ricerca(unsigned char **vec,unsigned char *elem, long long unsigned start, long long unsigned stop, long long unsigned* r, int n){
    
    int flag = 0, val;
    long long unsigned med;
    
    
    if(start != stop && start+1 != stop){
        //qui entra
        med = start + ((stop-start)/2);
        val = memcmp(elem, vec[med], n);
        if(val != 0){
            if(val > 0){
                flag = ricerca(vec, elem, med, stop, r, n);
            }else{
                flag = ricerca(vec, elem, start, med, r, n);
            }
        }else{
            flag = 1;
            *r = med;
        }
    }else{
        if((val = memcmp(elem, vec[start], n) )== 0){
            flag = 1;
            *r = start;
        }
    }
    
    return(flag);
}

// ------------------------------------------------- Output's prints --------------------------------------------------

void run_time_output (unsigned long long round, unsigned long long max_round) {
    if( max_round >= 50){
        if( round % ( max_round/50 ) == 0 ){
            printf("#");
            fflush(stdout);
        }
    }else{
        printf("1");
        fflush(stdout);
    }
}

void run_time_output_end (void) {
    printf("]  Fatto!\n");
}
