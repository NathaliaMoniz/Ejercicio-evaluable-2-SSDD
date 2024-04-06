# modify_value valor no válido

env IP_TUPLAS=localhost PORT_TUPLAS=8080 ../cliente 0
env IP_TUPLAS=localhost PORT_TUPLAS=8080 ../cliente 1 30 "valor sin modificar" 1
env IP_TUPLAS=localhost PORT_TUPLAS=8080 ../cliente 3 35 "valor erroneo a modificar" 5