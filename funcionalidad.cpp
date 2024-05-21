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
           // parseSee(m, jugador);
            parseSeeRefactor(m, jugador);
            obtenerValoresPase(jugador, m);
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

void obtenerValoresPase(Jugador & jugador, const string& mensaje) {
    vector<string> palabras = dividir_en_palabras_parentesis(mensaje);
    vector<string> resultado;
    jugador.flags.compañerosCerca.clear();
    jugador.flags.enemigosCerca.clear();
    for (const string &palabra : palabras) {
        if (palabra != "see" && palabra != "0") {
            if (palabra.find("(p") != string::npos) {
                resultado = sacarValoresFlags(palabra);
                // Extraer el nombre del equipo del mensaje
                size_t inicio_comillas = palabra.find('"');
                size_t fin_comillas = palabra.find('"', inicio_comillas + 1);
                string equipo_del_mensaje = palabra.substr(inicio_comillas + 1, fin_comillas - inicio_comillas - 1);
                if (jugador.nombreEquipo == equipo_del_mensaje) {
                    // Si hay solo 2 elementos en resultado, significa que el segundo parámetro contiene ambos valores del pase
                    if (resultado.size() == 2) {
                        jugador.flags.compañerosCerca.push_back(make_pair(stof(resultado.at(0)), stof(resultado.at(1))));
                    } else if (resultado.size() > 2) {
                        // Si hay más de 2 elementos en resultado, solo tomamos los dos primeros como valores del pase
                        jugador.flags.compañerosCerca.push_back(make_pair(stof(resultado.at(0)), stof(resultado.at(1))));
                    }
                }else if (jugador.nombreEquipo != equipo_del_mensaje) {
                    jugador.flags.enemigosCerca.push_back(make_pair(stof(resultado.at(0)), stof(resultado.at(1))));
                }
            }
        }  
    }
}


vector<string> sacarValoresFlags(const string &palabra){
    vector<string> valores_separados{};
    size_t segundo_parentesis = palabra.find(')');
    // Extrae los valores que están fuera del paréntesis
    string valores_fuera_del_parentesis = palabra.substr(segundo_parentesis + 1);
    // Elimina el espacio en blanco al inicio
    size_t primer_espacio = valores_fuera_del_parentesis.find(' ');
    if (primer_espacio != string::npos) {
        valores_fuera_del_parentesis = valores_fuera_del_parentesis.substr(primer_espacio + 1);
    }
    
    size_t segundo_espacio = valores_fuera_del_parentesis.find(' ');
    if (segundo_espacio != string::npos) {
        string primer_valor = valores_fuera_del_parentesis.substr(0, segundo_espacio);
        valores_separados.push_back(primer_valor);

        // Extraer el segundo valor
        string segundo_valor = valores_fuera_del_parentesis.substr(segundo_espacio + 1);
        valores_separados.push_back(segundo_valor);
    }
    return valores_separados;
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
    else if (mensajeRecibido.find("kick_in_l")!= -1 || mensajeRecibido.find("kick_in_r") != -1){ //saque de banda
        jugador.estadoPartido.saqueBanda = true;
        if(mensajeRecibido.find("kick_in_l")!= -1 && jugador.equipo == "l")
            jugador.estadoPartido.saqueBandaYo = true;
        else if (mensajeRecibido.find("kick_in_r") != -1 && jugador.equipo == "r")
            jugador.estadoPartido.saqueBandaYo = true;
    }
    else if (mensajeRecibido.find("corner_kick_l")!= -1 || mensajeRecibido.find("corner_kick_r") != -1){ //corner
        jugador.estadoPartido.saqueBanda = true;
        if(mensajeRecibido.find("corner_kick_l")!= -1 && jugador.equipo == "l")
            jugador.estadoPartido.saqueBandaYo = true;
        else if (mensajeRecibido.find("corner_kick_r") != -1 && jugador.equipo == "r")
            jugador.estadoPartido.saqueBandaYo = true;
    }
    else if (mensajeRecibido.find("foul_charge_l")!= -1 || mensajeRecibido.find("foul_charge_r") != -1){ //falta
        jugador.estadoPartido.saqueBanda = true;
        if(mensajeRecibido.find("foul_charge_l")!= -1 && jugador.equipo == "l")
            jugador.estadoPartido.saqueBandaYo = true;
        else if (mensajeRecibido.find("foul_charge_r") != -1 && jugador.equipo == "r")
            jugador.estadoPartido.saqueBandaYo = true;
    }
    else if (mensajeRecibido.find("play_on")!= -1){
        jugador.estadoPartido.saqueBanda = false;
        jugador.estadoPartido.saqueBandaYo = false;
        jugador.estadoPartido.saquePorteria = false;
        jugador.estadoPartido.saquePorteriaYo = false;
    }
    // }else if (doubleParsedMsg.at(3) == "goal_kick"){ //penalti ? creo que saque de puerta

    // }
        
}

