CC = gcc
CFLAGS = -Wall  
DEPS = claves.h list.h

all: cliente servidor libclaves.so

cliente: cliente.o libclaves.so
	$(CC) $(CFLAGS) -o cliente cliente.o -L. -lclaves -pthread 

servidor: servidor.o libclaves.so
	$(CC) $(CFLAGS) -o servidor servidor.o -L. -lclaves -pthread 

cliente.o: cliente.c claves.h 
	$(CC) $(CFLAGS) -c cliente.c

servidor.o: servidor.c claves.h 
	$(CC) $(CFLAGS) -c servidor.c

claves.o: claves.c claves.h
	$(CC) $(CFLAGS) -fPIC -c claves.c -o claves.o

send-recv.o: send-recv.c
	$(CC) $(CFLAGS) -fPIC -c send-recv.c -o send-recv.o

libclaves.so: claves.o send-recv.o
	$(CC) $(CFLAGS) -shared -o libclaves.so claves.o send-recv.o


clean:
	rm -f cliente servidor libclaves.so *.o
