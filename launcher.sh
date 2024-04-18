#!/bin/bash

# Compila el programa usando make
make

# Verifica si make se ejecutó correctamente
if [ $? -eq 0 ]; then
    echo "La compilación fue exitosa. Ejecutando el programa..."

    # Itera 11 veces
    gnome-terminal -- bash -c "/home/nacho/Escritorio/robocup/player NottighamMiedo goalie" &
    
    for i in {1..11}
    do
        # Lanza el archivo /player en una nueva terminal
        sleep 1
        gnome-terminal -- bash -c "/home/nacho/Escritorio/robocup/player NottighamMiedo j" &
    done

    # Itera 11 veces
    gnome-terminal -- bash -c "/home/nacho/Escritorio/robocup/player LosCojos goalie" &
    
    for i in {1..11}
    do
        # Lanza el archivo /player en una nueva terminal
        sleep 1
        gnome-terminal -- bash -c "/home/nacho/Escritorio/robocup/player LosCojos j" &
    done

else
    echo "Error: La compilación falló. No se puede ejecutar el programa."
fi