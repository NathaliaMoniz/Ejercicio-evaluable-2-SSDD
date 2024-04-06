#!/bin/bash

env IP_TUPLAS=localhost PORT_TUPLAS=8080 ./cliente 0
# Bucle for que itera 100 veces
for ((i=200; i>=0; i--))
do
    env IP_TUPLAS=localhost PORT_TUPLAS=8080 ./cliente 4 $i
done