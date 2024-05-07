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



vector<string> dividir_en_palabras_parentesis(string const &s) {
    vector<string> palabras;
    string palabra;
    char token1 = '(';
    int count_parentesis = 0;
    char token2 = ')';

    for (int i = 0; i < s.length(); i++) {
        if (s.at(i) == token1) {
            count_parentesis++;
            if (count_parentesis > 1)
                palabra.push_back(s.at(i));
        } else if (s.at(i) == token2) {
            count_parentesis--;
            if (count_parentesis > 0)
                palabra.push_back(s.at(i));
        } else if (s.at(i) == ' ') {
            if (count_parentesis == 0 && !palabra.empty()) {
                palabras.push_back(palabra);
                palabra.clear();
            } else {
                palabra.push_back(s.at(i));
            }
        } else {
            palabra.push_back(s.at(i));
        }
    }

    if (!palabra.empty()) {
        palabras.push_back(palabra);
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


void parseSeverMessage(const string &message, Jugador & jugador)
  {
    auto messages = dividir_en_palabras_parentesis(message);

    for (const string & m : messages) {
        if (m.substr(0, 3) == "see") {
            parseSee(m, jugador);

        } else if (m.substr(0, 10) == "sense_body") {
           // parseSenseBody(m, jugador);
        } else if (m.substr(0, 4) == "hear") {
            parseHearMessage(m, jugador);
        } else if (m.substr(0, 16) == "change_player_type" || m.substr(0, 2) == "ok") {
            // TODO
        } else if (m.substr(0, 11) == "player_type") {
            // TODO
        } else if (m.substr(0, 12) == "player_param") {
            // TODO
        } else {
            //cout << "Message not recognized:\n=====================================\n " << m << endl;
        }
    }
    return ;
  }

void parseHearMessage(string const & mensajeRecibido, Jugador & jugador) {

    if(mensajeRecibido.find("kick_off") != -1){
        comprobarKickOff(mensajeRecibido, jugador);
    }
    else if(mensajeRecibido.find("half_time") != -1 || mensajeRecibido.find("time_over") != -1)
    {
        mediaParte(mensajeRecibido, jugador);
    }
    if (mensajeRecibido.find("goal_") != -1){ 
        hearGol(mensajeRecibido, jugador);
    }
    // else if (doubleParsedMsg.at(3) == "kick_in_l" && doubleParsedMsg.at(3) == "kick_in_r"){ //saque de banda
    //     if(ladoKickOff == "kick_in_l")
            
    // }
    //else if (doubleParsedMsg.at(3) == "corner_kick"){ //corner

    // }else if (doubleParsedMsg.at(3) == "goal_kick"){ //penalti

    // }
        
}

void comprobarKickOff(string const & mensajeRecibido, Jugador & jugador){
    auto doubleParsedMsg = dividir_en_palabras(mensajeRecibido);
    if (doubleParsedMsg.at(3).compare("kick_of_l") || doubleParsedMsg.at(3).compare("kick_off_r") ){
        jugador.estadoPartido.kickOff = true;
        jugador.estadoPartido.enJuego = true;
    }

}

void mediaParte(string const & mensajeRecibido, Jugador & jugador){

    auto doubleParsedMsg = dividir_en_palabras(mensajeRecibido);
    string  auxKickOff;
    if (doubleParsedMsg.at(3).compare("kick_off_l")){
        auxKickOff= "l";
        if((jugador.numero==11) && (jugador.equipo == "l"))
            jugador.estadoPartido.kickOff = true;
    }
    else if(doubleParsedMsg.at(3).compare("kick_off_r")){
        auxKickOff= "r";
        if((jugador.numero==11) && (jugador.equipo == "r"))
            jugador.estadoPartido.kickOff = true;
    }
    auxKickOff= "r";
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    //colocarJugadorSegunNumero(jugador, socket, address);
    if((jugador.equipo == "r") && (jugador.numero != 1) ){ 
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
      //  girarEquipoVisitante(socket, address);
    }
}

void hearGol(string const & mensajeRecibido, Jugador & jugador){
    auto doubleParsedMsg = dividir_en_palabras(mensajeRecibido);
    if (doubleParsedMsg.at(3).find("goal_") != -1){ 
        if(((doubleParsedMsg.at(3).find("_l")) != -1 ) && (jugador.numero==11) && (jugador.equipo == "r"))
            jugador.estadoPartido.kickOff = true;
        if(((doubleParsedMsg.at(3).find("_r")) != -1) && (jugador.numero==11) && (jugador.equipo == "l"))
            jugador.estadoPartido.kickOff = true;
        jugador.colocarse=true;
        jugador.estadoPartido.enJuego = false;
    }
}

  

void parseSee(string const & mensajeInicial, Jugador & jugador) {
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

