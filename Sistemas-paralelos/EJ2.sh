#!/bin/bash

#Se compila el programa 
gcc -O3 -o ej2 EJ2-TP1-Version_Fran-2025.c 

#Se setean variables
array=(512 1024 2048 4096)
bs_array=(4 8 16 32 64)
ofile="resultado.csv"

#Se crea el informe
echo "N;BS;time" > ${ofile}

#Comienzan las pruebas
for size in ${array[@]}; do 
    for bs in ${bs_array[@]}; do
    	echo "Prueba de ejecución con matrices de dimensión de ${size} y tamaño de bloque ${bs}"
    	echo "${size};${bs};$(./ej2 ${size} ${bs} | tr '.' ',')" >> ${ofile}
     done
done

echo "Finalizan las pruebas, resultados en ${ofile}"