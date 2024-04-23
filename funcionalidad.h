#ifndef FUNCIONALIDAD_H
#define FUNCIONALIDAD_H

#include "jugador.h"

#include <iostream>
#include <vector>
#include <MinimalSocket/udp/UdpSocket.h>
using namespace std;

vector<string> quitarParentesis(const string & );

vector<string> dividir_en_palabras(const string & );


string colocarJugador(string const &, string const &);

void colocarJugadorSegunNumero(Jugador, MinimalSocket::udp::Udp<true> &,
                            MinimalSocket::Address const &);


string golpearBalon(string const &, string const &);


void iniciarJugador(string const &, Jugador &);

void girarEquipoVisitante(MinimalSocket::udp::Udp<true> &, MinimalSocket::Address const &);




float orientacionBalon(string const &);

float distanciaBalon(string const & );

#endif // FUNCIONALIDAD_H