void comprobarKickOff(string const & mensajeRecibido, Jugador & jugador){
    auto doubleParsedMsg = dividir_en_palabras(mensajeRecibido);
    if (doubleParsedMsg.at(3).compare("kick_off_l") || doubleParsedMsg.at(3).compare("kick_off_r") ){
        jugador.estadoPartido.kickOff = true;
        jugador.estadoPartido.enJuego = true;
        cout << "Ha empezado el partido" << endl;
        if(jugador.numero == 11)
            jugador.siguienteComando = "(kick 30 180)";
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
    //colocarJugadorSegunNumero(jugador, socket, address);
    jugador.estadoPartido.colocarse = true;
    if((jugador.equipo == "r") && (jugador.numero != 1) ){ 
      //  girarEquipoVisitante(socket, address);
      
    }
}

void hearGol(string const & mensajeRecibido, Jugador & jugador){
    auto doubleParsedMsg = dividir_en_palabras(mensajeRecibido);
    if (doubleParsedMsg.at(3).find("goal_l") != -1 || doubleParsedMsg.at(3).find("goal_r") != -1){      
        if(((doubleParsedMsg.at(3).find("_l")) != -1 ) && (jugador.numero==11) && (jugador.equipo == "r"))
            jugador.estadoPartido.kickOff = true;
        if(((doubleParsedMsg.at(3).find("_r")) != -1) && (jugador.numero==11) && (jugador.equipo == "l"))
            jugador.estadoPartido.kickOff = true;
        jugador.estadoPartido.colocarse = true;
    } else if(doubleParsedMsg.at(3).find("goal_kick_") != -1){
        jugador.estadoPartido.saquePorteria = true;
        if(((doubleParsedMsg.at(3).find("_l")) != -1 ) && (jugador.numero==1) && (jugador.equipo == "l"))
            jugador.estadoPartido.saquePorteriaYo = true;
        if(((doubleParsedMsg.at(3).find("_r")) != -1) && (jugador.numero==1) && (jugador.equipo == "r"))
            jugador.estadoPartido.saquePorteriaYo = true;
    }
}

  void parseSeeRefactor(string const & mensaje, Jugador & jugador) {
    vector<string> palabras = dividir_en_palabras_parentesis(mensaje);
    vector<string> resultado;
    for (const string &palabra : palabras) {
        if (palabra != "see" && palabra != "0") {
            if(palabra.find("(g r)") != string::npos) {
                resultado = sacarValoresFlags(palabra);
                jugador.flags.distanciaPorteriaDer = stof(resultado.at(0));
                jugador.flags.orientacionPorteriaDer = stof(resultado.at(1));
            }
            else if(palabra.find("(g l)") != string::npos) {
                resultado = sacarValoresFlags(palabra);
                jugador.flags.distanciaPorteriaIzq = stof(resultado.at(0));
                jugador.flags.orientacionPorteriaIzq = stof(resultado.at(1));
            }
            else if(palabra.find("(f r t") != string::npos) {
                resultado = sacarValoresFlags(palabra);
                jugador.flags.distanciaCornerDer1 = stof(resultado.at(0));
            }
            else if(palabra.find("(f r b)") != string::npos) {
                resultado = sacarValoresFlags(palabra);
                jugador.flags.distanciaCornerDer2 = stof(resultado.at(0));
            }
            else if(palabra.find("(f l t)") != string::npos) {
                resultado = sacarValoresFlags(palabra);
                jugador.flags.distanciaCornerIzq1 = stof(resultado.at(0));
            }
            else if(palabra.find("(f l b)") != string::npos) {
                resultado = sacarValoresFlags(palabra);
                jugador.flags.distanciaCornerIzq2 = stof(resultado.at(0));
            }
            else if(palabra.find("(f t 0)") != string::npos) {
                resultado = sacarValoresFlags(palabra);
                jugador.flags.distanciaCentroCampo1 = stof(resultado.at(0));
            }
            else if(palabra.find("(f b 0)") != string::npos) {
                resultado = sacarValoresFlags(palabra);
                jugador.flags.distanciaCentroCampo2 = stof(resultado.at(0));
            }    
            else if(palabra.find("(b)") != string::npos) {
                resultado = sacarValoresFlags(palabra);
                jugador.flags.distanciaBalon = stof(resultado.at(0));
                jugador.flags.orientacionBalon = stof(resultado.at(1));
            } 
            else if(palabra.find("(f l b 20") != string::npos || palabra.find("(f l t 20") != string::npos || palabra.find("(f l b 10") != string::npos || palabra.find("(f l t 10") != string::npos 
                    || palabra.find("(f l b 30") != string::npos || palabra.find("(f l t 30") != string::npos ){
                resultado = sacarValoresFlags(palabra);
                jugador.flags.flagsFondoIzquierda.push_back(stof(resultado.at(0)));
            }
            else if(palabra.find("(f r b 20") != string::npos || palabra.find("(f r t 20") != string::npos || palabra.find("(f r b 10") != string::npos || palabra.find("(f r t 10") != string::npos 
                    || palabra.find("(f r b 30") != string::npos || palabra.find("(f r t 30") != string::npos ){
                resultado = sacarValoresFlags(palabra);
                jugador.flags.flagsFondoDerecha.push_back(stof(resultado.at(0)));
            }
            else if(palabra.find("(p l b") != string::npos) {
                resultado = sacarValoresFlags(palabra);
                jugador.flags.distanciaAreaIzqAbajo = stof(resultado.at(0));
            }  
            else if(palabra.find("(p l c") != string::npos) {
                resultado = sacarValoresFlags(palabra);
                jugador.flags.distanciaAreaIzqCentro = stof(resultado.at(0));
            }  
            else if(palabra.find("(p l t") != string::npos) {
                resultado = sacarValoresFlags(palabra);
                jugador.flags.distanciaAreaIzqArriba = stof(resultado.at(0));
            }  
            else if(palabra.find("(p r b") != string::npos) {
                resultado = sacarValoresFlags(palabra);
                jugador.flags.distanciaAreaDerAbajo = stof(resultado.at(0));
            }  
            else if(palabra.find("(p r c") != string::npos) {
                resultado = sacarValoresFlags(palabra);
                jugador.flags.distanciaAreaDerCentro = stof(resultado.at(0));
            }  
            else if(palabra.find("(p r t") != string::npos) {
                resultado = sacarValoresFlags(palabra);
                jugador.flags.distanciaAreaDerArriba = stof(resultado.at(0));
            }  
            else if(palabra.find("(f c") != string::npos) {
                resultado = sacarValoresFlags(palabra);
                jugador.flags.distanciaCentroCampo = stof(resultado.at(0));
            }  
        }    
    }
  }


float distancia(string const &  mensajeRecibido){ 
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
}


void limpiarDatosJugador(Jugador & jugador){
    jugador.flags.distanciaPorteriaDer = -9343;
    jugador.flags.orientacionPorteriaDer = -9343;
    jugador.flags.distanciaPorteriaIzq = -9343;
    jugador.flags.orientacionPorteriaIzq = -9343;
    jugador.flags.distanciaCornerIzq1 = -9343;
    jugador.flags.distanciaCornerIzq2 = -9343;
    jugador.flags.distanciaCornerDer1 = -9343;
    jugador.flags.distanciaCornerDer2 = -9343;
    jugador.flags.distanciaCentroCampo1 = -9343;
    jugador.flags.distanciaCentroCampo2 = -9343;
    jugador.infoEquipo.distMiPorteria = -9343;
    jugador.infoEquipo.distPorteriaRival = -9343;
    jugador.infoEquipo.oriPorteriaRival = -9343;
    jugador.infoEquipo.minDistCompa = -9343;
    jugador.infoEquipo.minOriCompa = -9343;
    jugador.infoEquipo.maxDistCompa = -9343;
    jugador.infoEquipo.maxOriCompa = -9343;
    jugador.infoEquipo.minDistEnem = -9343;
    jugador.infoEquipo.minOriEnem = -9343;
    jugador.flags.distanciaBalon = 3;
    jugador.flags.orientacionBalon = 30;
    jugador.siguienteComando.clear();
    jugador.flags.flagsFondoDerecha.clear();
    jugador.flags.flagsFondoIzquierda.clear();
}