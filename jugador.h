#ifndef JUGADOR_H
#define JUGADOR_H

#include <iostream>
#include <vector>

using namespace std;

struct EstadoPartido {
    bool enJuego = false;
    bool kickOff = false;
    bool colocarse = false;
};

struct Flags {
    float distanciaPorteriaDer;
    float orientacionPorteriaDer;
    float distanciaPorteriaIzq;
    float orientacionPorteriaIzq;
    float distanciaCornerIzq1;
    float distanciaCornerIzq2;
    float distanciaCornerDer1;
    float distanciaCornerDer2;
    float distanciaCentroCampo1;
    float distanciaCentroCampo2;
    float distanciaBalon;
    float orientacionBalon;
    vector<pair<float, float>> pase; //primero distancia segundo orientacion
};

class Jugador {
public:
    Jugador(){}

    // Destructor
    ~Jugador() {}

    // Atributos privados
    int numero;
    string equipo;
    string nombreEquipo;
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