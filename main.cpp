#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <time.h>
#include <math.h>
#include <openssl/sha.h>
#include "lib.h"

// ========================== Main function ===========================
int main(void) {
    int seed, n, f;
    unsigned char *s, *h, *s0, *s1, *h1=NULL;
    unsigned long long N, Na, Nb, Ng, r, i, j, k;
    int long long memory;
    double a, b, g;
    
    // Per salvataggio su file
    FILE *fp;
    
    // Per output di riassunto
    char *collisione3[3];
    int collision3flag = 0, collisioncount = 0, repcnt1 = 0, repcnt2 = 0;
    time_t time0, time1, time2;
    
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
    N = (int)pow(256, n);
    Na = (int)(pow(N, a)*(3));
    g = (1 - a)/2;
    Ng = (int)pow(N, g);
    b = 1 - a;
    Nb = (int)(pow(N, b)*(3));
    printf("N = %llu   Na = %llu   Ng = %llu   Nb = %llu\n", N, Na, Ng, Nb);
    
    
    time0 = time(NULL);
    
    // Creazione del primo insieme
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
    printf("\nCostruendo Start[] e End[]...\n[");
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
//     printf("]  Fatto!\n");
    run_time_output_end();
    
    /**
     * In this step we sort the elements in @param End for speeading up the incoming procedure by a binary search.
     * (obviously, also @param Start must be swapped as @param End, otherwise the relation between @param Start[i]
     * and @param End[i] will be lost).
     * 
     * @todo elliminare quickSort2 ed utilizzare solamente quickSort con NULL al terzo vettore
     */
    printf("Ordinando Start[] e End[]...");
    fflush(stdout);
    quickSort2(End, Start, 0, Na-1, n);
//     quickSort(End, Start, NULL, 0, Na-1, n);
    printf("  Fatto!\n");
    
    
    // Costruzione del secondo gruppo e delle collisioni
    /**
     * ---------------------------------------------------- Step 2 ----------------------------------------------------
     * 
     * During this while cycle we peoduce other random hex-strings and computing a chain of @param Ng hashes. Afther
     * every hash computed we serch if it is already included in @param End; when this last condition occures, we can
     * derivate from @param Start[i] a 2-collision and store it in @param Img @param Pr1, and @param Pr2.
     * 
     * We repete this process untill @param Na collisions are found.
     */
    printf("Costruzione delle collisioni...\n[");
    i = 0;
    while(i < Na){
        s0 = randhexstring(n);
        s = copyhexstring(s0, n);
        
        for(j=1; j<=Ng; j++){
            
            chainhash (&s, &h, n, 1);
            
            f = 0;
            f = ricerca(End, s, 0, Na-1, &r, n);
            if(f == 1){
                
                free(s); // non mi serve più tenerlo in memoria (nota: h,s puntano 
                         // la stessa parte di memoria)
                s1 = copyhexstring(Start[r], n);
                
                
                chainhash (&s1, &h1, n, Ng-j);
                
                
                if(memcmp(s0,s1,n)!=0){
                    s = copyhexstring(s0, n);
                    
                    h = bytehash(n, s);
                    h1 = bytehash(n, s1);
                    
                    
                    k=0;
                    while(memcmp(h,h1,n)!=0 && k<j+1 && find00(n,h1)!=0 && find00(n,h1)!=0){
                        
                        free(s);
                        free(s1);
                        s = h;
                        s1 = h1;
                        h = bytehash(n, s);
                        h1 = bytehash(n, s1);
                        k++;
                        
                    }
                    
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
                j = Ng;// Exit from teh j-loop
            }
        }
    }
    run_time_output_end();
    
    
    /**
     * Ordering @param Img (and applying the same swaps to @param Pr1 and @param Pr2) to can use a binary search.
     */
    printf("Ordinando Img[], Pr1[] e Pr2[]...  ");
    fflush(stdout);
    quickSort(Img, Pr1, Pr2, 0, Na-1, n);
    printf("Fatto!\n");
    printf("\n\n");
    
    
    /**
     * ---------------------------------------------------- Step 3 ----------------------------------------------------
     * 
     * In this step we compute @param Nb random hex-strings @param s and them hash @param h until we found a 
     * 3-collision by searching @param h in @param Img.
     */
    time1 = time(NULL);
    if(collision3flag==0){
        printf("Computing %llu operations...\n[", Nb);
        for(i=0; i<Nb && collision3flag==0; i++){
            
            /// Generation of the random hex-string and his hash
            s = randhexstring(n);
            h = bytehash(n, s);
            
            /// Binary search of @param h inside @param Img
            f = 0;
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
