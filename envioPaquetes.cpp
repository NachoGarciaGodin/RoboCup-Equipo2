#include <iostream>
#include <thread>
#include <chrono>

#include "envioPaquetes.h"
#include "funcionalidad.h"
#include "jugador.h"
#include <MinimalSocket/udp/UdpSocket.h>
using namespace std;


const vector<pair<int, int>> posicionesIniciales = {
    {-51, 0}, {-30, -30}, {-35, -10}, {-35, 10}, {-30, 30}, 
    {-25, -10}, {-25, 10}, {-11, 0}, {-2, -27}, {-2, 27}, {-1, 0}, {-2, 0}
};

void girarEquipoVisitante(MinimalSocket::udp::Udp<true> & socket, MinimalSocket::Address const & address)
{
    socket.sendTo("(turn 180)", address);
}

void colocarJugadorSegunNumero(Jugador jugador, string const & auxKickOff ,MinimalSocket::udp::Udp<true> & socket, MinimalSocket::Address const & address){
    if((jugador.numero == 11) && (jugador.equipo == auxKickOff))
        socket.sendTo(colocarJugador(to_string(posicionesIniciales.at(jugador.numero).first), to_string(posicionesIniciales.at(jugador.numero).second)), address);
    else{
        socket.sendTo(colocarJugador(to_string(posicionesIniciales.at(jugador.numero - 1).first), to_string(posicionesIniciales.at(jugador.numero - 1).second)), address);
    }
}


bool comprobarKickOff (const string & mensaje, string & ladoKickOff, Jugador & jugador, MinimalSocket::udp::Udp<true> & socket, MinimalSocket::Address const & address){ // (hear 0 referee kick_off_l))  play-mode to kick_in, corner_kick, or goal_kick.
        auto parsedMsg = quitarParentesis(mensaje).at(0);
        auto doubleParsedMsg = dividir_en_palabras(parsedMsg);
        string auxKickOff;
        if (doubleParsedMsg.size() == 4) {
            if (doubleParsedMsg.at(3) == "kick_off_l" || doubleParsedMsg.at(3) == "kick_off_r"){
                ladoKickOff = doubleParsedMsg.at(3);
                return true;
            }else if((doubleParsedMsg.at(3)== "half_time") || (doubleParsedMsg.at(3)== "time_over")){
                if (ladoKickOff == "kick_off_l"){
                    auxKickOff= "l";
                }
                else if(ladoKickOff == "kick_off_r"){
                    auxKickOff= "r";
                }
                auxKickOff= "r";
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                colocarJugadorSegunNumero(jugador, auxKickOff, socket, address);
                if((jugador.equipo == "r") && (jugador.numero != 1) ){ 
                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                    girarEquipoVisitante(socket, address);
                }
                return false;
            }
            if (doubleParsedMsg.at(3).find("goal") != -1){ 
                colocarJugadorSegunNumero(jugador, auxKickOff, socket, address);
                if((jugador.equipo == "r") && (jugador.numero != 1) ){ 
                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                    girarEquipoVisitante(socket, address);
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            }
            // else if (doubleParsedMsg.at(3) == "kick_in_l" && doubleParsedMsg.at(3) == "kick_in_r"){ //saque de banda
            //     if(ladoKickOff == "kick_in_l")
                    
            // }
            //else if (doubleParsedMsg.at(3) == "corner_kick"){ //corner

            // }else if (doubleParsedMsg.at(3) == "goal_kick"){ //penalti

            // }
        }
        return true;
}


void decidirComando(Jugador & jugador, MinimalSocket::udp::Udp<true> & socket, MinimalSocket::Address const & address){

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
