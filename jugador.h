#ifndef JUGADOR_H
#define JUGADOR_H

#include <iostream>

using namespace std;

class Jugador {
public:
    struct Posicion {
        int posX;
        int posY;
    };    

    Jugador(){}

    // Destructor
    ~Jugador() {}

    // Atributos privados
    int numero;
    string equipo;
    float angRotacion;
    int tipoJugador;
    float orientacionAlBalon = 30;
    float distanciaAlBalon = 3;
    float orientacionPorteria;
    float distanciaPorteria;
    bool colocarse=false;
    bool KickOff=false;
    bool EnJuego=false;


};

#endif // JUGADOR_H