#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <time.h>
#include <math.h>
#include <openssl/sha.h>
#include "lib.h"

#include "omp.h"

#define CORE 4

// ========================== Main function ===========================
int main(void) {
    int seed, n, f, cond1, cond2, div1=0, div2=0;
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
    
    // Per output di computazione
    cond1 = (Na>=50);
    cond2 = (Nb>=50);
    if(cond1)
        div1 = Na/50;
    if(cond2)
        div2 = Nb/50;
    
    time0 = time(NULL);
    
    // Creazione del primo insieme
    unsigned char *Img[Na], *Pr1[Na], *Pr2[Na], *Start[Na], *End[Na];
    
    printf("\nCostruendo Start[] e End[]...\n[");
    
    // NOTE da parallelizzare con OpenMP questo for
    #pragma omp parallel for num_threads(CORE)
    for(i=0; i<Na; i++){
        unsigned char *s, *s0, *h;
        
        s = randhexstring(n);
        s0 = malloc(sizeof(char)*n);
        memcpy(s0, s, n);
        Start[i] = s0;  // Devo allocare una stringa diversa altrimenti al primo ciclo del
                        // prossimo for la cella puntata da Start[i] viene liberata
        for(j=0; j<Ng; j++){
            h = bytehash(n, s);
            free(s);
            s = h;
        }
        End[i] = s; // Non mi serve un'altra stringa perchè all'entrata del prossimo ciclo
                    // il valore di s verrà cambiato senza liberare la cella.
        
        if(Na>=50){
            if(i%(Na/50)==0){
                printf("#");
                fflush(stdout);
            }
        }else{
            printf("1");
            fflush(stdout);
        }
    }
    
    printf("]  Fatto!\n");
    printf("Ordinando Start[] e End[]...");
    fflush(stdout);
    quickSort2(End, Start, 0, Na-1, n);
    printf("  Fatto!\n");
    
    /*
    for(i=0;i<Na;i++){
        printhash(n, Start[i]);
        printf("-%llu->", Ng);
        printhash(n, End[i]);
        printf("\n");
    }*/
            
    
    // Costruzione del secondo gruppo e delle collisioni
    // NOTE da parallelizzare con OpenMP questo while
    printf("Costruzione delle collisioni...\n[");
//     i = 0;
//     while(i < Na){ // NOTE per for anziché while
    
    //#pragma omp parallel for num_threads(CORE)
    for (i=0; i < Na; i++) {
        unsigned char *s, *s0, *h, *s1, *h1=NULL;
        unsigned long long r;
        int f;
        
        s0 = randhexstring(n);
        s = malloc(sizeof(char)*(n+1));
        memcpy(s, s0, n);
        s[n] = '\0';
//         printf("ciao %s\n", s);
        
        
        for(j=1; j<=Ng; j++){
            
            h = bytehash(n, s);
            free(s);
            s = h;
            //printhash(n, s);
            
            f = 0;
            f = ricerca(End, s, 0, Na-1, &r, n);
            //printf("%d\n", f);
            if(f == 1){
                
                /*printf("element found: j:%llu, s0:", j);
                printhash(n, s0);
                printf(", End[r]");
                printhash(n, End[r]);
                printf("\n");*/
                
                free(s); // non mi serve più tenerlo in memoria (nota: h,s puntano 
                         // la stessa parte di memoria)
                s1 = malloc(sizeof(char)*(n+1));
                memcpy(s1, Start[r], n);
                s1[n] = '\0';
                
                /*printhash(n, Start[r]);
                printf(":Start[r], s1:");
                printhash(n, s1);*/
                
                for(k=1; k<= Ng -j; k++){
                    //printhash(0, bytehash(0, "ciao"));
                    
                    h1 = bytehash(n, s1);
                    free(s1);
                    s1 = h1;
                    
                    //printf("->");
                    //printhash(n, s1);
                    
                }
                //printf(" uscito\n");
                
                
                if(memcmp(s0,s1,n)!=0){
                    s = malloc(sizeof(char)*(n+1));
                    memcpy(s, s0, n);
                    s[n] = '\0';
                    
                    /*printhash(n, s);
                    printf(":s s1:");
                    printhash(n, s1);
                    printf("->");*/
                    
                    h = bytehash(n, s);
                    h1 = bytehash(n, s1);
                    
                    /*printhash(n, h);
                    printf(":h h1:");
                    printhash(n, h1);
                    printf("->");*/
                    
                    
                    k=0;
                    while(memcmp(h,h1,n)!=0 &&k<j+1&&find00(n,h1)!=0&&find00(n,h1)!=0){
                        
                        free(s);
                        free(s1);
                        s = h;
                        s1 = h1;
                        h = bytehash(n, s);
                        h1 = bytehash(n, s1);
                        k++;
                        
                        /*printhash(n, h);
                        printf(",");
                        printhash(n, h1);
                        printf("->");*/
                        
                    }
                    
                    if(memcmp(h,h1,n)==0){
                        if(find00(n,s)!=1&&find00(n,s1)!=1){
                            /*printhash(n, s);
                            printhash(n, s1);
                            printf(":scritto nelle 2-collisioni\n");*/
                            
                            Img[i] = h;
                            Pr1[i] = s;
                            Pr2[i] = s1;
//                             i++;// NOTE per for anziché while
                            
                            if(cond1){
                                if(i%div1==0){
                                    printf("#");
                                    fflush(stdout);
                                }
                            }else{
                                printf("1");
                                fflush(stdout);
                            }
                        }else{
                            i--; // NOTE per for anziché while
                        }
                    }else{
                        //printf("00err\n");
                    }
                }
                j = Ng;// Exit loop on j
            }else{
                i--; // NOTE per for anziché while
            }
        }
    }
    printf("]  Fatto!\n");
    
    printf("Ordinando Img[], Pr1[] e Pr2[]...  ");
    fflush(stdout);
    quickSort(Img, Pr1, Pr2, 0, Na-1, n);
    printf("Fatto!\n");
    
    printf("\n\n");
    
    time1 = time(NULL);
    
    // Step 2 algoritmo
    if(collision3flag==0){
        printf("Computing %llu operations...\n[", Nb);
        for(i=0; i<Nb && collision3flag==0; i++){
            
            //printhash(0, bytehash(0, "ciao"));
            
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
                            collisione3[0] = Pr1[r];
                            collisione3[1] = Pr2[r];
                            collisione3[2] = s;
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
            
            if(cond2){
                if(i%div2==0){
                    printf("#");
                    fflush(stdout);
                }
            }else{
                printf("1");
                fflush(stdout);
            }
        }
        printf("Eseguiti %llu calcoli\n", i);
    }
    
    
    if(collision3flag == 1){
            printf("Trovata 3-collisione al passo %llu!\n", i+1);
            for(i=0;i<3;i++){
                printhash(n, collisione3[i]);
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
                printhash(n, collisione3[i]);
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
