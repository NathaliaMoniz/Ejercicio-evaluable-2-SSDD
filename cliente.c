#include <mqueue.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include "claves.h"
#include <stdlib.h>
#include <time.h>

#define MAX_KEY_LENGTH 256
#define MAX_LINE_LENGTH 100


int main(int argc, char *argv[]){
    
    // Inicializar la semilla del generador de números aleatorios
    srand(time(NULL));

    // Inicialización de los valores
    int resultado;
    int op = atoi(argv[1]);
    int key = 0;
    char *value1 = "";
    int N_value2 = 0;

    // Si es necesaria la clave, tomará el valor aquí
    if (argc > 2){
        key = atoi(argv[2]);
    }
    // Si son necesarios los valores asociados a la clave, los tomará aquí
    if (argc > 3){
        value1 = strdup(argv[3]);
	    N_value2 = atoi(argv[4]);
    }
    
	double V_value2[N_value2];
    for (int i = 0; i<N_value2;i++){
        V_value2[i] = (double)rand() / rand();
    }

    if (op == 0){
        printf("envio peticion init cliente\n");
        resultado = init();
    }
    else if (op == 1){
        printf("envio peticion set_value cliente\n");
        resultado = set_value(key, value1, N_value2, V_value2);

    }

    else if (op == 2){
        char vacio1[MAX_KEY_LENGTH];
        int N_vacio2 = 0;
	    double V_vacio2[32];
        printf("envio peticion get_value cliente\n");
        resultado = get_value(key, vacio1, &N_vacio2, V_vacio2);
    }

    else if (op == 3){
        printf("envio peticion modify_value cliente\n");
        resultado = modify_value(key, value1, N_value2, V_value2);
    }
    else if (op == 4){
        printf("envio peticion delete_key cliente\n");
        resultado = delete_key(key);
    }
    else if (op == 5){
        printf("envio peticion exist_key cliente\n");
        resultado = exist(key);
    }

    if (resultado == -1){
        printf("Se ha producido un error.\n");
        fflush(stdout);
        return -1;
    }

    return 0;

}

