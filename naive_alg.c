/*
 * Copyright 2022 Lorenzo Pichetti lori.pichi@gmail.com
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <time.h>
#include <math.h>
#include <openssl/sha.h>
#include "lib.h"

// ================================================== Main function ===================================================
int main(void) {
    
    /**
     * ---------------------------------------------- Initializing Step -----------------------------------------------
     * 
     * We use @param seed, @param n, and @param a as input parameters. The user will insert the random generation seed,
     * the byte-lenght of the considered hash, and the algorithm's parameter @param a between 0 and 2/3.
     * 
     * @remark in this implementation the considered hash will be computed as the firsts @param n SHA1's bytes.
     * @remark the @param f is a flag used during step 2.
     */
    int seed, n;  double a;
    
    // Scelta del seme di generazione
    printf("Inserisci il seme di generazione casuale: ");
    scanf("%d", &seed);
    srand(seed);
    
    // Pre-computazione
    printf("Inserisci il numero di byte dell'hash: ");
    scanf("%d", &n);
    
    //n = n*2; //Trasformo il numero di byte in numero di cifre esadecimali
    printf("Inserisci il valore a tra 0 e 0.6666: ");
    scanf("%lf", &a);
    
    /**
     * We now define some variables for the final computation's output. In @param collision3 and @param collision3flag
     * we check if a 3-collision is found and store it, and in the @param timeX (with X in {0, 1, 2, 3}) we store the 
     * computation's times.
     * Finally, we use @param collisioncount, @param arepcnt, @param brepcnt, and @param repcnt2 to count the number of
     * repetitions and collisions found during the algorithm.
     */
    unsigned char *collision3[3];
    int collision3flag = 0, collisioncount = 0, arepcnt = 0, brepcnt = 0, repcnt2 = 0;
    time_t time0, time1, time2, time3;
    int long long memory;
    
    /**
     * We now compute from @param n and @param a the algorithm's parameters @param N, @param b, @param Na, and @param Nb.
     * @remark we enlarge the Na and Nb numbers by a constant factor of 8.
     * 
     * The variable @param i, @param j, @param k, and @param r will be used as cycles variables or indices.
     */
    unsigned long long N, Na, Nb, r, i, j;
    double b;
    
    b = (2 - a)/2;
    N = (int)pow(256, n);
    Na = ((int)(pow(N, a)))*8;
    Nb = ((int)(pow(N, b + 0.09)))*3;
    
    /**
     * @param f, @param s, and @param h will be used during the computation as temporary variables. Instead, @param Img,
     * @param Pr1, and @param Pr2 are the vector where we store the collisions.
     */
    unsigned char *Img[Na], *Pr1[Na], *Pr2[Na];
    unsigned char *s, *h;
    int f;
    
    
    /** Step 1 algoritmo
     * We now start the step 1 where we compute @param Na hashes and put them images in @param Img[i] and them preimages
     * in @param Pr1[i].
     */
    time0 = time(NULL);
    printf("Calcolando %llu operazioni per costruire i vettori...\n[", Na);
    fflush(stdout);
    
    
    for(i=0; i<Na; i++){
        s = randhexstring(n);
        h = bytehash(n, s);
  
        Img[i] = h;
        Pr1[i] = s;
        Pr2[i] = NULL;
            
        run_time_output (i, Na);
    }
    run_time_output_end (1);
    
    /* We now swap the position of @param Img and @param Pr1 by sorting Img (to be able to use a binary search over 
     * it in the step 2 and speed it up).
     */ 
    printf("Ordinando i vettori...  ");
    fflush(stdout);
    quickSort(Img, Pr1, Pr2, 0, Na -1, n);
    printf("Ordinati!\n");
    
    /**
     * In the optional step we check if some of the Na hashes computed did generate a 2-collision
     */
    time1 = time(NULL);
    printf("Ricercando le collisioni in generazione...\n");
    i=0;
    while(i<Na){
        j = i +1;
        while(memcmp(Img[i],Img[j], n)==0){
            if(memcmp(Pr1[i],Pr1[j], n)!=0){
                if(Pr2[i]==NULL){
                    Pr2[i] = Pr1[j];
                    collisioncount++;
                }else{
                    if(memcmp(Pr2[i],Pr1[j], n)!=0){
                        collision3flag = 1;
                        collision3[0] = Pr1[i];
                        collision3[1] = Pr2[i];
                        collision3[2] = Pr1[j];
                    }else{
                        repcnt2++;
                    }
                }
            }else{
                arepcnt++; 
            }
            j++;
        }
        i = j;
    }
    printf("Trovate %d collisioni in generazione.\n", collisioncount);
    
    
    /**
     * Step 2: in this step we generate @param Nb hashes and, if them collide with one of the first Na, update 
     * @param Pr1 until we find a 3-collision.
     */
    time2 = time(NULL);
    printf("Calcolando %llu operazioni...\n[", Nb);
    for(i=0; i<Nb && collision3flag==0; i++){
        
        printhash(0, bytehash(0, "ciao"));
        
        s = randhexstring(n);
        h = bytehash(n, s);
        
        f = 0;
        f = ricerca(Img, h, 0, Na-1, &r, n);
        if(f==1){
            if(memcmp(Pr1[r],s,n)!=0){
                if(Pr2[r]==NULL){
                    Pr2[r] = s;
                    collisioncount++;
                }else{
                    if(memcmp(Pr2[r],s,n)!=0){
                        collision3flag = 1;
                        collision3[0] = Pr1[r];
                        collision3[1] = Pr2[r];
                        collision3[2] = s;
                    }else{
                        repcnt2 ++;
                        free(s);
                    }
                }
            }else{
                brepcnt++;
                free(s);
            }
        }else{        
            free(s);
        }
        free(h);
        
        run_time_output (i, Nb);
    }
    printf("] Eseguiti %llu calcoli\n", i);
    if (collision3flag == 1) {
        printf("Trovata 3-collisione: ");
        printhash(n, Pr1[r]);
        printf(" ");
        printhash(n, Pr2[r]);
        printf(" ");
        printhash(n, s);
        printf("\n");
    }
    time3 = time(NULL);
    
    /**
     * ----------------------------------------------- Printing Results -----------------------------------------------
     * 
     * We now print and @return all the informations about computation: result, time elapsed and memory used.
     * Furthermore, we also print the parameters choiced at the and of computation with the relative value of Na, Nb...
     */
    memory = ((sizeof(char)*(n+1))+sizeof(char*))*Na*3;
    
    printf("------------------------------------------------------------------------------\n");
    printf("Riassunto:\n\tseme di generazione: %d\n\tbyte: %d\n\tN = %llu\n", seed, n, N);
    printf("\ta = %lf\tNa = %llu\n\tb = %lf\tNb = %llu\n\n", a, Na, b, Nb);
    if(collision3flag == 1){
            printf("Trovata 3-collisione al passo %llu!\n", i+1);
            for(i=0;i<3;i++){
                printhash(n, collision3[i]);
                printf(" ");
            }
            printf("\n");
    }else{
        printf("Nessuna 3-collisione trovata :(\n");
    }
    printf("Il calcolo è durato %ld secondi:\n\tIl passo 1: %ld\n\tIl passo opzionale: %ld\n\tIl passo 2: %ld\n", time3-time0, time1-time0, time2-time1, time3-time2);
    printf("Ci sono state %d ripetizioni nella generazione e %d nel calcolo\n", arepcnt,brepcnt+repcnt2);
    printf("Sono stati usati circa %lld byte di memoria\n", memory);
    printf("------------------------------------------------------------------------------\n");
    
    /**
     * Before ending the program, we store the results in the output file 'Storico1.txt' using the parameter @param fp.
     */
    FILE *fp;
    fp = fopen("Storico1.txt", "a");
    fprintf(fp, "%d,%d,%lf,%llu,%llu,%llu,%ld,%ld,%lld,",seed,n/2,a,N,Na,Nb,time1-time0,time3-time2,memory);
    if(collision3flag == 1){
            for(i=0;i<2;i++){
                fprintf(fp,"%s,", collision3[i]);
            }
            fprintf(fp,"%s;\n",collision3[2]);
    }else{
        fprintf(fp,"-1,-1,-1;\n");
    }
    fclose(fp);
    
    return(0);
}
