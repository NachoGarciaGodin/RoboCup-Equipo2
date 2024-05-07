#!/bin/bash

# Compila el programa usando make
make clean
make

# Verifica si make se ejecutó correctamente
if [ $? -eq 0 ]; then
    echo "La compilación fue exitosa. Ejecutando el programa..."

    # Itera 11 veces
        #/home/nacho/Escritorio/RoboCup-Equipo2/player NottighamMiedo goalie &
        /home/laura/Documents/GitHub/RoboCup-Equipo2/player NottighamMiedo goalie &
    
    for i in {1..10}
    do
        # Lanza el archivo /player en una nueva terminal
        sleep 1.5
        #/home/nacho/Escritorio/RoboCup-Equipo2/player  NottighamMiedo j &
        /home/laura/Documents/GitHub/RoboCup-Equipo2/player NottighamMiedo j &
    done

    sleep 1.5

    # Itera 11 veces
        #/home/nacho/Escritorio/RoboCup-Equipo2/player LosCojos goalie &
        /home/laura/Documents/GitHub/RoboCup-Equipo2/player LosCojos goalie &
        
    
    for i in {1..10}
    do
        # Lanza el archivo /player en una nueva terminal
        sleep 1.5
        #/home/nacho/Escritorio/RoboCup-Equipo2/player LosCojos j &
        /home/laura/Documents/GitHub/RoboCup-Equipo2/player LosCojos j &
    done

else
    echo "Error: La compilación falló. No se puede ejecutar el programa."
fi