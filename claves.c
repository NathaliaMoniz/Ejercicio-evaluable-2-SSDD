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
#include <stdio.h> 
#include <stdlib.h>


int init(){

    // Obtener la variable de entorno IP_TUPLAS
    char *ip_tuplas; 
 
    ip_tuplas = getenv("IP_TUPLAS"); 
    if (ip_tuplas == NULL){ 
        printf("La variable de entorno IP_TUPLAS no está definida.\n"); 
        return 0; 
    } 
    else 
        printf("Variable IP_TUPLAS definida con valor %s\n", ip_tuplas); 



    // Obtener la variable de entorno PORT_TUPLAS
    char *port_tuplas_str; 
 
    port_tuplas_str = getenv("IP_TUPLAS"); 
    if (port_tuplas_str == NULL){ 
        printf("La variable de entorno PORT_TUPLAS no está definida.\n"); 
        return 0; 
    } 
    else 
        printf("Variable PORT_TUPLAS definida con valor %s\n", port_tuplas_str); 


    // Convertir el valor de la variable de entorno PORT_TUPLAS a entero
    int port_tuplas = atoi(port_tuplas_str);

    // Crear el socket 
    int client_sd;
    struct sockaddr_in server_addr;
    struct hostent *hp;
    client_sd = socket(AF_INET, SOCK_STREAM, 0);

    if (client_sd == -1) {
		printf("Error al crear el socket\n");
		return -1;
	}

    // Especificar una direccion para el socket 
    struct sockaddr_in server_addr;
    bzero((char *)&server_addr, sizeof(server_addr)); // Asegurarse de que no haya basura en la estructura antes de usarla
    server_addr.sin_family = AF_INET;
    server_addr.sin_port   = htons(port_tuplas);
    hp = gethostbyname (ip_tuplas);

    if (hp == NULL) {
		printf("Error en gethostbyname\n");
		return -1;
	}

    memcpy (&(server_addr.sin_addr), hp->h_addr, hp->h_length);

    // Iniciar la conexión
    int connection_status = connect(client_sd, (struct sockaddr *) &server_addr, sizeof(server_addr));

    if (connection_status == -1) {
		printf("Error en la conexión\n");
		return -1;
	}

    char op = 0; // Operación de init

    // Enviar la op
    int send_status = send(client_sd, (char*) &op, sizeof(char), 0);
    if (send_status == -1){
		printf("Error en el envio\n");
		return -1;
	}

    // Recibir respuesta del servidor 
    int32_t res; 
    int recv_status = recv(client_sd, (char *) &res, sizeof(int32_t), 0);   
    if (recv_status == -1){
		printf("Error en recepción\n");
		return -1;
	}

    // Imprimir la respuesta 
	printf("El resultado es %d \n", ntohl(res));
    
    // Cerrar el socket 
   	close (client_sd);
    return 0;
    
}

int set_value(int key, char *value1, int N_value2, double *V_value2){
    
    // Obtener la variable de entorno IP_TUPLAS
    char *ip_tuplas; 
 
    ip_tuplas = getenv("IP_TUPLAS"); 
    if (ip_tuplas == NULL){ 
            printf("La variable de entorno IP_TUPLAS no está definida.\n"); 
            return 0; 
    } 
    else 
            printf("Variable IP_TUPLAS definida con valor %s\n", ip_tuplas); 


    // Obtener la variable de entorno PORT_TUPLAS
    char *port_tuplas_str; 
 
    port_tuplas_str = getenv("IP_TUPLAS"); 
    if (port_tuplas_str == NULL){ 
            printf("La variable de entorno PORT_TUPLAS no está definida.\n"); 
            return 0; 
    } 
    else 
            printf("Variable PORT_TUPLAS definida con valor %s\n", port_tuplas_str); 

    return 0;

    // Convertir el valor de la variable de entorno PORT_TUPLAS a entero
    int port_tuplas = atoi(port_tuplas_str);

    // Crear el socket 
    int client_sd;
    struct sockaddr_in server_addr;
    struct hostent *hp;
    client_sd = socket(AF_INET, SOCK_STREAM, 0);

    if (client_sd == -1) {
		printf("Error al crear el socket\n");
		return -1;
	}

    // Especificar una direccion para el socket 
    struct sockaddr_in server_addr;
    bzero((char *)&server_addr, sizeof(server_addr)); // Asegurarse de que no haya basura en la estructura antes de usarla
    server_addr.sin_family = AF_INET;
    server_addr.sin_port   = htons(port_tuplas);
    hp = gethostbyname (ip_tuplas);

    if (hp == NULL) {
		printf("Error en gethostbyname\n");
		return -1;
	}

    memcpy (&(server_addr.sin_addr), hp->h_addr, hp->h_length);

    // Iniciar la conexión
    int connection_status = connect(client_sd, (struct sockaddr *) &server_addr, sizeof(server_addr));

    if (connection_status == -1) {
		printf("Error en la conexión\n");
		return -1;
	}

    char op = 0; // Operación de init

    // Enviar la op
    int send_status = send(client_sd, (char*) &op, sizeof(char), 0);
    if (send_status == -1){
		printf("Error en el envio\n");
		return -1;
	}

    // Recibir respuesta del servidor 
    int32_t res; 
    int recv_status = recv(client_sd, (char *) &res, sizeof(int32_t), 0);   
    if (recv_status == -1){
		printf("Error en recepción\n");
		return -1;
	}

    // Imprimir la respuesta 
	printf("El resultado es %d \n", ntohl(res));
    
    // Cerrar el socket 
   	close (client_sd);
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

