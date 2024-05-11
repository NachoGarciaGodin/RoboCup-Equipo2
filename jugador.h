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
    float distanciaBalon = 3;
    float orientacionBalon = 30;
    vector<pair<float, float>> compañerosCerca; //primero distancia segundo orientacion
    vector<pair<float, float>> enemigosCerca; //primero distancia segundo orientacion
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
   
    float orientacionPorteria;
    float distanciaPorteria;
    EstadoPartido estadoPartido;
    Flags flags;
    bool hayPase=false;
    float orientacionPase;
    float distanciaPase;


    string siguienteComando;
};


#endif // JUGADOR_H