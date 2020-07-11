#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <time.h>
#include <math.h>

// ================== Generazione stringhe random ===================
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

// ======================== Calcolo degli Hash =========================
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

// ====================== Soarting e ricerca =======================

void swap(char **vec, char **vec2, char **vec3, long long unsigned k, long long unsigned h){
    char *temp;
    
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
    
    
    if(low < high && high != -1){ // siccome high è long long unsigned va messo il -1 a parte
        //printf("entrato, %d\n", low<high);
        /* pi is partitioning index, arr[pi] is now
           at right place */
        pi = partition(vec, vec2, vec3, low, high, n);
        
        quickSort(vec, vec2, vec3, low, pi - 1, n);  // Before pi
        quickSort(vec, vec2, vec3, pi + 1, high, n); // After pi
    }
}

void swap2(char **vec, char **vec2, long long unsigned k, long long unsigned h){
    char *temp;
    
    temp = vec[k];
    vec[k] = vec[h];
    vec[h] = temp;
    temp = vec2[k];
    vec2[k] = vec2[h];
    vec2[h] = temp;
    
}

int partition2(char **vec,char **vec2, long long unsigned low, long long unsigned high, int n){
    // pivot (Element to be placed at right position)
    long long unsigned i, j, pivot;
    pivot = high;
    
    i = (low - 1);  // Index of smaller element

    for (j = low; j <= high- 1; j++){
        // If current element is smaller than the pivot
        if (memcmp(vec[j], vec[pivot], n)<0)/*(arr[j] < pivot)*/
        {
            i++;    // increment index of smaller element
            swap2(vec, vec2, i, j); //arr[i] and arr[j]
        }
    }
    swap2(vec, vec2, i+1, high); //arr[i + 1] and arr[high])
    return (i + 1);
}

