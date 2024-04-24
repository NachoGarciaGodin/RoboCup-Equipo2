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
    // Constructor
    Jugador(int num, string eq, float ang) : numero(num), equipo(eq), angRotacion(ang) {}

    Jugador(){}

    // Destructor
    ~Jugador() {}



    // Atributos privados
    int numero;
    string equipo;
    float angRotacion;

};



#endif // JUGADOR_H