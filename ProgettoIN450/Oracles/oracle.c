#include <stdlib.h>
#include <stdio.h>

int main(int argc, char* argv[]){
    int k = atoi(argv[1]), h; //In h inserisco il numero di bit dell'hash che ho calcolato
    char command[217];
    
    // Calcolo degli hash con l'algoritmo selezionato
    if(k==1){
        sprintf(command, "./Oracles/Sslprog/md5 %s", argv[2]);
        system(command);
        h = 128;
    }else{
        if(k==2){
            sprintf(command, "./Oracles/Sslprog/sha1 %s", argv[2]);
            system(command);
            h = 160;
        }else{
            if(k==3){
                sprintf(command, "./Oracles/Sslprog/sha256 %s", argv[2]);
                system(command);
                h = 256;
            }else{
                if(k==4){
                    sprintf(command, "./Oracles/Sslprog/sha3-384 %s", argv[2]);
                    system(command);
                    h = 384;
                }else{
                 printf("errere di immissione\n");
                }    
            }
            
        }
        
    }
    
    char out[h/4];
    FILE *fp;
    
    // Lettura dei soli byte che mi interessano dell'hash completo e salvataggio in stringa interna
    fp = fopen("Oracles/Sslprog/output.txt", "r");
    fgets(out, h/4+1, (FILE*)fp);
    //printf("%s\n", out);
    fclose(fp);
        
    // Salvataggio dell'hash accorciato in un altro file esterno
    fp = fopen("Oracles/hash.txt", "w+");
    fprintf(fp, "%s", out);
    fclose(fp);
    
    return(0);
}
