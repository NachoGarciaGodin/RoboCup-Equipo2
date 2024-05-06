#ifndef FUNCIONALIDAD_H
#define FUNCIONALIDAD_H

#include "jugador.h"

#include <iostream>
#include <vector>
using namespace std;

vector<string> quitarParentesis(const string & );

vector<string> dividir_en_palabras(const string & );


string colocarJugador(string const &, string const &);


string golpearBalon(string const &, string const &);


void iniciarJugador(string const &, Jugador &);


string orientarJugador(string const & );


void parseSeverMessage(const string &, Jugador &);


void parseSee(string const &, Jugador &);

float orientacion(string const &);

float distancia(string const & );


#endif // FUNCIONALIDAD_H