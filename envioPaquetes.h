#ifndef ENVIO_PAQUETES_H
#define ENVIO_PAQUETES_H

#include "jugador.h"
#include "envioPaquetes.h"
#include "funcionalidad.h"

#include <iostream>
#include <vector>
#include <MinimalSocket/udp/UdpSocket.h>
using namespace std;


void girarEquipoVisitante(MinimalSocket::udp::Udp<true> &, MinimalSocket::Address const &);

void decidirComando(Jugador &, MinimalSocket::udp::Udp<true> &, MinimalSocket::Address const &);

void colocarJugadorSegunNumero(Jugador, MinimalSocket::udp::Udp<true> &, MinimalSocket::Address const &);

bool comprobarKickOff (const string & , string & , Jugador & , MinimalSocket::udp::Udp<true> & , MinimalSocket::Address const & );



#endif //ENVIO_PAQUETES_H