# exist_key OK
env IP_TUPLAS=localhost PORT_TUPLAS=8080 ../cliente 0 
env IP_TUPLAS=localhost PORT_TUPLAS=8080 ../cliente 1 23 "Ana" 2
env IP_TUPLAS=localhost PORT_TUPLAS=8080 ../cliente 1 55 "Nacho" 2
env IP_TUPLAS=localhost PORT_TUPLAS=8080 ../cliente 1 39 "Alberto" 6
env IP_TUPLAS=localhost PORT_TUPLAS=8080 ../cliente 1 79 "Nathalia" 2
env IP_TUPLAS=localhost PORT_TUPLAS=8080 ../cliente 5 55                 # Este exist debe dar 1