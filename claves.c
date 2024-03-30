// implementacion de servicios
#include <stdio.h>
#include <netdb.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "claves.h"


int init(){
    
    // Crear el socket 
    int sd;
    struct sockaddr_in server_addr;
    struct hostent *hp;
    sd = socket(AF_INET, SOCK_STREAM, 0);

    // Especificar una direccion para el socket 
    struct sockaddr_in server_addr;
    bzero((char *)&server_addr, sizeof(server_addr)); // Asegurarse de que no haya basura en la estructura antes de usarla
    server_addr.sin_family = AF_INET;
    server_addr.sin_port   = htons(4200);
    return 0;
    
}

int set_value(int key, char *value1, int N_value2, double *V_value2){
    
    
    return 0;
}

int get_value(int key, char *value1, int *N_value2, double *V_value2){
    
    
    return 0;
}

int modify_value(int key, char *value1, int N_value2, double *V_value2){
    
    
    return 0;
}

int delete_key(int key){
    
    
    return 0;
}

int exist(int key){
    
    
    return 0;
}

int copy_key(int key1, int key2){
    
    
    return 0;
}

