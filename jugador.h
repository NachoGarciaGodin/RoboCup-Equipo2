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
    // Constructor
    Jugador(int num, string eq, float distanciaBalon, float orientacionBalon, float ang) : numero(num), equipo(eq),  distanciaBalon(distanciaBalon), orientacionBalon(orientacionBalon), angRotacion(ang) {}

    Jugador(){}

    // Destructor
    ~Jugador() {}


    // Atributos privados
    int numero;
    string equipo;
    float angRotacion;
    float orientacionBalon = 30;
    float distanciaBalon = 3;
    float orientacionPorteria;
    float distanciaPorteria;

};



#endif // JUGADOR_H