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
#include "send-recv.h"

char *port(){
    // Obtener la variable de entorno PORT_TUPLAS
    char *port_tuplas_str; 
 
    port_tuplas_str = getenv("PORT_TUPLAS"); 
    if (port_tuplas_str == NULL){ 
        printf("La variable de entorno PORT_TUPLAS no está definida.\n"); 
        return NULL; 
    } 
    else 
        printf("Variable PORT_TUPLAS definida con valor %s\n", port_tuplas_str); 
    
    
    return port_tuplas_str;
}

char *ip(){
    // Obtener la variable de entorno IP_TUPLAS
    char *ip_tuplas; 
 
    ip_tuplas = getenv("IP_TUPLAS"); 
    if (ip_tuplas == NULL){ 
        printf("La variable de entorno IP_TUPLAS no está definida.\n"); 
        return NULL; 
    } 
    else 
        printf("Variable IP_TUPLAS definida con valor %s\n", ip_tuplas); 

    return ip_tuplas;
}
int init(){
    int send_status;
    char *port_tuplas_str = port();
    char *ip_tuplas = ip();
    int port_tuplas = atoi(port_tuplas_str);

    // Crear el socket 
    int sd;
    struct sockaddr_in server_addr;
    struct hostent *hp;
    sd = socket(AF_INET, SOCK_STREAM, 0);

    if (sd == -1) {
		printf("Error al crear el socket\n");
		return -1;
	}

    // Especificar una direccion para el socket 
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
    int connection_status = connect(sd, (struct sockaddr *) &server_addr, sizeof(server_addr));
    if (connection_status == -1) {
		perror("Error en la conexión\n");
		return -1;
	}
    char op = 0; // Operación de init

    // Enviar la op
    send_status = sendMessage(sd, (char*) &op, sizeof(char));
    if (send_status == -1){
		printf("Error en el envio\n");
		return -1;
	}

    // Recibir respuesta del servidor 
    int32_t res; 
	fflush(stdout);
    int recv_status = recvMessage(sd, (char *) &res, sizeof(int32_t));   
    if (recv_status == -1){
		printf("Error en recepción\n");
		return -1;
	}

    // Imprimir la respuesta 
	printf("\nRespuesta %d \n", ntohl(res));
    
    // Cerrar el socket 
   	close (sd);
    return 0;
}

int set_value(int key, char *value1, int N_value2, double *V_value2){
    int send_status;
    char *port_tuplas_str = port();
    char *ip_tuplas = ip();
    int port_tuplas = atoi(port_tuplas_str);

    // Crear el socket 
    int sd;
    struct sockaddr_in server_addr;
    struct hostent *hp;
    sd = socket(AF_INET, SOCK_STREAM, 0);

    if (sd == -1) {
		printf("Error al crear el socket\n");
		return -1;
	}

    // Especificar una direccion para el socket 
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
    int connection_status = connect(sd, (struct sockaddr *) &server_addr, sizeof(server_addr));
    if (connection_status == -1) {
		perror("Error en la conexión\n");
		return -1;
	}
    
    char op = 1; // Operación de set

    // Enviar la op
    send_status = sendMessage(sd, (char*) &op, sizeof(char));
    if (send_status == -1){
		printf("Error en el envio\n");
		return -1;
	}
   
    // Enviar la key
    int netKey = htonl(key); // Convertir key a formato de red 
    
    send_status = sendMessage(sd, (char*) &netKey, sizeof(int));
    if (send_status == -1){
		printf("Error en el envio de la clave\n");
		return -1;
	}

    // Enviar value1
    send_status = sendMessage(sd, value1, 256);
    if (send_status == -1){
		printf("Error en el envio de value1\n");
		return -1;
	}

    // Enviar N_value2
    int netN_value2 = htonl(N_value2); // Convertir key a formato de red 
    send_status = sendMessage(sd, (char*) &netN_value2, sizeof(int));
    if (send_status == -1){
		printf("Error en el envio de N_value2\n");
		return -1;
	}

    // Enviar V_value2
    for(int i = 0; i < N_value2; i++){
        send_status = sendMessage(sd, (char*) &V_value2[i], sizeof(double));
        if (send_status == -1){
            printf("Error en el envio de V_value2\n");
            return -1;
	    }
    }
    // Recibir respuesta del servidor 
    int32_t res; 
	fflush(stdout);
    int recv_status = recvMessage(sd, (char *) &res, sizeof(int32_t));   
    if (recv_status == -1){
		printf("Error en recepción\n");
		return -1;
	}
    
    // Imprimir la respuesta 
	printf("\nRespuesta %d \n", ntohl(res));
    
    // Cerrar el socket 
   	close (sd);
    return 0;
}

