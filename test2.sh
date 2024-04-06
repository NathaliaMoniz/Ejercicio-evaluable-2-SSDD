# get_value OK
env IP_TUPLAS=localhost PORT_TUPLAS=8080 ./cliente 0 
env IP_TUPLAS=localhost PORT_TUPLAS=8080 ./cliente 1 86 "prueba 1" 2
env IP_TUPLAS=localhost PORT_TUPLAS=8080 ./cliente 1 33 "shrek" 2
env IP_TUPLAS=localhost PORT_TUPLAS=8080 ./cliente 1 5 "100 anos de soledad" 6
env IP_TUPLAS=localhost PORT_TUPLAS=8080 ./cliente 2 5