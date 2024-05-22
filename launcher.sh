#!/bin/bash

# Compila el programa usando make
#make clean
make

Puerto=6500

# Verifica si make se ejecutó correctamente
if [ $? -eq 0 ]; then
    echo "La compilación fue exitosa. Ejecutando el programa..."

    gnome-terminal -- bash -c "./player NottighamMiedo G  $Puerto; exec bash" 
    ((Puerto++))
    gnome-terminal -- bash -c "./player LosCojos G $Puerto; exec bash" 
        
    for (( i=1; i<=10; i++ ))
    do
        ((Puerto++))
        gnome-terminal -- bash -c "./player NottighamMiedo j  $Puerto; exec bash" 
        ((Puerto++))
        gnome-terminal -- bash -c "./player LosCojos j $Puerto; exec bash" 
    done

else
    echo "Error: La compilación falló. No se puede ejecutar el programa."
fi