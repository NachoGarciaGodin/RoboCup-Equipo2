#include "funcionalidad.h"
#include "jugador.h"

#include <iostream>
#include <vector>
#include <algorithm>
#include <memory>
#include <functional>
#include <string>
#include <thread>

#include <MinimalSocket/udp/UdpSocket.h>


using namespace std;



const vector<pair<int, int>> posicionesIniciales = {
    {-51, 0}, {-30, -30}, {-35, -10}, {-35, 10}, {-30, 30},
    {-25, -10}, {-25, 10}, {-11, 0}, {-2, -27}, {-2, 27}, {-0.75, -10}, {-1, 0}
};

vector<string> quitarParentesis(const string & cadena){
  vector<string> groups;
  string group = "";
  int count = 0;
  for (auto x : cadena)
  {
    if (x == '(')
    {
      count++;
      continue;
    }
    if (x == ')')
    {
      count--;

      if (count == 0 && group != "")
      {
        groups.push_back(group);
        group = "";
      }
      continue;
    }

    if (count > 0)
    {
      group = group + x;
    }
  }
  return groups;
}

vector<string> dividir_en_palabras(const string& cadena) {
    vector<string> palabras;
    string palabra_actual;
    for (char caracter : cadena) {
        if (caracter == ' ') {
            if (!palabra_actual.empty()) {
                palabras.push_back(palabra_actual);
                palabra_actual.clear();
            }
        } else {
            palabra_actual += caracter;
        }
    }
    if (!palabra_actual.empty()) {
        palabras.push_back(palabra_actual);
    }
    return palabras;
}

void encontrarCadena(string const & mensajeInicial, Jugador & jugador) {

    int posSee=mensajeInicial.find("see",0);
    if (posSee != -1){
        int posBall = mensajeInicial.find("(b)",posSee);
        int posFlagPorteria=-1;
        if (posBall != -1){
            string aux = mensajeInicial.substr(posBall+4,8);
            jugador.distanciaAlBalon = distancia(aux);
            jugador.orientacionAlBalon = orientacion(aux);
        }
        if(jugador.equipo=="r")
            posFlagPorteria= mensajeInicial.find("(g l)",posSee);
        else
            posFlagPorteria= mensajeInicial.find("(g r)",posSee);
        if (posFlagPorteria != -1){
            string aux2 = mensajeInicial.substr(posFlagPorteria+5,11);
            jugador.distanciaPorteria = distancia(aux2);
            jugador.orientacionPorteria = orientacion(aux2);
        }
    }
}

float distancia(string const &  mensajeRecibido){ // revisar decimales, no los coge
    int distancia=0;
    auto mensaje = mensajeRecibido;
    if(mensajeRecibido.find(")",0)!=-1)
        mensaje.erase(mensajeRecibido.find(")",0));

    vector<string> palabras = dividir_en_palabras(mensaje);
    distancia=stof(palabras.at(0));

    return distancia;
}

float orientacion(string const & mensajeRecibido){
    int orientacion=0;
    auto mensaje = mensajeRecibido;
    if(mensajeRecibido.find(")",0)!=-1)
        mensaje.erase(mensajeRecibido.find(")",0));

    vector<string> palabras = dividir_en_palabras(mensaje);
    orientacion=stof(palabras.at(1));

    return orientacion;
}


string colocarJugador(int const & posx, int const & posy){

    string msgEnvio {"(move "};

    msgEnvio.append(to_string(posx));
    msgEnvio.append(" ");
    msgEnvio.append(to_string(posy));
    msgEnvio.append(")");


    return msgEnvio;

}

string golpearBalon(string const & power, string const & direction){

    string msgEnvio {"(kick "};

    msgEnvio.append(power);
    msgEnvio.append(" ");
    msgEnvio.append(direction);
    msgEnvio.append(" )");


    return msgEnvio;

}

string orientarJugador(string const & gradosAOrientarse){

    string msgEnvio {"(turn "};

    msgEnvio.append(gradosAOrientarse);
    msgEnvio.append(" )");


    return msgEnvio;

}

void colocarJugadorSegunNumero(Jugador jugador, string const & auxKickOff ,MinimalSocket::udp::Udp<true> & socket, MinimalSocket::Address const & address){
    if((jugador.numero == 11) && (jugador.equipo == auxKickOff))
        socket.sendTo(colocarJugador(posicionesIniciales.at(jugador.numero).first, posicionesIniciales.at(jugador.numero).second),address);
    else{
        socket.sendTo(colocarJugador(posicionesIniciales.at(jugador.numero - 1).first, posicionesIniciales.at(jugador.numero - 1).second),address);

    }
}

void girarEquipoVisitante(MinimalSocket::udp::Udp<true> & socket, MinimalSocket::Address const & address)
{
    socket.sendTo("(turn 180)", address);
}


void iniciarJugador(string const & mensajeInicial, Jugador & jugador){

    auto parsedMsg = quitarParentesis(mensajeInicial).at(0);
    auto doubleParsedMsg = dividir_en_palabras(parsedMsg);


    jugador.equipo = doubleParsedMsg.at(1);
    jugador.numero = stoi(doubleParsedMsg.at(2));
    if( (jugador.numero>1) && (jugador.numero <6))
        jugador.tipoJugador = 1;
    else if((jugador.numero>5) && (jugador.numero <9))
        jugador.tipoJugador = 2;
    else if(jugador.numero>8)
        jugador.tipoJugador = 3;
    else
        jugador.tipoJugador = 0;
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
    jugador.distanciaAlBalon=50;
    jugador.orientacionAlBalon=50;
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
                // std::size_t message_max_size = 1000;
                // auto mensajeAux = socket.receive(message_max_size);
                if((jugador.equipo == "r") && (jugador.numero != 1) ){ //
                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                    girarEquipoVisitante(socket, address);
                }
                return false;
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
