# Error exist

env IP_TUPLAS=localhost PORT_TUPLAS=8080 ../cliente 0
env IP_TUPLAS=localhost PORT_TUPLAS=8080 ../cliente 1 50 "valor a encontrar" 8
env IP_TUPLAS=localhost PORT_TUPLAS=8080 ../cliente 5 78            # Cuando no lo encuentra, devuelve 0