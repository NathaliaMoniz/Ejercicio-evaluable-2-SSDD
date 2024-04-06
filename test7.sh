# set_value misma llave y tamaño del vector no válido

env IP_TUPLAS=localhost PORT_TUPLAS=8080 ./cliente 0
env IP_TUPLAS=localhost PORT_TUPLAS=8080 ./cliente 1 10 "correcto" 2
env IP_TUPLAS=localhost PORT_TUPLAS=8080 ./cliente 1 10 "error misma llave" 2
env IP_TUPLAS=localhost PORT_TUPLAS=8080 ./cliente 1 15 "error N_value pequeño" 0
env IP_TUPLAS=localhost PORT_TUPLAS=8080 ./cliente 1 20 "error N_value demasiado grande" 33 1 20 "error N_value demasiado grande" 33