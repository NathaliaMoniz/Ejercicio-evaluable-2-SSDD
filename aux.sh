#!/bin/bash

env IP_TUPLAS=localhost PORT_TUPLAS=8080 ./cliente 0
# Bucle for que itera 100 veces
for ((i=0; i<=100; i++))
do
    env IP_TUPLAS=localhost PORT_TUPLAS=8080 ./cliente 1 $i "valor $i" 1
done