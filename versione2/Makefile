#CC=/usr/bin/cc
CC=/usr/bin/gcc
LIBS=lib.c -lm -lssl -lcrypto

default: uno tre

uno: algoritmo1.c
	$(CC) algoritmo1.c -o algoritmo1 $(LIBS)

tre: algoritmo3.c
	$(CC) algoritmo3.c -o algoritmo3 $(LIBS)

err: 00err.c
	$(CC) 00err.c -o 00err $(LIBS)

run-uno:
	echo "233\n3\n0.5" | ./algoritmo1 
