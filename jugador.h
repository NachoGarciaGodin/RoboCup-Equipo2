#ifndef JUGADOR_H
#define JUGADOR_H

#include <iostream>
#include <vector>

using namespace std;

struct EstadoPartido {
    bool enJuego = false;
    bool kickOff = false;
    bool colocarse = false;
    bool saqueBanda = false;
    bool saqueBandaYo = false;
    bool saquePorteria=false;
    bool saquePorteriaYo=false;
};


struct InfoEquipo {
    float minDistCompa;
    float minOriCompa;

    float maxDistCompa;
    float maxOriCompa;

    float minDistEnem;
    float minOriEnem;

    float distMiPorteria;
    float distPorteriaRival;
    float oriPorteriaRival;

};

struct Flags {
    float distanciaPorteriaDer;
    float orientacionPorteriaDer;
    float distanciaPorteriaIzq;
    float orientacionPorteriaIzq;
    float distanciaCornerIzq1; //1 es arriba
    float distanciaCornerIzq2; //2 es abajo
    float distanciaCornerDer1;
    float distanciaCornerDer2;
    float distanciaCentroCampo;
    float distanciaCentroCampo1;
    float distanciaCentroCampo2;
    float distanciaAreaIzqArriba;
    float distanciaAreaIzqAbajo;
    float distanciaAreaIzqCentro;
    float distanciaAreaDerArriba;
    float distanciaAreaDerAbajo;
    float distanciaAreaDerCentro;
    float distanciaBalon = 3;
    float orientacionBalon = 30;
    vector<pair<float, float>> compañerosCerca; //primero distancia segundo orientacion
    vector<pair<float, float>> enemigosCerca; 

    vector<float> flagsFondoIzquierda; 
    vector<float> flagsFondoDerecha;
   

};

class Jugador {
public:
    Jugador(){}

    int numero;
    string equipo;
    string nombreEquipo;
   
    EstadoPartido estadoPartido;
    Flags flags;
    InfoEquipo infoEquipo;

    string siguienteComando;
};


#endif // JUGADOR_H