#include "funcionalidad.h"
#include "jugador.h"

#include <iostream>
#include <vector>
#include <algorithm>
#include <memory>
#include <functional>
#include <string>
#include <thread>

using namespace std;




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


string colocarJugador(string const & posx, string const & posy){

    string msgEnvio {"(move "};

    msgEnvio.append(posx);
    msgEnvio.append(" ");
    msgEnvio.append(posy);
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




bool comprobarKickOff (const string & mensaje, string & ladoKickOff){ // (hear 0 referee kick_off_l)) 
        auto parsedMsg = quitarParentesis(mensaje).at(0);
        auto doubleParsedMsg = dividir_en_palabras(parsedMsg);

        if (doubleParsedMsg.size() == 4) {
            if (doubleParsedMsg.at(3) == "kick_off_l" || doubleParsedMsg.at(3) == "kick_off_r"){
                ladoKickOff = doubleParsedMsg.at(3);
                return true;
            }
                
        }
}
