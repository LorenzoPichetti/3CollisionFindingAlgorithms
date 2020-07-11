#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <time.h>
#include <math.h>

char* randhexstring(int n){
    int k, i;
    char *s;
    
    s = malloc(sizeof(char)*(n+1));
    
    for(i = 0; i<n; i++){
        k = rand()%16;
        //printf("%d ", k);
        if(k<10)
            s[i] = k + 48;
        else
            s[i] = k + 87;
    }
    s[i] = '\0';
    
    return(s);
}

char* bytehash(int n, char* in){
    char command[120], *out;
    FILE *fp;
    
    out = malloc(sizeof(char)*(n+1));
    
    sprintf(command, "./Oracles/byteoracle %d %s", n/2, in);
    system(command);
    
    fp = fopen("Oracles/bytehash.txt", "r");
    fgets(out, n+1, (FILE*)fp);
    fclose(fp);
    
    return(out);
}

//      Soarting e ricerca

void swap(char **vec, char **vec2, char **vec3, long long unsigned k, long long unsigned h){
    char *temp;
    
    //printf("est: %s\n", vec[k]);
    //printf("est: %s\n", vec[h]);
    
    temp = vec[k];
    vec[k] = vec[h];
    vec[h] = temp;
    temp = vec2[k];
    vec2[k] = vec2[h];
    vec2[h] = temp;
    temp = vec3[k];
    vec3[k] = vec3[h];
    vec3[h] = temp;
    
}

int partition (char **vec,char **vec2,char **vec3,long long unsigned low,long long unsigned high, int n){
    // pivot (Element to be placed at right position)
    long long unsigned i, j, pivot;
    pivot = high;
    
    i = (low - 1);  // Index of smaller element

    for (j = low; j <= high- 1; j++){
        // If current element is smaller than the pivot
        if (memcmp(vec[j], vec[pivot], n)<0)/*(arr[j] < pivot)*/
        {
            i++;    // increment index of smaller element
            swap(vec, vec2, vec3, i, j); //arr[i] and arr[j]
        }
    }
    swap(vec, vec2, vec3, i+1, high); //arr[i + 1] and arr[high])
    return (i + 1);
}

/* low  --> Starting index,  high  --> Ending index */
void quickSort(char **vec,char **vec2,char **vec3,long long unsigned low,long long unsigned high, int n){
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
    }else{
        //printf("non entrato, %d\n", low<high);
    }
}

int ricerca(char **vec, char *elem, long long unsigned start, long long unsigned stop, long long unsigned* r, int n){
    
    int flag = 0, val;
    long long unsigned med;
    
    
    if(start != stop && start+1!=stop){
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
        if(val = memcmp(elem, vec[start], n) == 0){
            flag = 1;
            *r = start;
        }
    }
    
    return(flag);
}

int main(void) {
    int seed, n, f, cond1, cond2, div1=0, div2=0;
    char *s, *h;
    unsigned long long N, Na, Nb, r, i, j;
    int long long memory;
    double a, b;
    time_t seconds;
    
    // Per output di riassunto
    char *collisione3[3];
    int collision3flag = 0, collisioncount = 0, arepcnt = 0, brepcnt = 0, repcnt2 = 0;
    time_t time0, time1, time2, time3;
    FILE *fp;
    
    // Scelta del seme di generazione
    printf("Inserisci il seme di generazione casuale: ");
    scanf("%d", &seed);
    srand(seed);
    
    // Pre-computazione
    printf("Inserisci il numero di byte dell'hash: ");
    scanf("%d", &n);
    n = n*2; //Trasformo il numero di byte in numero di cifre esadecimali
    printf("Inserisci il valore a tra 0 e 0.6666: ");
    scanf("%lf", &a);
    N = (int)pow(16, n);
    Na = (int)(pow(N, a)*(8));
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
    char *Img[Na], *Pr1[Na], *Pr2[Na];
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
    quickSort(Img, Pr1, Pr2, 0, Na - 1, n);
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
                    //printf("C'è una collisione in generazione per %s\n", Img[i]);
                    collisioncount++;
                }else{
                    if(memcmp(Pr2[i],Pr1[j], n)!=0){
                        printf("\t\ttrovato!: %s, %s, %s, --> %s\n", Pr1[i], Pr2[i], Pr1[j], Img[i]);
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
    printf("Riassunto:\n\tseme di generazione: %d\n\tbyte: %d\n\tN = %llu\n", seed, n/2, N);
    printf("\ta = %lf\tNa = %llu\n\tb = %lf\tNb = %llu\n\n", a, Na, b, Nb);
    if(collision3flag == 1){
            printf("Trovata 3-collisione al passo %llu!\n", i+1);
            for(i=0;i<3;i++){
                printf("%s ", collisione3[i]);
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
