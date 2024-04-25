#include "funcionalidad.h"
#include "jugador.h"

#include <iostream>
#include <vector>
#include <algorithm>
#include <memory>
#include <functional>
#include <string>

#include <MinimalSocket/udp/UdpSocket.h>


using namespace std;



const vector<pair<int, int>> posicionesIniciales = {
    {-51, 0}, {-30, -30}, {-35, -10}, {-35, 10}, {-30, 30}, 
    {-25, -10}, {-25, 10}, {-11, 0}, {-0.5, -27}, {-0.5, 27}, {-0.75, -10}, {-0.4, 0.15}
};


vector<string> quitarParentesis(const string & cadena){
    vector<string> palabras;
    string palabra_actual; 
    int parentesis=0,posicion_inicial,i=0;
    for (char caracter : cadena) {
        if (caracter == '(') {
            parentesis++;
            if(parentesis==1)
                posicion_inicial=i+1;
        }else if(caracter==')'){
            parentesis--;
            if(parentesis == 0){
                palabra_actual=cadena.substr(posicion_inicial,i-posicion_inicial);
                if (!palabra_actual.empty()) {
                    palabras.push_back(palabra_actual);
                    palabra_actual.clear();
                }
            }
        }
        i++;
    }
    if (!palabra_actual.empty()) {
        palabras.push_back(palabra_actual);
    }
    return palabras;
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

float distanciaBalon(string const &  mensajeRecibido){ // revisar decimales, no los coge
    int distancia=0,pos;
    pos=mensajeRecibido.find(' ',0);
    string mensaje_aux=mensajeRecibido.substr(0,pos);
    distancia=stof(mensaje_aux);
    return distancia;
}

float orientacionBalon(string const & mensajeRecibido){
    int orientacion=0,pos;
    pos=mensajeRecibido.find(' ',0);
    auto pos2=mensajeRecibido.find(')',pos);
    if(pos2==-1)
        pos2=mensajeRecibido.find(' ',pos+1); 
    string mensaje_aux=mensajeRecibido.substr(pos+1,pos2-pos);
    orientacion=stof(mensaje_aux);
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

/*
void colocarJugadores(MinimalSocket::udp::Udp<true> & socket, MinimalSocket::Address const & address){
    for(auto pos : posicionesIniciales){
         socket.sendTo(colocarJugador(pos.first, pos.second), address);
    }
}
*/

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


void iniciarJugador(string const & mensajeInicial, Jugador & jugador){

    auto parsedMsg = quitarParentesis(mensajeInicial).at(0);
    auto doubleParsedMsg = dividir_en_palabras(parsedMsg);

    
    jugador.equipo = doubleParsedMsg.at(1);
    jugador.numero = stoi(doubleParsedMsg.at(2));


}

void decidirComando(Jugador jugador, float & distanciaAlBalon, float & orientacionAlBalon, MinimalSocket::udp::Udp<true> & socket, MinimalSocket::Address const & address){
    
    const float velocidadBase = 10;
    if (jugador.numero == 8 && jugador.equipo == "l"){
    }
    if(distanciaAlBalon <= 0.6){
        cout << jugador.numero << "ha golpeado/quiere " << endl;
        socket.sendTo(golpearBalon("100","0"), address);
    }
    else if (abs(orientacionAlBalon) > 20 )
    {
        socket.sendTo(orientarJugador(to_string(20)), address);
    }
    else {
        
        float velocidad = (distanciaAlBalon * 100) / velocidadBase ;
        if (velocidad < velocidadBase)
            velocidad = 10;
        socket.sendTo("(dash " + to_string(velocidad) + ")", address);
    }

    
}

/*
string comprobarKickOff (const string & mensaje){ // (hear 0 referee kick_off_l)) 
        auto parsedMsg = quitarParentesis(mensaje).at(0);
        cout << parsedMsg << endl;
        auto doubleParsedMsg = dividir_en_palabras(parsedMsg);

        if (doubleParsedMsg.size() == 4) {
            if (doubleParsedMsg.at(3) == "kick_off_l" || doubleParsedMsg.at(3) == "kick_off_r"))
            
}

*/