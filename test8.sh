# get_value llave errónea

env IP_TUPLAS=localhost PORT_TUPLAS=8080 ./cliente 0
env IP_TUPLAS=localhost PORT_TUPLAS=8080 ./cliente 1 20 "correcto" 5
env IP_TUPLAS=localhost PORT_TUPLAS=8080 ./cliente 2 21                 # La clave 21 no corresponde a ninguna clave de la listao corresponde a ninguna clave de la lista