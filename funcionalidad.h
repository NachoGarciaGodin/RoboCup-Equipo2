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

void colocarJugadorSegunNumero(Jugador, string const & ,MinimalSocket::udp::Udp<true> &,
                            MinimalSocket::Address const &);


string golpearBalon(string const &, string const &);


void iniciarJugador(string const &, Jugador &);

void girarEquipoVisitante(MinimalSocket::udp::Udp<true> &, MinimalSocket::Address const &);

void decidirComando(Jugador, MinimalSocket::udp::Udp<true> & socket, MinimalSocket::Address const & address);

string orientarJugador(string const & );

void encontrarCadena(string const &, Jugador &);

float orientacion(string const &);

float distancia(string const & );

bool comprobarKickOff (const string &, string &, Jugador &, MinimalSocket::udp::Udp<true> & socket, MinimalSocket::Address const & address);

#endif // FUNCIONALIDAD_H