#CC=/usr/bin/cc
CC=/usr/bin/gcc
LIBS= lib.o
FLAGS= -lm -lssl -lcrypto -g

default: lib.o
	$(CC) main.c -o algoritmo3 $(LIBS) $(FLAGS)

lib.o:
	$(CC) -c lib.c -o lib.o $(FLAGS)
	
err: 00err.cpp lib.o
	$(CC) 00err.c -o 00err $(LIBS) $(FLAGS)

run-uno:
	echo "233\n3\n0.5" | ./algoritmo3

clean:
	rm *.o
	rm algoritmo3
	rm *.txt
	rm 00err