CC = gcc
CFLAGS = -Wall  
DEPS = claves.h list.h

all: cliente servidor libclaves.so

cliente: cliente.o libclaves.so
	$(CC) $(CFLAGS) -o cliente cliente.o -L. -lclaves

servidor: servidor.o libclaves.so
	$(CC) $(CFLAGS) -o servidor servidor.o -L. -lclaves

cliente.o: cliente.c claves.h message.h
	$(CC) $(CFLAGS) -c cliente.c

servidor.o: servidor.c claves.h message.h
	$(CC) $(CFLAGS) -c servidor.c

claves.o: claves.c claves.h
	$(CC) $(CFLAGS) -fPIC -c claves.c -o claves.o

libclaves.so: claves.o
	$(CC) $(CFLAGS) -shared -o libclaves.so claves.o

clean:
	rm -f cliente servidor libclaves.so *.o
