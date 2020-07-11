#include <stdlib.h>
#include <stdio.h>

int main(int argc, char* argv[]){
    char out[500];
    
    FILE *fp;

    fp = fopen("Oracles/Sslprog/input.txt", "w+");
    fprintf(fp, "%s", argv[1]);
    fclose(fp);
    
    system("openssl dgst -sha256 -r -out Oracles/Sslprog/output.txt Oracles/Sslprog/input.txt");
    //system("openssl dgst -sha256 Sslprog/input.txt");
    
    return(0);
    
}
