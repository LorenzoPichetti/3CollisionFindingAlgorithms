#CC=/usr/bin/cc
CC=/usr/bin/gcc
LIBS= lib.o
FLAGS= -lm -lssl -lcrypto -g

default: lib.o
	$(CC) new_alg.c -o new_alg $(LIBS) $(FLAGS)
	$(CC) naive_alg.c -o naive_alg $(LIBS) $(FLAGS)

lib.o:
	$(CC) -c lib.c -o lib.o $(FLAGS)
	
err: 00err.cpp lib.o
	$(CC) 00err.c -o 00err $(LIBS) $(FLAGS)

run-uno:
	echo "233\n3\n0.5" | ./algoritmo3

clean:
	rm *.o
	rm new_alg
	rm naive_alg
	rm *.txt
	rm 00err
