#include <mqueue.h>
#include <pthread.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include "./lista/list.h"
#include "./lista/list.c"
#include "claves.h"
#include "send-recv.h"


// mutex y variables condicionales para proteger la copia del mensaje
pthread_mutex_t mutex_mensaje;
int busy = true;
pthread_cond_t cond_mensaje;
mqd_t  q_servidor;          

List my_list;
int iniciado;

void tratar_peticion(int * s){
	int32_t resultado;	
    int s_local;
    pthread_mutex_lock(&mutex_mensaje);
	s_local = (* (int *)s);
	busy = false;
	pthread_cond_signal(&cond_mensaje);
	pthread_mutex_unlock(&mutex_mensaje);

	char op_recibido;
	int32_t key_recibido;
	char value1_recibido;
	int32_t N_value2_recibido;
	double V_value2_recibido[N_value2_recibido];
	int recv_status = recvMessage(s_local, (char *)&op_recibido, sizeof(char));

    // ejecutar la petición del cliente y preparar respuesta
	if (op_recibido ==0){
        resultado = iniciar(&my_list);
		iniciado = true;
		resultado = htons(resultado);
		sendMessage(s_local, (char*)&resultado, sizeof(int32_t));
    }

	else {
		int recv_status = recvMessage(s_local, (char *)&key_recibido, sizeof(int32_t));
		int recv_status = recvMessage(s_local, (char *)&value1_recibido, sizeof(char));
	}
	
	if (op_recibido == 1 && iniciado == true){
		key_recibido = ntohs(key_recibido);
        resultado = set(&my_list, key_recibido, value1_recibido, N_value2_recibido, V_value2_recibido);
		
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
	
	// Se devuelve el resultado al cliente
	// Para ello se envía el resultado a su cola
    q_cliente = mq_open(mensaje.q_name, O_WRONLY);
	if (q_cliente == -1){
		perror("No se puede abrir la cola del cliente");
		fflush(stdout);
		mq_close(q_servidor);
		mq_unlink(q_server_name);
	}
	else {
		//printList(my_list);
		if (mq_send(q_cliente, (const char *) &resultado, sizeof(struct message), 0) <0) {
			perror("mq_send");
			fflush(stdout);
			mq_close(q_servidor);
			mq_unlink(q_server_name);
			mq_close(q_cliente);
		}
		
	}
	pthread_exit(0);
}

int main(void){  
	TRAZA
    struct message mensaje;
    
    pthread_attr_t t_attr;  // Atributos de los hilos
    struct mq_attributes attributes;    // Atributos de la cola

    attributes.mq_flags = 0;
    attributes.mq_maxmsg = 1;
    attributes.mq_msgsize = sizeof(mensaje);
    attributes.mq_curmsgs = 0;
    pthread_t thid;         // ID del hilo

    char q_server_name[MAX];                            // Nombre de la cola servidor  
    sprintf(q_server_name,  "/Cola-%s", getlogin());   // El combre de la cola del servidor será el nombre del usuario

    // abrir la cola
    q_servidor = mq_open(q_server_name, O_CREAT|O_RDONLY, 0700, &attributes);

    // tratamiento de error
    if(q_servidor == -1){
        perror("No se ha podido crear la cola del servidor");
		fflush(stdout);
        return -1;
    }
    pthread_mutex_init(&mutex_mensaje, NULL);
	pthread_cond_init(&cond_mensaje, NULL);
	pthread_attr_init(&t_attr);

    pthread_attr_setdetachstate(&t_attr, PTHREAD_CREATE_DETACHED);

    // Recibir el mensaje de la cola
    while(1) {
			
            if (mq_receive(q_servidor, (char *) &mensaje, sizeof(mensaje), 0) < 0 ){
				perror("mq_recev server");
				fflush(stdout);
				return -1;
		}
		printf("mensaje transmitido\n");

		if (pthread_create(&thid, &t_attr, (void *)tratar_peticion, (void *)&mensaje)== 0) {
			// Se espera a que el thread copie el mensaje 
			pthread_mutex_lock(&mutex_mensaje);
			while (mensaje_no_copiado)
				pthread_cond_wait(&cond_mensaje, &mutex_mensaje);
			mensaje_no_copiado = true;
			pthread_mutex_unlock(&mutex_mensaje);
	 		}   
        }
	return 0;
    

}

