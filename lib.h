void printhash(int n, unsigned char* h);

unsigned char* randhexstring(int n);

unsigned char* bytehash(int n, unsigned char in[]);

int find00(int n, unsigned char* h);

//      Sorting e ricerca

/* low  --> Starting index,  high  --> Ending index */
void quickSort(unsigned char **vec,unsigned char **vec2,unsigned char **vec3,long long unsigned low,long long unsigned high, int n);
/* low  --> Starting index,  high  --> Ending index */
void quickSort2(unsigned char **vec,unsigned char **vec2,long long unsigned low,long long unsigned high, int n);

int ricerca(unsigned char **vec,unsigned char *elem, long long unsigned start, long long unsigned stop, long long unsigned* r, int n);
