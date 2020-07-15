#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <time.h>
#include <math.h>
#include <openssl/sha.h>
#include "lib.h"

int main(void) {
    int seed, n, f, cond1, cond2, div1=0, div2=0;
    unsigned char *s, *h;
    unsigned long long N, Na, Nb, r, i, j;
    int long long memory;
    double a, b;
    time_t seconds;
    
    // Per output di riassunto
    unsigned char *collisione3[3];
    int collision3flag = 0, collisioncount = 0, arepcnt = 0, brepcnt = 0, repcnt2 = 0;
    time_t time0, time1, time2, time3;
    FILE *fp;
    
    int PE = 8, midindex, rank;
    
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
    N = (int)pow(256, n);
    Na = (int)(pow(N, a)*(8));
    Na = (Na/PE +1)*PE;
    b = (2 - a)/2;
    Nb = (int)(pow(N, b)*(3));
    
    // Per output di computazione
    cond1 = (Na>=50);
    cond2 = (Nb>=50);
    if(cond1)
        div1 = Na/50;
    if(cond2)
        div2 = Nb/50;
    
    time0 = time(NULL);
    
    // Step 1 algoritmo
    unsigned char *Img[Na], *Pr1[Na], *Pr2[Na];
    printf("Calcolando %llu operazioni per costruire i vettori...\n[", Na);
    fflush(stdout);
    
    
    for(i=0; i<Na; i++){
        s = randhexstring(n);
        h = bytehash(n, s);
  
        Img[i] = h;
        Pr1[i] = s;
        Pr2[i] = NULL;
            
        if(cond1){
            if(i%div1==0){
                printf("#");
                fflush(stdout);
            }
        }else{
            printf("1");
            fflush(stdout);
        }
        
    }
    printf("] eseguite!\n");
    
    printf("Ordinando i vettori...  ");
    fflush(stdout);
    quickSort(Img, Pr1, Pr2, 0, Na -1, n);
    printf("Ordinati!\n");
    
    time1 = time(NULL);
    
    
    // Passo opzionale
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
                        collisione3[0] = Pr1[i];
                        collisione3[1] = Pr2[i];
                        collisione3[2] = Pr1[j];
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
    
    time2 = time(NULL);
    
    // Step 2 algoritmo
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
                        collisione3[0] = Pr1[r];
                        collisione3[1] = Pr2[r];
                        collisione3[2] = s;
                        
                        printf("Trovata 3-collisione: ");
                        printhash(n, Pr1[r]);
                        printf(" ");
                        printhash(n, Pr2[r]);
                        printf(" ");
                        printhash(n, s);
                        printf("\n");
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
    printf("] Eseguiti %llu calcoli\n", i);
    time3 = time(NULL);
    
    // Riassunto computazione
    memory = ((sizeof(char)*(n+1))+sizeof(char*))*Na*3;
    
    printf("------------------------------------------------------------------------------\n");
    printf("Riassunto:\n\tseme di generazione: %d\n\tbyte: %d\n\tN = %llu\n", seed, n, N);
    printf("\ta = %lf\tNa = %llu\n\tb = %lf\tNb = %llu\n\n", a, Na, b, Nb);
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
    printf("Il calcolo è durato %ld secondi:\n\tIl passo 1: %ld\n\tIl passo opzionale: %ld\n\tIl passo 2: %ld\n", time3-time0, time1-time0, time2-time1, time3-time2);
    printf("Ci sono state %d ripetizioni nella generazione e %d nel calcolo\n", arepcnt,brepcnt+repcnt2);
    printf("Sono stati usati circa %lld byte di memoria\n", memory);
    printf("------------------------------------------------------------------------------\n");
    
    // Terzo salvataggio su file
    fp = fopen("Storico1.txt", "a");
    fprintf(fp, "%d,%d,%lf,%llu,%llu,%llu,%ld,%ld,%lld,",seed,n/2,a,N,Na,Nb,time1-time0,time3-time2,memory);
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
