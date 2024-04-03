#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <stdarg.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "./lista/list.h"
#include "./lista/list.c"
#include "claves.h"
#include "send-recv.h"


// mutex y variables condicionales para proteger la copia del mensaje
pthread_mutex_t mutex_mensaje;
int busy = true;
pthread_cond_t cond_mensaje;       
List my_list;
int iniciado;

void tratar_peticion(int * s){
	int recv_status;
	int32_t resultado;	
    int s_local;
	char op_recibido;
	int key_recibido;
	char *value1_recibido;
	int N_value2_recibido = 0;
	double V_value2_recibido[N_value2_recibido];

    pthread_mutex_lock(&mutex_mensaje);
	s_local = (* (int *)s);
	busy = false;
	pthread_cond_signal(&cond_mensaje);
	pthread_mutex_unlock(&mutex_mensaje);

	recv_status = recvMessage(s_local, (char *)&op_recibido, sizeof(char));
	if (recv_status == -1) {
			perror("Error en recepcion\n");
			close(s_local);
			exit(-1);
	}
	printf("op_recibido: %d\n", op_recibido);
	fflush(stdout);

    // ejecutar la petición del cliente y preparar respuesta
	if (op_recibido ==0){
        resultado = iniciar(&my_list);
		iniciado = true;
		resultado = htons(resultado);
		sendMessage(s_local, (char*)&resultado, sizeof(int32_t));
    }

	// En caso de no ser init, se reciben el resto de los parámetros
	else {
		printf("Tomar el resto de valores\n");
		fflush(stdout);
		recv_status = recvMessage(s_local, (char *)&key_recibido, sizeof(int));
		if (recv_status == -1) {
			perror("Error en recepcion\n");
			close(s_local);
			exit(-1);
		}
		key_recibido = ntohl(key_recibido);
		printf("key: %d\n", key_recibido);
		fflush(stdout);

		recv_status = recvMessage(s_local, value1_recibido, sizeof(char));
		if (recv_status == -1) {
			perror("Error en recepcion\n");
			close(s_local);
			exit(-1);
		}
		printf("value1: %s\n", value1_recibido);
		fflush(stdout);
		recv_status = recvMessage(s_local, (char *)&N_value2_recibido, sizeof(int32_t));
		if (recv_status == -1) {
			perror("Error en recepcion\n");
			close(s_local);
			exit(-1);
		}
		printf("N_value2: %d\n", N_value2_recibido);
		fflush(stdout);
		
		key_recibido = ntohs(key_recibido);
		N_value2_recibido = ntohs(N_value2_recibido);
	}
	
	if (op_recibido == 1 && iniciado == true){
		
        resultado = set(&my_list, key_recibido, value1_recibido, N_value2_recibido, V_value2_recibido);
		sendMessage(s_local, (char*)&resultado, sizeof(int32_t));
	}
	else if (op_recibido == 2 && iniciado == true){
		resultado = get(my_list, key_recibido, value1_recibido, &N_value2_recibido, V_value2_recibido);
	}

	else if (op_recibido == 3 && iniciado == true){
		resultado = modify(&my_list, key_recibido, value1_recibido, N_value2_recibido, V_value2_recibido);
	}

	else if (op_recibido == 4 && iniciado == true){
		resultado = delete(&my_list, key_recibido);
	}

	else if (op_recibido == 5 && iniciado == true){
		resultado = inlist(&my_list, key_recibido);
	}
	else {
		resultado = -1;
	}
	
	resultado = htonl(resultado);
	int send_status = sendMessage(s_local, (char *)&resultado, sizeof(int32_t));  // enví­a el resultado
	if (send_status == -1) {
		perror("Error en envi­o\n");
		close(s_local);
		exit(-1);
	}
	close(s_local);
	pthread_exit(0);
}

int main(int argc, char *argv[]){  
	int sd_server, sd_client ;
	struct sockaddr_in server_addr,  client_addr;
	int opt = 1;
	socklen_t size;
	size = sizeof(client_addr);
    pthread_attr_t t_attr;  // Atributos de los hilos
    pthread_t thid;         // ID del hilo


    // abrir socket del server
    if ((sd_server = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        printf("SERVER: Error en el socket");
        return -1;
    }

	// socket options...
	if (setsockopt(sd_server, SOL_SOCKET, SO_REUSEADDR | SO_REUSEADDR, &opt, sizeof(opt)))
	{
		printf("SERVER: Error en las opciones del socket");
        return -1;
	}
	
	int puerto = atoi(argv[1]);
	int32_t netPuerto = (int32_t)puerto;
	bzero((char *)&server_addr, sizeof(server_addr));
	server_addr.sin_family      = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port        = htons(netPuerto);

	// bind + listen
	int err = bind(sd_server, (const struct sockaddr *)&server_addr, sizeof(server_addr));
	if (err == -1) {
		printf("Error en bind\n");
		return -1;
	}

    err = listen(sd_server, SOMAXCONN);
	if (err == -1) {
		printf("Error en listen\n");
		return -1;
	}

    pthread_mutex_init(&mutex_mensaje, NULL);
	pthread_cond_init(&cond_mensaje, NULL);
	pthread_attr_init(&t_attr);
    pthread_attr_setdetachstate(&t_attr, PTHREAD_CREATE_DETACHED);

    // recibir del cliente
    while(1) {
		printf("esperando conexion\n");
		sd_client = accept(sd_server, (struct sockaddr *) &client_addr, (socklen_t *)&size);
		if (sd_client == -1) {
			printf("Error en accept\n");
			return -1;
		}

		if (pthread_create(&thid, &t_attr, (void *)tratar_peticion, (void *)&sd_client)== 0) {
			/* esperar a que el hijo copie el descriptor */ 
			pthread_mutex_lock(&mutex_mensaje);
			while (busy == true)
				pthread_cond_wait(&cond_mensaje, &mutex_mensaje);
			busy = true;
			pthread_mutex_unlock(&mutex_mensaje);
	 		}   
        }
	
	close(sd_server);
	return 0;

}