int get_value(int key, char *value1, int *N_value2, double *V_value2){
    int send_status;
    char *port_tuplas_str = port();
    char *ip_tuplas = ip();
    int port_tuplas = atoi(port_tuplas_str);

    // Crear el socket 
    int sd;
    struct sockaddr_in server_addr;
    struct hostent *hp;
    sd = socket(AF_INET, SOCK_STREAM, 0);

    if (sd == -1) {
		printf("Error al crear el socket\n");
		return -1;
	}

    // Especificar una direccion para el socket 
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
    int connection_status = connect(sd, (struct sockaddr *) &server_addr, sizeof(server_addr));
    if (connection_status == -1) {
		perror("Error en la conexión\n");
		return -1;
	}
    
    char op = 2; // Operación de get

    // Enviar la op
    send_status = sendMessage(sd, (char*) &op, sizeof(char));
    if (send_status == -1){
		printf("Error en el envio\n");
		return -1;
	}

    // Enviar la key
    int netKey = htonl(key); // Convertir key a formato de red 
    
    send_status = sendMessage(sd, (char*) &netKey, sizeof(int));
    if (send_status == -1){
		printf("Error en el envio de la clave\n");
		return -1;
	}

    // Recibir value1
    char value1_recibido[256];
    int recv_status = recvMessage(sd, value1_recibido, 256);

    if (recv_status == -1){
		printf("Error en recepción de value1\n");
		return -1;
	}
    strcpy(value1, value1_recibido);
    

    // Recibir N_value2
    int hostN_value2;
    recv_status = recvMessage(sd, (char *) &hostN_value2, sizeof(int));
    if (recv_status == -1){
        printf("Error en recepción de N_value2\n");
        return -1;
    }
    int N_value2_recibido = ntohl(hostN_value2);
    *N_value2 = N_value2_recibido;

    // Recibir V_value2
    double V_value2_recibido[N_value2_recibido];
    for (int i = 0; i < N_value2_recibido; i++) {
        recv_status = recvMessage(sd, (char*)&V_value2_recibido[i], sizeof(double)); // Recibir cada elemento del vector
        V_value2[i] = V_value2_recibido[i];
        if (recv_status == -1) {
            perror("Error en recepción del elemento del vector V_value2\n");
            
            return -1;
        }
    }

    // Recibir respuesta del servidor
    int32_t res; 
    recv_status = recvMessage(sd, (char *) &res, sizeof(int32_t));   
    if (recv_status == -1){
		printf("Error en recepción\n");
		return -1;
	}
    
    // Imprimir la respuesta 
	printf("\nRespuesta %d \n", ntohl(res));
    
    // Cerrar el socket 
   	close (sd);
    return 0;
}

int modify_value(int key, char *value1, int N_value2, double *V_value2){
    
    int send_status;
    char *port_tuplas_str = port();
    char *ip_tuplas = ip();
    int port_tuplas = atoi(port_tuplas_str);

    // Crear el socket 
    int sd;
    struct sockaddr_in server_addr;
    struct hostent *hp;
    sd = socket(AF_INET, SOCK_STREAM, 0);

    if (sd == -1) {
		printf("Error al crear el socket\n");
		return -1;
	}

    // Especificar una direccion para el socket 
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
    int connection_status = connect(sd, (struct sockaddr *) &server_addr, sizeof(server_addr));
    if (connection_status == -1) {
		perror("Error en la conexión\n");
		return -1;
	}
    
    char op = 3; // Operación de modify

    // Enviar la op
    send_status = sendMessage(sd, (char*) &op, sizeof(char));
    if (send_status == -1){
		printf("Error en el envio\n");
		return -1;
	}
   
    // Enviar la key
    int netKey = htonl(key); // Convertir key a formato de red 
    
    send_status = sendMessage(sd, (char*) &netKey, sizeof(int));
    if (send_status == -1){
		printf("Error en el envio de la clave\n");
		return -1;
	}

    // Enviar value1
    send_status = sendMessage(sd, value1, 256);
    if (send_status == -1){
		printf("Error en el envio de value1\n");
		return -1;
	}

    // Enviar N_value2
    int netN_value2 = htonl(N_value2); // Convertir key a formato de red 
    send_status = sendMessage(sd, (char*) &netN_value2, sizeof(int));
    if (send_status == -1){
		printf("Error en el envio de N_value2\n");
		return -1;
	}

    // Enviar V_value2
    for(int i = 0; i < N_value2; i++){
        send_status = sendMessage(sd, (char*) &V_value2[i], sizeof(double));
        if (send_status == -1){
            printf("Error en el envio de V_value2\n");
            return -1;
	    }
    }
    
    // Recibir respuesta del servidor 
    int32_t res; 
	fflush(stdout);
    int recv_status = recvMessage(sd, (char *) &res, sizeof(int32_t));   
    if (recv_status == -1){
		printf("Error en recepción\n");
		return -1;
	}
    
    // Imprimir la respuesta 
	printf("\nRespuesta %d \n", ntohl(res));
    
    // Cerrar el socket 
   	close (sd);

    return 0;
}

