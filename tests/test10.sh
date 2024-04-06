# delete_key valor no válido

env IP_TUPLAS=localhost PORT_TUPLAS=8080 ../cliente 0
env IP_TUPLAS=localhost PORT_TUPLAS=8080 ../cliente 1 40 "este valor no debe ser eliminado" 3
env IP_TUPLAS=localhost PORT_TUPLAS=8080 ../cliente 4 56