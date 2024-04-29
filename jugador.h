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

    struct PorteriaContraria {
        float orientacion;
        float distancia;
    };

    struct PosicionBalon {
        float distancia;
        float orientacion;
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

};



#endif // JUGADOR_H