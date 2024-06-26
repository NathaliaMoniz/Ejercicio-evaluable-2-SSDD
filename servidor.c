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
pthread_mutex_t mutex_lista1, mutex_lista2;
int busy = true;
pthread_cond_t cond_mensaje;       
List my_list;
int iniciado;

int tratar_peticion(int * s){
	// Declaración de las variables que se van a utilizar
	int recv_status;
	int32_t resultado;	
    int s_local;
	char op_recibido;
	int key_recibido;
	char value1_recibido[256];
	int N_value2_recibido = 0;

	// Copia la dirección del cliente a local
    pthread_mutex_lock(&mutex_mensaje);
	s_local = (* (int *)s);
	busy = false;
	pthread_cond_signal(&cond_mensaje);
	pthread_mutex_unlock(&mutex_mensaje);

	// Recibe el operador del cliente
	recv_status = recvMessage(s_local, (char *)&op_recibido, sizeof(char));
	if (recv_status == -1) {
			perror("Error en recepcion\n");
			close(s_local);
			exit(-1);
	}
	printf("op_recibido: %d\n", op_recibido);
	fflush(stdout);

    // Ejecuta la operación del cliente y prepara una respuesta
	if (op_recibido ==0){
        resultado = iniciar(&my_list);
		iniciado = true;
		resultado = htonl(resultado);
		sendMessage(s_local, (char*)&resultado, sizeof(int32_t));
    }

	// En caso de no ser init, se comprueba si el init se ha realizado previamente
	else if (iniciado == true){
		// Recibe la clave del cliente
		recv_status = recvMessage(s_local, (char *)&key_recibido, sizeof(int));
		if (recv_status == -1) {
			perror("Error en recepcion\n");
			close(s_local);
			exit(-1);
		}
		key_recibido = ntohl(key_recibido);

		// Comprueba si hay que realizar una operación que requiera sólo de la clave
		if (op_recibido == 4 && iniciado == true){
			// Eliminar el valor asociado a la clave key_recibido
			resultado = delete(&my_list, key_recibido);
			resultado = htonl(resultado);
			sendMessage(s_local, (char*)&resultado, sizeof(int32_t));
		}
		else if (op_recibido == 5 && iniciado == true){
			// Comprobar si existe el valor asociado a la clave key_recibido
			resultado = inlist(&my_list, key_recibido);
			resultado = htonl(resultado);
			sendMessage(s_local, (char*)&resultado, sizeof(int32_t));
		}
		else if (op_recibido == 2 && iniciado == true){
			char value1_found[256];
			int N_value2_found;
			double V_value2_found[32];

			// Obtener los valores asociados a la clave key_recibido
			int result = get(my_list, key_recibido, value1_found, &N_value2_found, V_value2_found);
			printf("res: %d\n", result);

			printf("value1_found: %s\n", value1_found);

			// Envío de los valores encontrados
			sendMessage(s_local, value1_found, 256); // Envía value1
			int netN_value2_found = htonl(N_value2_found);
			printf("N_value2: %d\n", N_value2_found);
			sendMessage(s_local, (char*)&netN_value2_found, sizeof(int)); // Envía N_value2
			for (int i = 0; i < N_value2_found; i++) {
				sendMessage(s_local, (char*)&V_value2_found[i], sizeof(double)); // Envía cada elemento de V_value2
			}
			// Envío de respuesta al cliente
			sendMessage(s_local, (char*)&result, sizeof(int32_t)); // Envía el resultado de la operación
		}
		else {
			// Recibe value1 del cliente
			recv_status = recvMessage(s_local, value1_recibido, 256);
			if (recv_status == -1) {
				perror("Error en recepcion\n");
				close(s_local);
				exit(-1);
			}
			// Recibe N_value2 del cliente
			recv_status = recvMessage(s_local, (char *)&N_value2_recibido, sizeof(int));
			if (recv_status == -1) {
				perror("Error en recepcion\n");
				close(s_local);
				exit(-1);
			}
			N_value2_recibido = ntohl(N_value2_recibido);
			
			double *V_value2_recibido = malloc(N_value2_recibido * sizeof(double)); // Alojar memoria para el vector

			// Recibe V_value2 del cliente
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
				// Añadir un elemento a la lista
				sendMessage(s_local, (char*)&resultado, sizeof(int32_t));
			}
			else if (op_recibido == 3 && iniciado == true){
				resultado = modify(&my_list, key_recibido, value1_recibido, N_value2_recibido, V_value2_recibido);
				resultado = htonl(resultado);
				// Modificar un elemento de la lista
				sendMessage(s_local, (char*)&resultado, sizeof(int32_t));
			}
			else {
				// En caso de que el operador no coincida con ninguna acción posible
				resultado = -1;
			}
		}
	}
	else {
		// Si el init no se ha realizado y el operador no coincide con init, se llegará aquí
		resultado = -1;
		printf("Error: init no realizado\n");
		resultado = htonl(resultado);
		sendMessage(s_local, (char*)&resultado, sizeof(int32_t));
	}

	// Imprimir la lista al terminar
	printList(my_list);

	// Cerrar el socket y el hilo
	close(s_local);
	pthread_exit(0);
	return 0;
}

int main(int argc, char *argv[]){  
	// Declarar las variables para el socket y los hilos
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

	// Inicializar mutex y variables condicionales
    pthread_mutex_init(&mutex_mensaje, NULL);
	pthread_mutex_init(&mutex_lista1, NULL);
    pthread_mutex_init(&mutex_lista2, NULL);
	pthread_cond_init(&cond_mensaje, NULL);
	pthread_attr_init(&t_attr);
    pthread_attr_setdetachstate(&t_attr, PTHREAD_CREATE_DETACHED);

    // recibir del cliente
    while(1) {
		// Esperar connect del cliente
		sd_client = accept(sd_server, (struct sockaddr *) &client_addr, (socklen_t *)&size);
		if (sd_client == -1) {
			printf("Error en accept\n");
			return -1;
		}

		if (pthread_create(&thid, &t_attr, (void *)tratar_peticion, (void *)&sd_client)== 0) {
			// esperar a que el hijo copie el descriptor 
			pthread_mutex_lock(&mutex_mensaje);
			while (busy == true)
				pthread_cond_wait(&cond_mensaje, &mutex_mensaje);
			busy = true;
			pthread_mutex_unlock(&mutex_mensaje);
	 		}   
        }
	
	close(sd_server);
	
	// Destruir los mutex y las variables condicionales
	pthread_mutex_destroy(&mutex_mensaje);
	pthread_mutex_destroy(&mutex_lista1);
	pthread_mutex_destroy(&mutex_lista2);
	pthread_cond_destroy(&cond_mensaje);

	return 0;
}