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

int tratar_peticion(int * s){
	int recv_status;
	int32_t resultado;	
    int s_local;
	char op_recibido;
	int key_recibido;
	char value1_recibido[256];
	int N_value2_recibido = 0;


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
		resultado = htonl(resultado);
		sendMessage(s_local, (char*)&resultado, sizeof(int32_t));
    }

	// En caso de no ser init, se reciben el resto de los parámetros
	else {
		// printf("Tomar key\n");
		fflush(stdout);
		recv_status = recvMessage(s_local, (char *)&key_recibido, sizeof(int));
		if (recv_status == -1) {
			perror("Error en recepcion\n");
			close(s_local);
			exit(-1);
		}
		key_recibido = ntohl(key_recibido);
		// printf("key: %d\n", key_recibido);
		fflush(stdout);

		if (op_recibido == 4 && iniciado == true){
			resultado = delete(&my_list, key_recibido);
			resultado = htonl(resultado);
			sendMessage(s_local, (char*)&resultado, sizeof(int32_t));
		}

		else if (op_recibido == 5 && iniciado == true){
			resultado = inlist(&my_list, key_recibido);
			resultado = htonl(resultado);
			sendMessage(s_local, (char*)&resultado, sizeof(int32_t));
		}

		else {
			// printf("Tomar el resto de valores\n");
			recv_status = recvMessage(s_local, value1_recibido, 256);
			if (recv_status == -1) {
				perror("Error en recepcion\n");
				close(s_local);
				exit(-1);
			}
			// printf("value1: %s\n", value1_recibido);
			fflush(stdout);
			recv_status = recvMessage(s_local, (char *)&N_value2_recibido, sizeof(int));
			if (recv_status == -1) {
				perror("Error en recepcion\n");
				close(s_local);
				exit(-1);
			}
			N_value2_recibido = ntohl(N_value2_recibido);
			// printf("N_value2: %d\n", N_value2_recibido);
			// fflush(stdout);
			
			double *V_value2_recibido = malloc(N_value2_recibido * sizeof(double)); // Alojar memoria para el vector

			for (int i = 0; i < N_value2_recibido; i++) {
				recv_status = recvMessage(s_local, (char*)&V_value2_recibido[i], sizeof(double)); // Recibir cada elemento del vector
				if (recv_status == -1) {
					perror("Error en recepción del elemento del vector V_value2\n");
					free(V_value2_recibido); // Liberar memoria en caso de error
					return -1;
				}
			}

			if (op_recibido == 1 && iniciado == true){
				resultado = set(&my_list, key_recibido, value1_recibido, N_value2_recibido, V_value2_recibido);
				resultado = htonl(resultado);
				sendMessage(s_local, (char*)&resultado, sizeof(int32_t));
			}
			
			else if (op_recibido == 2 && iniciado == true){
				printf("4\n");
    			fflush(stdout);
				char value1_found[256];
				int N_value2_found;
				double *V_value2_found = NULL;

				// Obtener los valores asociados a la clave key_recibido
				printf("5\n");
    			fflush(stdout);
				int result = get(my_list, key_recibido, value1_found, &N_value2_found, V_value2_found);
				printf("6\n");
    			fflush(stdout);
				printf("res: %d\n", result);

				// Preparar la respuesta
				if (result == 0) {
					// Envío de respuesta exitosa al cliente
					sendMessage(s_local, (char*)&result, sizeof(int32_t)); // Envía el resultado de la operación

					// Envío de los valores encontrados
					sendMessage(s_local, value1_found, 256); // Envía value1
					int32_t netN_value2_found = htonl(N_value2_found);
					sendMessage(s_local, (char*)&netN_value2_found, sizeof(int32_t)); // Envía N_value2
					for (int i = 0; i < N_value2_found; i++) {
						double net_V_value2 = htonl(V_value2_found[i]); // Convertir cada elemento de V_value2 a formato de red
						sendMessage(s_local, (char*)&net_V_value2, sizeof(double)); // Envía cada elemento de V_value2
					}
				
				}

				else if (op_recibido == 3 && iniciado == true){
					resultado = modify(&my_list, key_recibido, value1_recibido, N_value2_recibido, V_value2_recibido);
					resultado = htonl(resultado);
					sendMessage(s_local, (char*)&resultado, sizeof(int32_t));
				}

				else {
					resultado = -1;
				}
			}
		}
	}

	// Imprimir la lista al terminar
	printList(my_list);
	
	// resultado = htonl(resultado);
	// int send_status = sendMessage(s_local, (char *)&resultado, sizeof(int32_t));  // enví­a el resultado
	// if (send_status == -1) {
	// 	perror("Error en envi­o\n");
	// 	close(s_local);
	// 	exit(-1);
	// }
	close(s_local);
	pthread_exit(0);
	return 0;
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