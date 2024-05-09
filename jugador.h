#ifndef JUGADOR_H
#define JUGADOR_H

#include <iostream>

using namespace std;

struct EstadoPartido {
    bool enJuego = false;
    bool kickOff = false;
    bool colocarse = false;
};

class Jugador {
public:
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
    EstadoPartido estadoPartido;
    bool hayPase=false;
    float orientacionPase;
    float distanciaPase;


};

#endif // JUGADOR_H