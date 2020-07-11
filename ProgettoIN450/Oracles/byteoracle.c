#include <stdlib.h>
#include <stdio.h>

int main(int argc, char* argv[]){
    int k = atoi(argv[1]);
    char command[217], out[2*k];
    FILE *fp;
    
    if(k<=48){
        
        // Creazione dell'hash a 384 bits
        sprintf(command, "./Oracles/Sslprog/sha3-384 %s", argv[2]);
        system(command);
        
        // Lettura dei soli byte che mi interessano dell'hash completo e salvataggio in stringa interna
        fp = fopen("Oracles/Sslprog/output.txt", "r");
        fgets(out, 2*k+1, (FILE*)fp);
        //printf("%s\n", out);
        fclose(fp);
        
        // Salvataggio dell'hash accorciato in un altro file esterno
        fp = fopen("Oracles/bytehash.txt", "w+");
        fprintf(fp, "%s", out);
        fclose(fp);
        
    }else
        printf("ERRORE: immissione errata del numero di byte\n");
    
    
    return(0);
}
