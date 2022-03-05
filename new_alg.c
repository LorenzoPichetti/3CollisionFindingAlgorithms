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
#include "lib.h"

// ================================================== Main function ===================================================
int main(void) {
    
    /**
     * ---------------------------------------------- Initializing Step -----------------------------------------------
     * 
     * We use @param seed, @param n, and @param a as input parameters. The user will insert the random generation seed,
     * the byte-lenght of the considered hash, and the algorithm's parameter @param a between 0 and 1/3.
     * 
     * @remark in this implementation the considered hash will be computed as the firsts @param n SHA1's bytes.
     * @remark the @param f is a flag used during step 2.
     */
    int seed, n, f;
    double a;
    
    // Scelta del seme di generazione
    printf("Inserisci il seme di generazione casuale: ");
    scanf("%d", &seed);
    srand(seed);
    
    // Pre-computazione
    printf("Inserisci il numero di byte dell'hash: ");
    scanf("%d", &n);
    //n = n*2; //Trasformo il numero di byte in numero di cifre esadecimali
    printf("Inserisci il valore a tra 0 e 0.3333: ");
    scanf("%lf", &a);
    
    /**
     * We now compute from @param n and @param a the algorithm's parameters @param N, @param b, @param g and
     * the relative parameters @param Na, @param Nb, and @param Ng.
     * 
     * The variable @param i, @param j, @param k, and @param r will be used as cycles variables or indices.
     */
    double b, g;
    unsigned long long N, Na, Nb, Ng, r, i, j, k;
    
    N = (int)pow(256, n);
    Na = (int)(pow(N, a)*(3));
    g = (1 - a)/2;
    Ng = (int)pow(N, g);
    b = 1 - a;
    Nb = (int)(pow(N, b)*(3));
    printf("N = %llu   Na = %llu   Ng = %llu   Nb = %llu\n\n", N, Na, Ng, Nb);
    
    
    /**
     * We use the following variables for storeing the compuation's informations and print them afther them
     * end of the algorithm.
     * 
     * @param collisione3 and @param collision3flag are used for store the result and as stop condition.
     * @param time0, @param time1, @param time2 are used for computing the elapsed time and @param memory for space.
     * @param collisioncount, @param repcnt1, and @param repcnt2 are used for counting the false collisions found.
     * @param fp is used for generating the output file ' @todo '. 
     */
    FILE *fp;
    char *collisione3[3];
    int long long memory;
    time_t time0, time1, time2;
    int collision3flag = 0, collisioncount = 0, repcnt1 = 0, repcnt2 = 0;
    
    
    /**
     * All the hashes are computed and stored by using the 'openssl/sha.h' library, so are typed as 'unsigned char *'.
     * We then use @param s, @param s0, @param s1, @param h, and @param h1 as temporary valuse and @param Start,
     * @param End, @param Img, @param Pr1, and @param Pr2 as arraies of hashes.
     */
    unsigned char *s, *h, *s0, *s1, *h1=NULL;
    unsigned char *Img[Na], *Pr1[Na], *Pr2[Na], *Start[Na], *End[Na];
    
    
    /**
     * ---------------------------------------------------- Step 1 ----------------------------------------------------
     * 
     * In this cycles we create the @param Start and @param End arraies:
     *  1) We generate @param Na random hex-strings and put them in @param Start
     *  2) For every random element Start[i] we set (@param s) = Start[i] and iteratively upedate s = hash(s)
     *     for @param Ng times (by using the cycle's variable @param j).
     *  3) We put the final result in @param s in @param End[i].
     */
    time0 = time(NULL);
    run_time_output_start ("Costruendo Start[] e End[]...", 1);
    for(i=0; i<Na; i++){
        s0 = randhexstring(n);
        Start[i] = s0; 
        
        s = copyhexstring(s0, n);   // Devo allocare una stringa diversa altrimenti al primo ciclo del
        chainhash (&s, &h, n, Ng);  // prossimo for la cella puntata da Start[i] viene liberata
        
        End[i] = s; // Non mi serve un'altra stringa perchè all'entrata del prossimo ciclo
                    // il valore di s verrà cambiato senza liberare la cella.
                    
        /// Operation for the run-time output during the computation
        run_time_output(i, Na);
    }
    run_time_output_end(1);
    
    /**
     * 
     * In this step we sort the elements in @param End for speeading up the incoming procedure by a binary search.
     * (obviously, also @param Start must be swapped as @param End, otherwise the relation between @param Start[i]
     * and @param End[i] will be lost).
     * 
     */

    run_time_output_start ("Ordinando Start[] e End[]...", 0);
    quickSort(End, Start, NULL, 0, Na-1, n);
    run_time_output_end (0);
    
    
    /**
     * ---------------------------------------------------- Step 2 ----------------------------------------------------
     * 
     * During this while cycle we peoduce other random hex-strings and computing a chain of @param Ng hashes. Afther
     * every hash computed we serch if it is already included in @param End; when this last condition occures, we can
     * derivate from @param Start[i] a 2-collision and store it in @param Img @param Pr1, and @param Pr2.
     * 
     * We repete this process untill @param Na collisions are found.
     */
    run_time_output_start ("Costruzione delle collisioni...", 1);
    i = 0;
    while(i < Na){
        s0 = randhexstring(n);
        s = copyhexstring(s0, n);
        
        for(j=1; j<=Ng; j++){
            
            chainhash (&s, &h, n, 1);
            
            f = ricerca(End, s, 0, Na-1, &r, n);
            if(f == 1){
                
                /** Since the hash in @param s is equal to the one in @param End[r] we can freed the memory pointed by
                 *  @param s and @param h (that is the same) and set them as NULL for save us from dangling pointer.
                 */ 
                free(s); s = NULL; h = NULL;
                
                /**
                 * We now compute the (@param Ng - @param j)-th element of the hash chain starting on @param Start[r].
                 * By construction we have that @param s0 and @param s1 are such that afther an hashchain of lenght j
                 * they have the same output.
                 */
                s1 = copyhexstring(Start[r], n);
                chainhash (&s1, &h1, n, Ng-j);
                
                /**
                 * Now, if s0 and s1 are different, we can generate a 2-collision and increment the @param i of the 
                 * while cycle, otherwise, we restart the @param j 'for' cycle without incrementing @param i.
                 */
                if( memcmp(s0,s1,n) != 0 ) {
                    s = copyhexstring(s0, n);
                    
                    /**
                     * Until @param s and @param s1 have different hashes, we update them with their hashes. We are
                     * sure that in at most @param j operations one 2-collision occures.
                     */
                    k=0;
                    h = bytehash(n, s);
                    h1 = bytehash(n, s1);
                    
                    while(memcmp(h,h1,n)!=0 && k<j+1 && find00(n,h1)!=0 && find00(n,h1)!=0){
                        
                        free(s);
                        free(s1);
                        s = h;
                        s1 = h1;
                        h = bytehash(n, s);
                        h1 = bytehash(n, s1);
                        k++;
                        
                    }
                    
                    /**
                     * When this collision occures we put the hash (@param h) = (@param h1) in @param Img[i] and the
                     * two preimages @param s and @param s1 in @param Pr1[i] and @param Pr2[i] (and we increment i).
                     */
                    if(memcmp(h,h1,n)==0){
                        if(find00(n,s)!=1 && find00(n,s1)!=1) {
                            
                            Img[i] = h;
                            Pr1[i] = s;
                            Pr2[i] = s1;
                            i++;
                            
                            run_time_output(i, Na);
                        }
                    }
                }
                j = Ng;     // Exit from teh j-loop
                f = 0;      // Reinitialize the flag f
            }
        }
    }
    run_time_output_end(1);
    
    
    /**
     * Ordering @param Img (and applying the same swaps to @param Pr1 and @param Pr2) to can use a binary search.
     */
    run_time_output_start ("Ordinando Img[], Pr1[] e Pr2[]...  ", 0);
    quickSort(Img, Pr1, Pr2, 0, Na-1, n);
    run_time_output_end (0);
    printf("\n\n");
    
    
    /**
     * ---------------------------------------------------- Step 3 ----------------------------------------------------
     * 
     * In this step we compute @param Nb random hex-strings @param s and them hash @param h until we found a 
     * 3-collision by searching @param h in @param Img.
     */
    time1 = time(NULL);
    if(collision3flag == 0) {
        printf("Computing %llu operations...\n[", Nb);
        for(i=0; i<Nb && collision3flag==0; i++){
            
            /// Generation of the random hex-string and his hash
            s = randhexstring(n);
            h = bytehash(n, s);
            
            /// Binary search of @param h inside @param Img
            f = ricerca(Img, h, 0, Na-1, &r, n);
            
            /**
             * If we found @param h == @param Img[r] we check if @param s is equal to @param @Pr1[r] or @param Pr2[r],
             * otherwise we have found a 3-collision and we stop the 'for' cycle.
             */
            if ( f==1 ){
                if(memcmp(Pr1[r],s,n)!=0){
                    if(Pr2[r]==NULL){
                        Pr2[r] = s;
                        collisioncount++;
                    }else{
                        if(memcmp(Pr2[r],s,n)!=0){
                            
                            printf("]  trovato!: ");
                            printhash(n, Pr1[r]);
                            printf(", ");
                            printhash(n, Pr2[r]);
                            printf(", ");
                            printhash(n, s);
                            printf(" --> ");
                            printhash(n, h);
                            printf("\n");
                            
                            collision3flag = 1;
                            collisione3[0] = (char*) Pr1[r];
                            collisione3[1] = (char*) Pr2[r];
                            collisione3[2] = (char*) s;
                        }else{
                            repcnt2 ++;
                            free(s);
                        }
                    }
                }else{
                    repcnt2++;
                    free(s);
                }
                f = 0;
            }else{        
                free(s);
            }
            free(h);
            
            run_time_output(i, Nb);
        }
        printf("Eseguiti %llu calcoli\n", i);
    }
    
    
    /**
     * ----------------------------------------------- Printing Results -----------------------------------------------
     * 
     * We now print and @return all the informations about computation: result, time elapsed and memory used.
     * Furthermore, we also print the parameters choiced at the and of computation with the relative value of Na, Nb...
     */
    if(collision3flag == 1){
            printf("Trovata 3-collisione al passo %llu!\n", i+1);
            for(i=0;i<3;i++){
                printhash(n, (unsigned char*) collisione3[i]);
                printf(" ");
            }
            printf("\n");
    }else{
        printf("Nessuna 3-collisione trovata :(\n");
    }
    time2 = time(NULL);
    
    // Riassunto computazione
    memory = ((sizeof(char)*(n+1))+sizeof(char*))*Na*5;
    
    printf("\n------------------------------------------------------------------------------\n");
    printf("Riassunto:\n\tseme di generazione: %d\n\tbyte: %d\n\tN = %llu\n", seed, n, N);
    printf("\ta = %lf\tNa = %llu\n\tb = %lf\tNb = %llu\n\tg = %lf\tNg = %llu\n\n", a, Na, b, Nb, g, Ng);
    
    if(collision3flag == 1){
            printf("Trovata 3-collisione: ");
            for(i=0;i<3;i++){
                printhash(n, (unsigned char*) collisione3[i]);
                printf(" ");
            }
            printf("\n\n");
    }else{
        printf("Nessuna 3-collisione trovata :(\n");
    }
    
    printf("Il calcolo è durato %ld secondi:\n\tIl passo 1: %ld\n\tIl passo 2: %ld\n", time2-time0, time1-time0, time2-time1);
    printf("Ci sono state %d ripetizioni nella generazione e %d nel calcolo\n", repcnt1, repcnt2);
    printf("Sono stati usati circa %lld byte di memoria\n", memory);
    printf("------------------------------------------------------------------------------\n");
    
    fp = fopen("Storico3.txt", "a");
    fprintf(fp, "%d,%d,%lf,%llu,%llu,%llu,%llu,%ld,%ld,%lld,",seed,n/2,a,N,Na,Nb,Ng,time1-time0,time2-time1,memory);
    if(collision3flag == 1){
            for(i=0;i<2;i++){
                fprintf(fp,"%s,", collisione3[i]);
            }
            fprintf(fp,"%s;\n",collisione3[2]);
    }else{
        fprintf(fp,"-1,-1,-1;\n");
    }
    fclose(fp);
    
    
    return(0);
}
