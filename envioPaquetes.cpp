#include <iostream>

#include "envioPaquetes.h"
#include "funcionalidad.h"

using namespace std;


const vector<pair<int, int>> posicionesIniciales = {
    {-51, 0}, {-30, -30}, {-35, -10}, {-35, 10}, {-30, 30}, 
    {-25, -10}, {-25, 10}, {-11, 0}, {-2, -27}, {-2, 27}, {-0.75, -10}, {-1, 0}
};


void colocarJugadorSegunNumero(Jugador jugador, MinimalSocket::udp::Udp<true> & socket, MinimalSocket::Address const & address){
    if(jugador.numero == 11 && jugador.equipo == "l")
        socket.sendTo(colocarJugador(posicionesIniciales.at(jugador.numero).first, posicionesIniciales.at(jugador.numero).second),address);
    else{
        socket.sendTo(colocarJugador(posicionesIniciales.at(jugador.numero - 1).first, posicionesIniciales.at(jugador.numero - 1).second),address);
       
    }
}

void girarEquipoVisitante(MinimalSocket::udp::Udp<true> & socket, MinimalSocket::Address const & address)
{
    socket.sendTo("(turn 180)", address);
}




void decidirComando(Jugador jugador, MinimalSocket::udp::Udp<true> & socket, MinimalSocket::Address const & address){

    const float velocidadBase = 20;
    bool PelotaenManos=0;
    switch(jugador.tipoJugador){
        case 0:
            if(PelotaenManos==1){
                socket.sendTo(golpearBalon("100", to_string(jugador.orientacionPorteria)), address);
                PelotaenManos=0;
            }
            else if(jugador.distanciaAlBalon<0.6){
                socket.sendTo("(catch " + to_string(jugador.orientacionAlBalon) + ")", address);
                PelotaenManos=1;
            }
            break;
        case 1: 
            if((jugador.orientacionAlBalon > 20))
                socket.sendTo(orientarJugador("10"), address);
            else if(jugador.orientacionAlBalon < -20)
                socket.sendTo(orientarJugador("-10"), address);
            else if(jugador.distanciaAlBalon > 30)
                 socket.sendTo("(dash " + to_string(30) + ")", address);
            else if((jugador.distanciaAlBalon < 30) && (jugador.distanciaAlBalon > 5))
                socket.sendTo("(dash 5)", address);
            else if((jugador.distanciaAlBalon < 5) && (jugador.distanciaAlBalon > 0.6))
                socket.sendTo("(dash 50)", address);
            else if(jugador.distanciaAlBalon < 0.6)
                socket.sendTo(golpearBalon("20", to_string(jugador.orientacionPorteria)), address);
            break;
        case 2:
            if((jugador.orientacionAlBalon > 20))
                socket.sendTo(orientarJugador("10"), address);
            else if(jugador.orientacionAlBalon < -20)
                socket.sendTo(orientarJugador("-10"), address);
            else if(jugador.distanciaAlBalon > 15)
                 socket.sendTo("(dash " + to_string(30) + ")", address);
            else if((jugador.distanciaAlBalon < 15) && (jugador.distanciaAlBalon > 5))
                socket.sendTo("(dash 5)", address);
                else if((jugador.distanciaAlBalon < 5) && (jugador.distanciaAlBalon > 0.6))
                socket.sendTo("(dash 50)", address);
            else if(jugador.distanciaAlBalon < 0.6)
                socket.sendTo(golpearBalon("20", to_string(jugador.orientacionPorteria)), address);
            break;
        case 3:
            if((jugador.orientacionAlBalon > 20))
                socket.sendTo(orientarJugador("10"), address);
            else if(jugador.orientacionAlBalon < -20)
                socket.sendTo(orientarJugador("-10"), address);
            else if(jugador.distanciaAlBalon < 0.6)
                socket.sendTo(golpearBalon("20", to_string(jugador.orientacionPorteria)), address);
            else{ 
                float velocidad = (jugador.distanciaAlBalon * 100) / velocidadBase ;
                if (velocidad < velocidadBase)
                    velocidad = velocidadBase;
                socket.sendTo("(dash " + to_string(velocidad) + ")", address);
            }
            break;
    }
}
