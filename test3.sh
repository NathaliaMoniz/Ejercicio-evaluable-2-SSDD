# modify_value OK
env IP_TUPLAS=localhost PORT_TUPLAS=8080 ./cliente 0 
env IP_TUPLAS=localhost PORT_TUPLAS=8080 ./cliente 1 500 "cosas" 2
env IP_TUPLAS=localhost PORT_TUPLAS=8080 ./cliente 1 57 "la vida de bryan" 2
env IP_TUPLAS=localhost PORT_TUPLAS=8080 ./cliente 1 24 "101 dalmatas" 6
env IP_TUPLAS=localhost PORT_TUPLAS=8080 ./cliente 3 24 "102 dalmatas" 2LAS=8080 ../cliente 3 24 "102 dalmatas" 2