/* low  --> Starting index,  high  --> Ending index */
void quickSort2(char **vec,char **vec2,long long unsigned low,long long unsigned high, int n){
    long long unsigned pi;
    
    
    if(low < high && high != -1){ // siccome high è long long unsigned va messo il -1 a parte
        //printf("entrato, %d\n", low<high);
        /* pi is partitioning index, arr[pi] is now
           at right place */
        pi = partition2(vec, vec2, low, high, n);
        
        quickSort2(vec, vec2, low, pi - 1, n);  // Before pi
        quickSort2(vec, vec2, pi + 1, high, n); // After pi
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


// ========================== Main function ===========================
int main(void) {
    int seed, n, f, cond1, cond2, div1=0, div2=0;
    char *s, *h, *s0, *s1, *h1;
    unsigned long long N, Na, Nb, Ng, r, i, j, k;
    int long long memory;
    double a, b, g;
    
    // Per salvataggio su file
    FILE *fp;
    
    // Per output di riassunto
    char *collisione3[3];
    int collision3flag = 0, collisioncount = 0, repcnt1 = 0, repcnt2 = 0;
    time_t time0, time1, time2, time3;
    
    // Scelta del seme di generazione
    printf("Inserisci il seme di generazione casuale: ");
    scanf("%d", &seed);
    srand(seed);
    
    // Pre-computazione
    printf("Inserisci il numero di byte dell'hash: ");
    scanf("%d", &n);
    n = n*2; //Trasformo il numero di byte in numero di cifre esadecimali
    printf("Inserisci il valore a tra 0 e 0.3333: ");
    scanf("%lf", &a);
    N = (int)pow(16, n);
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
    char *Img[Na], *Pr1[Na], *Pr2[Na], *Start[Na], *End[Na];
    
    printf("\nCostruendo Start[] e End[]...\n[");
    for(i=0; i<Na; i++){
        s = randhexstring(n);
        s0 = malloc(sizeof(char)*(n+1));
        strncpy(s0, s, n);
        s0[n] = '\0';
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
    
    
    // Costruzione del secondo gruppo e delle collisioni
    printf("Costruzione delle collisioni...\n[");
    i = 0;
    while(i < Na){
        s0 = randhexstring(n);
        
        s = malloc(sizeof(char)*(n+1));
        strncpy(s, s0, n);
        s[n] = '\0';
        
        for(j=1; j<=Ng; j++){
            h = bytehash(n, s);
            free(s);
            s = h;
            
            f = 0;
            f = ricerca(End, s, 0, Na-1, &r, n);
            if(f == 1){
                free(s); // non mi serve più tenerlo in memoria (nota: h,s puntano la stessa parte di memoria)
                s1 = malloc(sizeof(char)*(n+1));
                strncpy(s1, Start[r], n);
                s1[n] = '\0';       // Boh, non so perchè solo qui
                //printf("n:%d, len(s1):%ld, s1:%s\n",n, strlen(s1), s1);
                
                for(k=1; k<= Ng -j; k++){
                    h1 = bytehash(n, s1);
                    free(s1);
                    s1 = h1;
                }
                
                if(memcmp(s0,s1,n)!=0){
                    s = malloc(sizeof(char)*(n+1));
                    strncpy(s, s0, n);
                    s[n] = '\0';
                    h = bytehash(n, s);
                    h1 = bytehash(n, s1);
                    
                    while(memcmp(h,h1,n)!=0){
                        free(s);
                        free(s1);
                        s = h;
                        s1 = h1;
                        h = bytehash(n, s);
                        h1 = bytehash(n, s1);
                    }
                    
                    Img[i] = h;
                    Pr1[i] = s;
                    Pr2[i] = s1;
                    i++;
                    
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
                j = Ng;// Exit loop on j
            }
        }
    }
    printf("]  Fatto!\n");
    
    printf("Ordinando Img[], Pr1[] e Pr2[]...  ");
    fflush(stdout);
    quickSort(Img, Pr1, Pr2, 0, Na-1, n);
    printf("Fatto!\n");
    
    printf("\n\n");
    
    // Salvataggio su file delle 2-collisioni
    fp = fopen("2Collisioni.txt", "w+");
    for(i=0; i<Na; i++){
        fprintf(fp, "%s, %s ---> %s\n", Pr1[i], Pr2[i], Img[i]);
    }
    fclose(fp);
    
    
    time1 = time(NULL);
    // Passo opzionale
    
    printf("Ricercando le collisioni in generazione...  ");
    i=0;
    while(i<Na){
        j = i +1;
        while(memcmp(Img[i],Img[j], n)==0){
            if(memcmp(Pr1[i],Pr1[j], n)!=0){
                if(memcmp(Pr2[i],Pr1[j], n)!=0){
                    printf("\n\t\ttrovato!: %s, %s, %s, --> %s", Pr1[i], Pr2[i], Pr1[j], Img[i]);
                    collision3flag = 1;
                    collisione3[0] = Pr1[i];
                    collisione3[1] = Pr2[i];
                    collisione3[2] = Pr1[j];
                }
            }else{
                repcnt1++;
            }
            j++;
        }
        i = j;
    }
    if(collision3flag==0)
        printf("Fatto!\n");
    else
        printf("\n");
    
    time2 = time(NULL);
    
    
    // Step 2 algoritmo
    if(collision3flag==0){
        printf("Computing %llu operations...\n[", Nb);
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
                            printf("]  trovato!: %s, %s, %s --> %s\n", Pr1[r], Pr2[r], s, h);
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
                printf("%s ", collisione3[i]);
            }
            printf("\n");
    }else{
        printf("Nessuna 3-collisione trovata :(\n");
    }
    time3 = time(NULL);
    
    // Riassunto computazione
    memory = ((sizeof(char)*(n+1))+sizeof(char*))*Na*5;
    
    printf("\n------------------------------------------------------------------------------\n");
    printf("Riassunto:\n\tseme di generazione: %d\n\tbyte: %d\n\tN = %llu\n", seed, n/2, N);
    printf("\ta = %lf\tNa = %llu\n\tb = %lf\tNb = %llu\n\tg = %lf\tNg = %llu\n\n", a, Na, b, Nb, g, Ng);
    
    if(collision3flag == 1){
            printf("Trovata 3-collisione: ");
            for(i=0;i<3;i++){
                printf("%s ", collisione3[i]);
            }
            printf("\n\n");
    }else{
        printf("Nessuna 3-collisione trovata :(\n");
    }
    
    printf("Il calcolo è durato %ld secondi:\n\tIl passo 1: %ld\n\tIl passo opzionale: %ld\n\tIl passo 2: %ld\n", time3-time0, time1-time0, time2-time1, time3-time2);
    printf("Ci sono state %d ripetizioni nella generazione e %d nel calcolo\n", repcnt1, repcnt2);
    printf("Sono stati usati circa %lld byte di memoria\n", memory);
    printf("------------------------------------------------------------------------------\n");
    
    fp = fopen("Storico3.txt", "a");
    fprintf(fp, "%d,%d,%lf,%llu,%llu,%llu,%llu,%ld,%ld,%lld,",seed,n/2,a,N,Na,Nb,Ng,time1-time0,time3-time2,memory);
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
