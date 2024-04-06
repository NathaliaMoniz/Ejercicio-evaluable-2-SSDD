#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include <pthread.h>

pthread_mutex_t mutex_lista1, mutex_lista2;


int iniciar(List *l) {
	*l = NULL;
	return 0;
}	

int set(List *l, int key, char *value1, int N_value2, double *V_value2){
	
	pthread_mutex_lock(&mutex_lista1); // Para proteger la verificación de la lista
	struct Node *ptr, *temp;
	temp = *l;
	
	// El vector no tiene un tamaño permitido
	if (N_value2 > 32 || N_value2 < 1){
		printf("Error: el número de valores del vector solo puede estar entre 1 y 32\n");
		pthread_mutex_unlock(&mutex_lista1);
		return -1;
	}

	while (temp != NULL) {
        if (temp->key == key) {
            printf("Error: Key %d ya está en la lista.\n", key);
			pthread_mutex_unlock(&mutex_lista1);
            return -1; // La llave ya existe
        }
        temp = temp->next;
    }

	pthread_mutex_unlock(&mutex_lista1);  // Desbloquear el primer mutex después de modificar la lista

	pthread_mutex_lock(&mutex_lista2);  // Bloquear el segundo mutex antes de modificar la lista
	ptr = (struct Node *) malloc(sizeof(struct Node));
	if (ptr == NULL){
		pthread_mutex_unlock(&mutex_lista2);

		return -1;
	}
		

	ptr->V_value2 = (double *) malloc(N_value2 * sizeof(double));
	if (ptr->V_value2 == NULL) {
		free(ptr);
		pthread_mutex_unlock(&mutex_lista2);

		return -1;
	}
	strcpy(ptr->value1, value1);
	ptr->key = key;
	ptr->N_value2 = N_value2;
	memcpy(ptr->V_value2, V_value2, N_value2*sizeof(double));

	if (*l == NULL) {  // Lista vacía
		ptr->next = NULL;
		*l = ptr;
	}
	// Inserta el valor en la cabeza
	else {
		ptr->next = *l;
		*l = ptr;
	}

	
	pthread_mutex_unlock(&mutex_lista2);  // Desbloquear el segundo mutex después de modificar la lista
	return 0;
}	

int get(List l, int key, char *value1, int *N_value2, double *V_value2){
	
	pthread_mutex_lock(&mutex_lista1);
	List aux;
	aux = l;	
	
	while (aux!=NULL) {
		if (aux->key == key) {
			
			strcpy(value1, aux->value1);
			*N_value2 = aux->N_value2;
			memcpy(V_value2, aux->V_value2, *N_value2*sizeof(double));
			printf("Valor a obtener encontrado\n");
			pthread_mutex_unlock(&mutex_lista1);
			return 0;		// Valor encontrado
		}
		else{
			aux = aux->next;
		}
	}
	printf("Valor a obtener no encontrado\n");
	pthread_mutex_unlock(&mutex_lista1);
	return -1;  // Valor no encontrado
}	

int printList(List l){
	List aux;

	aux = l;
	printf("esta es la lista: \n");
	while(aux != NULL){
		printf("Key=%d    value1=%s	value2=%d\n", aux->key, aux->value1, aux->N_value2);
		for(int i = 0; i<aux->N_value2; i++){
			printf("V_value2[%d]: %f\n", i, aux->V_value2[i]);
		}
		aux = aux->next;
	}
	
	return 0;
}	

int modify(List *l, int key, char *value1, int N_value2, double *V_value2){
	pthread_mutex_lock(&mutex_lista1);
	List aux;
	aux = *l;
	while (aux!=NULL) {
		if (aux->key == key) {
			strcpy(aux->value1, value1);
			aux->N_value2 = N_value2;
			memcpy(aux->V_value2, V_value2, N_value2*sizeof(double));
			printf("Valor a modificar %d encontrado\n", key);
			pthread_mutex_unlock(&mutex_lista1);  // Desbloquear el mutex después de modificar la lista
			return 0;		// Valor encontrado
		}
		else{
			aux = aux->next;
		}
	}
	printf("Error: valor a modificar %d no encontrado\n", key);
	pthread_mutex_unlock(&mutex_lista1);  // Desbloquear el mutex si no se encontró el valor
	return -1;		// Valor no encontrado
}

int delete(List *l, int key){
	
	List aux, back;

	pthread_mutex_lock(&mutex_lista1);  // Bloquea el mutex antes de entrar a la sección crítica

	if (*l == NULL) {  // Lista vacía
		pthread_mutex_unlock(&mutex_lista1);  // Desbloquea el mutex antes de salir
		return -1;
	}

	// Primer elemento de la lista
	if ((*l)->key == key){
		aux = *l;
		*l = (*l)->next;
		free(aux->V_value2);
		free(aux);
		pthread_mutex_unlock(&mutex_lista1);  // Desbloquea el mutex antes de salir
		return 0;
	}
	
	aux = *l;
	back = *l;
	while (aux!=NULL) {
		if (aux->key == key) {
			back->next = aux->next;
			free (aux);
			printf("Valor a borrar %d encontrado\n", key);
			pthread_mutex_unlock(&mutex_lista1);  // Desbloquea el mutex antes de salir
			return 0;		// Valor encontrado
		}
		else {
			back = aux;
			aux = aux->next;
		}
	}
	printf("Error: valor a borrar %d no encontrado\n", key);
	pthread_mutex_unlock(&mutex_lista1);  // Desbloquea el mutex antes de salir
	return -1;		// Valor no encontrado
}	

int inlist(List *l, int key){
	pthread_mutex_lock(&mutex_lista1); // Bloquea el mutex antes de entrar a la zona crítica

    List aux = *l;
    while (aux != NULL) {
        if (aux->key == key) {
            printf("Valor %d encontrado\n", key);
            pthread_mutex_unlock(&mutex_lista1); // Desbloquea el mutex antes de salir
            return 1; // Valor encontrado
        } else {
            aux = aux->next;
        }
    }

    printf("Error: valor %d no encontrado\n", key);
    pthread_mutex_unlock(&mutex_lista1); // Desbloquea el mutex antes de salir
    return 0; // Valor no encontrado
}

int destroy(List *l){
	pthread_mutex_lock(&mutex_lista1); // Bloquea el mutex antes de entrar a la zona crítica

    List aux;
    while (*l != NULL) {
        aux = *l;
        *l = aux->next;
        free(aux->V_value2);
        free(aux);
    }

    pthread_mutex_unlock(&mutex_lista1); // Desbloquea el mutex antes de salir
    return 0;
}	

