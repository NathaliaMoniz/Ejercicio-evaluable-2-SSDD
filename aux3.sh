#!/bin/bash

env IP_TUPLAS=localhost PORT_TUPLAS=8080 ./cliente 0
# Bucle for que itera 100 veces
for ((i=0; i<=200; i+=2))
do
    env IP_TUPLAS=localhost PORT_TUPLAS=8080 ./cliente 3 $i "valor $i modificado" 2
done