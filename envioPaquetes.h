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

void arbolDecisiones(Jugador & , MinimalSocket::udp::Udp<true> & , MinimalSocket::Address const & );

void accionesRepetidas(Jugador & , MinimalSocket::udp::Udp<true> & , MinimalSocket::Address const & );


void arbolJugador1(Jugador, MinimalSocket::udp::Udp<true> &, MinimalSocket::Address const &);
void arbolJugador2(Jugador, MinimalSocket::udp::Udp<true> &, MinimalSocket::Address const &);
void arbolJugador3(Jugador, MinimalSocket::udp::Udp<true> &, MinimalSocket::Address const &);
void arbolJugador4(Jugador, MinimalSocket::udp::Udp<true> &, MinimalSocket::Address const &);
void arbolJugador5(Jugador, MinimalSocket::udp::Udp<true> &, MinimalSocket::Address const &);
void arbolJugador6(Jugador, MinimalSocket::udp::Udp<true> &, MinimalSocket::Address const &);
void arbolJugador7(Jugador, MinimalSocket::udp::Udp<true> &, MinimalSocket::Address const &);
void arbolJugador8(Jugador, MinimalSocket::udp::Udp<true> &, MinimalSocket::Address const &);
void arbolJugador9(Jugador, MinimalSocket::udp::Udp<true> &, MinimalSocket::Address const &);
void arbolJugador10(Jugador, MinimalSocket::udp::Udp<true> &, MinimalSocket::Address const &);
void arbolJugador11(Jugador, MinimalSocket::udp::Udp<true> &, MinimalSocket::Address const &);



#endif //ENVIO_PAQUETES_H