#include <mqueue.h>
#include <pthread.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include "./lista/list.h"
#include "./lista/list.c"
#include "claves.h"
#include "message.h"

#define TRAZA {printf ("Traza en %s:%d\n",__FILE__,__LINE__);fflush(stdout);}

// mutex y variables condicionales para proteger la copia del mensaje
pthread_mutex_t mutex_mensaje;
int mensaje_no_copiado = true;
pthread_cond_t cond_mensaje;
mqd_t  q_servidor;          // Cola del servidor


List my_list;
int iniciado = false;
void tratar_peticion(void *mess){
    
    struct message mensaje;	// mensaje local
	mqd_t q_cliente;		// cola del cliente
	int resultado;		    // resultado de la operación

    char q_server_name[MAX];                            // Nombre de la cola servidor  
    sprintf(q_server_name,  "/Cola-%s", getlogin());   // El combre de la cola del servidor será el nombre del usuario


	// el thread copia el mensaje a un mensaje local
	pthread_mutex_lock(&mutex_mensaje);

	mensaje = (*(struct message *) mess);

	// ya se puede despertar al servidor
	mensaje_no_copiado = false;

	pthread_cond_signal(&cond_mensaje);

	pthread_mutex_unlock(&mutex_mensaje);

    // ejecutar la petición del cliente y preparar respuesta
	if (mensaje.op ==0){
        resultado = iniciar(&my_list);
		iniciado = true;
    }
		
	else if (mensaje.op == 1 && iniciado == true){
        resultado = set(&my_list, mensaje.key, mensaje.value1, mensaje.N_value2, mensaje.V_value2);
		
	}

	else if (mensaje.op == 2 && iniciado == true){
		resultado = get(my_list, mensaje.key, mensaje.value1, &mensaje.N_value2, mensaje.V_value2);
		// printf("valores de get:\nkey:%d value1:%s N_value2:%d\n", mensaje.key, mensaje.value1, mensaje.N_value2);
        
        // for(int i = 0; i < mensaje.N_value2; i++){
		//     printf("V_value2[%d]: %f\n", i, mensaje.V_value2[i]);
	    // }
	}

	else if (mensaje.op == 3 && iniciado == true){
		resultado = modify(&my_list, mensaje.key, mensaje.value1, mensaje.N_value2, mensaje.V_value2);
	}

	else if (mensaje.op == 4 && iniciado == true){
		resultado = delete(&my_list, mensaje.key);
	}

	else if (mensaje.op == 5 && iniciado == true){
		resultado = inlist(&my_list, mensaje.key);
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