int delete_key(int key){
    int send_status;
    char *port_tuplas_str = port();
    char *ip_tuplas = ip();
    int port_tuplas = atoi(port_tuplas_str);

    // Crear el socket 
    int sd;
    struct sockaddr_in server_addr;
    struct hostent *hp;
    sd = socket(AF_INET, SOCK_STREAM, 0);

    if (sd == -1) {
		printf("Error al crear el socket\n");
		return -1;
	}

    // Especificar una direccion para el socket 
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
    int connection_status = connect(sd, (struct sockaddr *) &server_addr, sizeof(server_addr));
    if (connection_status == -1) {
		perror("Error en la conexión\n");
		return -1;
	}
    
    char op = 4; // Operación de delete

    // Enviar la op
    send_status = sendMessage(sd, (char*) &op, sizeof(char));
    if (send_status == -1){
		printf("Error en el envio\n");
		return -1;
	}
   
    // Enviar la key
    int netKey = htonl(key); // Convertir key a formato de red 
    
    send_status = sendMessage(sd, (char*) &netKey, sizeof(int));
    if (send_status == -1){
		printf("Error en el envio de la clave\n");
		return -1;
	}

    // Recibir respuesta del servidor 
    int32_t res; 
	fflush(stdout);
    int recv_status = recvMessage(sd, (char *) &res, sizeof(int32_t));   
    if (recv_status == -1){
		printf("Error en recepción\n");
		return -1;
	}
    
    // Imprimir la respuesta 
	printf("\nRespuesta %d \n", ntohl(res));
    
    // Cerrar el socket 
   	close (sd);
    
    return 0;
}

int exist(int key){
    
    int send_status;
    char *port_tuplas_str = port();
    char *ip_tuplas = ip();
    int port_tuplas = atoi(port_tuplas_str);

    // Crear el socket 
    int sd;
    struct sockaddr_in server_addr;
    struct hostent *hp;
    sd = socket(AF_INET, SOCK_STREAM, 0);

    if (sd == -1) {
		printf("Error al crear el socket\n");
		return -1;
	}

    // Especificar una direccion para el socket 
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
    int connection_status = connect(sd, (struct sockaddr *) &server_addr, sizeof(server_addr));
    if (connection_status == -1) {
		perror("Error en la conexión\n");
		return -1;
	}
    
    char op = 5; // Operación de delete

    // Enviar la op
    send_status = sendMessage(sd, (char*) &op, sizeof(char));
    if (send_status == -1){
		printf("Error en el envio\n");
		return -1;
	}
   
    // Enviar la key
    int netKey = htonl(key); // Convertir key a formato de red 
    
    send_status = sendMessage(sd, (char*) &netKey, sizeof(int));
    if (send_status == -1){
		printf("Error en el envio de la clave\n");
		return -1;
	}

    // Recibir respuesta del servidor 
    int32_t res; 
	fflush(stdout);
    int recv_status = recvMessage(sd, (char *) &res, sizeof(int32_t));   
    if (recv_status == -1){
		printf("Error en recepción\n");
		return -1;
	}
    
    // Imprimir la respuesta 
	printf("\nRespuesta %d \n", ntohl(res));
    
    // Cerrar el socket 
   	close (sd);
    return 0;
}