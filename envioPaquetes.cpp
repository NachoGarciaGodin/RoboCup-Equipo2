#include <iostream>
#include <thread>
#include <chrono>
#include <algorithm>

#include "envioPaquetes.h"
#include "funcionalidad.h"
#include "jugador.h"
#include <MinimalSocket/udp/UdpSocket.h>
using namespace std;


const vector<pair<int, int>> posicionesIniciales = {
    {-51, 0}, {-25, -13}, {-25, -4}, {-25, 4}, {-25, 13}, {-18, -5}, {-18, 5}, {-11, 0}, {-8, -19}, {-0.5, 19}, {-1, 0}, {-2, 0}
};

void girarEquipoVisitante(MinimalSocket::udp::Udp<true> & socket, MinimalSocket::Address const & address)
{
    socket.sendTo("(turn 180)", address);
}

void colocarJugadorSegunNumero(Jugador jugador, MinimalSocket::udp::Udp<true> & socket, MinimalSocket::Address const & address ){
    if((jugador.numero == 11) && (jugador.estadoPartido.kickOff))
        socket.sendTo(colocarJugador(to_string(posicionesIniciales.at(jugador.numero).first), to_string(posicionesIniciales.at(jugador.numero).second)), address);
    else{
        socket.sendTo(colocarJugador(to_string(posicionesIniciales.at(jugador.numero - 1).first), to_string(posicionesIniciales.at(jugador.numero - 1).second)), address);
    }
}

void arbolDecisiones(Jugador & jugador, MinimalSocket::udp::Udp<true> & socket, MinimalSocket::Address const & address){
    
    informacionVision(jugador);

    if(jugador.estadoPartido.colocarse == true){
        jugador.estadoPartido.enJuego = false;
        colocarJugadorSegunNumero(jugador, socket, address);
        jugador.estadoPartido.colocarse = false;
    // }else if (!jugador.siguienteComando.empty()){
    //     //Este case solo ocurre cuando queremos obligar a un jugador a hacer algo por ejemplo despues de kickoff el pase es hacia detras si o si
    //     cout << "Siguiente comando: " << jugador.siguienteComando << endl;
    //     socket.sendTo(jugador.siguienteComando, address);
    //     jugador.siguienteComando.clear();
    }else{
        switch (jugador.numero)
        {
        case 1:
            arbolJugador1(jugador, socket, address);
            break;
        case 2:
            arbolJugador2(jugador, socket, address);
            break;
        case 3:
            arbolJugador2(jugador, socket, address);
            break;
        case 4:
            arbolJugador2(jugador, socket, address);
            break;
        case 5:
            arbolJugador2(jugador, socket, address);
            break;
        case 6:
            arbolJugador6(jugador, socket, address);
            break;  
        case 7:
            arbolJugador7(jugador, socket, address);
            break;
        case 8:
            arbolJugador8(jugador, socket, address);
            break;
        case 9:
            arbolJugador10(jugador, socket, address);
            break;
        case 10:
            arbolJugador10(jugador, socket, address);
            break;
        case 11:
            if(jugador.estadoPartido.enJuego && jugador.estadoPartido.kickOff && jugador.flags.distanciaBalon <= 1 && jugador.equipo=="l"){
                socket.sendTo(golpearBalon("45", "92"), address);
                jugador.estadoPartido.kickOff=false;
            }else
                arbolJugador10(jugador, socket, address);
            break;
        default:
            break;
        }

    }
}

void arbolJugador1(Jugador jugador, MinimalSocket::udp::Udp<true> & socket, MinimalSocket::Address const & address){
    static bool pelotaEnManos = false;
    
    /*if(jugador.estadoPartido.saquePorteria){
        socket.sendTo(golpearBalon("60", "0"), address);
        jugador.estadoPartido.saquePorteria = false;
    }*/
    if(pelotaEnManos){
        socket.sendTo(golpearBalon("80", "0"), address);
        pelotaEnManos=0;
    }else if(jugador.flags.distanciaBalon<2){
        if(jugador.flags.orientacionBalon < 0)
            socket.sendTo("(catch -45)", address);
        else
            socket.sendTo("(catch 45)", address);
        pelotaEnManos=1;
    }else if(jugador.flags.distanciaBalon <= 10 && jugador.flags.distanciaBalon != -9343 && jugador.infoEquipo.distPorteriaRival > 90){
        socket.sendTo(("(dash 100 " + to_string(jugador.flags.orientacionBalon) + ")"), address);
    }else if(jugador.flags.distanciaBalon > 10 && jugador.infoEquipo.distPorteriaRival >= 90 && jugador.infoEquipo.distPorteriaRival < 98.5){
        socket.sendTo(("(dash 20 180)"), address);
    }else if((jugador.flags.orientacionBalon > 15)){
        socket.sendTo(orientarJugador(to_string(jugador.flags.orientacionBalon)), address);
    }else if(jugador.flags.orientacionBalon < -15){
        socket.sendTo(orientarJugador(to_string(jugador.flags.orientacionBalon)), address);
    }
    
}

void arbolJugador2(Jugador jugador, MinimalSocket::udp::Udp<true> & socket, MinimalSocket::Address const & address){
    
    float fondoMasCercano = -1;
    float fondoMasLejano = -1;
    float bandaSuperiorMasCercana = -1;
    float bandaInferiorMasCercana = -1;

    if(jugador.equipo == "l"){
        
        if(jugador.flags.flagsBandaSuperiorIzq.size() > 0){
            float auxDistBandaSuperiorMin = jugador.flags.flagsBandaSuperiorIzq[0];
             for(auto flagFondo : jugador.flags.flagsBandaSuperiorIzq){
                auxDistBandaSuperiorMin = min(auxDistBandaSuperiorMin, flagFondo);
            }

            bandaSuperiorMasCercana = auxDistBandaSuperiorMin;
        }

        if(jugador.flags.flagsBandaInferiorIzq.size() > 0){
            float auxDistBandaInferiorMin = jugador.flags.flagsBandaInferiorIzq[0];
             for(auto flagFondo : jugador.flags.flagsBandaInferiorIzq){
                auxDistBandaInferiorMin = min(auxDistBandaInferiorMin, flagFondo);
            }

            bandaInferiorMasCercana = auxDistBandaInferiorMin;
        }

        if(jugador.flags.flagsFondoDerecha.size() > 0){
            float auxDistFondoMin = jugador.flags.flagsFondoDerecha[0];
            float auxDistFondoMax = jugador.flags.flagsFondoDerecha[0];

            for(auto flagFondo : jugador.flags.flagsFondoDerecha){
                auxDistFondoMin = min(auxDistFondoMin, flagFondo);
                auxDistFondoMax = max(auxDistFondoMax, flagFondo);
            }
            fondoMasCercano = auxDistFondoMin;
            fondoMasLejano = auxDistFondoMax;
        }
    }else{

        if(jugador.flags.flagsBandaSuperiorDer.size() > 0){
            float auxDistBandaSuperiorMin = jugador.flags.flagsBandaSuperiorDer[0];
             for(auto flagFondo : jugador.flags.flagsBandaSuperiorDer){
                auxDistBandaSuperiorMin = min(auxDistBandaSuperiorMin, flagFondo);
            }
            bandaSuperiorMasCercana = auxDistBandaSuperiorMin;
        }

        if(jugador.flags.flagsBandaInferiorDer.size() > 0){
            float auxDistBandaInferiorMin = jugador.flags.flagsBandaInferiorDer[0];
             for(auto flagFondo : jugador.flags.flagsBandaInferiorDer){
                auxDistBandaInferiorMin = min(auxDistBandaInferiorMin, flagFondo);
            }
            bandaInferiorMasCercana = auxDistBandaInferiorMin;
        }

        if(jugador.flags.flagsFondoIzquierda.size() > 0){
            float auxDistFondoMin = jugador.flags.flagsFondoIzquierda[0];
            float auxDistFondoMax = jugador.flags.flagsFondoIzquierda[0];

            for(auto flagFondo : jugador.flags.flagsFondoIzquierda){
                auxDistFondoMin = min(auxDistFondoMin, flagFondo);
                auxDistFondoMax = max(auxDistFondoMax, flagFondo);
            }

            fondoMasCercano = auxDistFondoMin;
            fondoMasLejano = auxDistFondoMax;
        }
    }

    if(jugador.estadoPartido.saqueBanda && jugador.estadoPartido.saqueBandaYo){
            if((jugador.infoEquipo.minDistCompa < jugador.flags.distanciaBalon) && (jugador.infoEquipo.minDistCompa != -9343))
                socket.sendTo(("(dash 0)"),address);
            else{ 
                if(jugador.flags.distanciaBalon <= 1){
                socket.sendTo(golpearBalon("80", "180"), address);    
                }
                else if (jugador.flags.distanciaBalon > 1 && jugador.flags.distanciaBalon <= 1.5){
                    socket.sendTo(("(dash 50)"),address);
                }
                else{
                    socket.sendTo(("(dash 100)"),address);
                }
            }
    }
    else if (jugador.estadoPartido.saqueBanda && !jugador.estadoPartido.saqueBandaYo)
        socket.sendTo(("(dash 0)"),address);
    else if(jugador.estadoPartido.saquePorteria && jugador.estadoPartido.saquePorteriaYo){
        if((jugador.infoEquipo.minDistCompa < jugador.flags.distanciaBalon) && (jugador.infoEquipo.minDistCompa != -9343))
                socket.sendTo(("(dash 0)"),address);
            else{ 
                if(jugador.flags.distanciaBalon <= 1){
                socket.sendTo(golpearBalon("30", "180"), address);    
                }
                else if (jugador.flags.distanciaBalon > 1 && jugador.flags.distanciaBalon <= 1.5){
                    socket.sendTo(("(dash 50)"),address);
                }
                else{
                    socket.sendTo(("(dash 100)"),address);
                }
            }
    }
    else if(jugador.estadoPartido.saquePorteria && !jugador.estadoPartido.saquePorteriaYo) 
        socket.sendTo(("(dash 0)"),address);

    //Si tengo el balon en los pies y estoy mirando hacia el campo contrario despejo hacia su porteria
    else if(jugador.flags.distanciaBalon <= 1 && jugador.infoEquipo.distPorteriaRival != -9343){
        socket.sendTo(golpearBalon("100", to_string(jugador.infoEquipo.oriPorteriaRival)), address);  
    }else if (jugador.flags.distanciaBalon <= 1 && ((bandaSuperiorMasCercana != -1 && jugador.equipo == "r") || (bandaInferiorMasCercana != -1 && jugador.equipo == "l"))){
        socket.sendTo(golpearBalon("100", "-90"), address);
    }else if (jugador.flags.distanciaBalon <= 1 && ((bandaInferiorMasCercana != -1 && jugador.equipo == "r") || (bandaSuperiorMasCercana != -1 && jugador.equipo == "l"))){
        socket.sendTo(golpearBalon("100", "90"), address);
    }else if(jugador.flags.distanciaBalon <= 1 && jugador.infoEquipo.distPorteriaRival == -9343 && jugador.flags.distanciaCentroCampo2 != -9343){
        socket.sendTo(golpearBalon("100", "-20"), address);
    }else if(jugador.flags.distanciaBalon <= 1 && jugador.infoEquipo.distMiPorteria != -9343){
        socket.sendTo(golpearBalon("100", "180"), address);
    }
    else if(jugador.flags.distanciaBalon <= 1 && jugador.infoEquipo.minDistEnem <= 1 && jugador.infoEquipo.minDistEnem != -9343){
        socket.sendTo(("(tackle " + to_string(jugador.flags.orientacionBalon) + ")"), address);
    }
    else if((jugador.flags.orientacionBalon > 30 ))
        socket.sendTo(orientarJugador(to_string(jugador.flags.orientacionBalon)), address);
    else if(jugador.flags.orientacionBalon < -30)
        socket.sendTo(orientarJugador(to_string(jugador.flags.orientacionBalon)), address);
    //Si tengo el balon cerca por detras de mi corro hacia el balon 
    else if (jugador.infoEquipo.distMiPorteria != -9343 && jugador.flags.distanciaBalon > 1){
        socket.sendTo("(dash 100 "+ to_string(jugador.flags.orientacionBalon) + ")", address);
    }//Si estoy lejos del balon y demasiafo arriba en el campo corro hacia mi posicion
    else if(jugador.flags.distanciaBalon > 20 && fondoMasCercano < 80 && fondoMasCercano != -1){
            socket.sendTo("(dash 50 180)", address);
    }// si trngo el balon cerca corro hacia el al menos que este cerca del medio del campo
    else if(jugador.flags.distanciaBalon > 1 &&  jugador.flags.distanciaBalon < 30 && (fondoMasCercano >= 70 || fondoMasCercano == -1)){
        socket.sendTo("(dash 100 "+ to_string(jugador.flags.orientacionBalon) + ")", address);
    }
    
}

void arbolJugador3(Jugador jugador, MinimalSocket::udp::Udp<true> & socket, MinimalSocket::Address const & address){

    if(jugador.estadoPartido.saquePorteria){
        if(jugador.flags.distanciaBalon > 1)
            socket.sendTo("(dash 50)", address);
        else if (jugador.flags.distanciaBalon <= 1)
            socket.sendTo(golpearBalon("100", "0"), address);
    }

    //TODO

}

void arbolJugador4(Jugador jugador, MinimalSocket::udp::Udp<true> & socket, MinimalSocket::Address const & address){
    //TODO

}
void arbolJugador5(Jugador jugador, MinimalSocket::udp::Udp<true> & socket, MinimalSocket::Address const & address){

   //TODO

}

void arbolJugador6(Jugador jugador, MinimalSocket::udp::Udp<true> & socket, MinimalSocket::Address const & address){

    float fondoMasCercano = -1;
    float fondoMasLejano = -1;

    if(jugador.equipo == "l"){
        
        if(jugador.flags.flagsFondoDerecha.size() > 0){
            float auxDistFondoMin = jugador.flags.flagsFondoDerecha[0];
            float auxDistFondoMax = jugador.flags.flagsFondoDerecha[0];

            for(auto flagFondo : jugador.flags.flagsFondoDerecha){
                auxDistFondoMin = min(auxDistFondoMin, flagFondo);
                auxDistFondoMax = max(auxDistFondoMax, flagFondo);
            }
            fondoMasCercano = auxDistFondoMin;
            fondoMasLejano = auxDistFondoMax;
        }
    }else{

        if(jugador.flags.flagsFondoIzquierda.size() > 0){
            float auxDistFondoMin = jugador.flags.flagsFondoIzquierda[0];
            float auxDistFondoMax = jugador.flags.flagsFondoIzquierda[0];

            for(auto flagFondo : jugador.flags.flagsFondoIzquierda){
                auxDistFondoMin = min(auxDistFondoMin, flagFondo);
                auxDistFondoMax = max(auxDistFondoMax, flagFondo);
            }

            fondoMasCercano = auxDistFondoMin;
            fondoMasLejano = auxDistFondoMax;
        }
    }

    
    if(jugador.estadoPartido.saqueBanda && jugador.estadoPartido.saqueBandaYo){
            if((jugador.infoEquipo.minDistCompa < jugador.flags.distanciaBalon) && (jugador.infoEquipo.minDistCompa != -9343))
                socket.sendTo(("(dash 0)"),address);
            else{ 
                if(jugador.flags.distanciaBalon <= 1){
                socket.sendTo(golpearBalon("30", "180"), address);    
                }
                else if (jugador.flags.distanciaBalon > 1 && jugador.flags.distanciaBalon <= 1.5){
                    socket.sendTo(("(dash 50)"),address);
                }
                else{
                    socket.sendTo(("(dash 100)"),address);
                }
            }
    }
    else if (jugador.estadoPartido.saqueBanda && !jugador.estadoPartido.saqueBandaYo)
        socket.sendTo(("(dash 0)"),address);
    else if(jugador.estadoPartido.saquePorteria)
        socket.sendTo(("(dash 0)"),address);
    else if(jugador.flags.distanciaBalon <= 1 ){
        
        //Si veo la porteria contraria y hay algun compaÃ±ero mas cerca de ella se la paso
        if(jugador.infoEquipo.distPorteriaRival > 25 && jugador.infoEquipo.maxDistCompa > 15){
            socket.sendTo(golpearBalon(to_string(jugador.infoEquipo.maxDistCompa * 2.5), to_string(jugador.infoEquipo.maxOriCompa )), address);
        } // si estamos cerca del balon y hay un enemigo cerca hacemos tackle
        else if (jugador.infoEquipo.minDistEnem <= 1 && jugador.infoEquipo.minDistEnem != -9343){
            socket.sendTo(("(tackle " + to_string(jugador.flags.orientacionBalon ) + ")"), address);
        }//si no se que hacer y veo a alguien se la paso
        else if(jugador.infoEquipo.minDistCompa != -9343 && jugador.infoEquipo.minDistCompa > 5){
            socket.sendTo(golpearBalon(to_string(jugador.infoEquipo.minDistCompa * 2.5), to_string(jugador.infoEquipo.minOriCompa )), address);
        }// Y si estoy mas perdido que un pulpo en un garaje y no veo mi porteria la despejo
        else if(jugador.infoEquipo.distMiPorteria != -9343){
            socket.sendTo(golpearBalon("50", "180"), address);
        }
        else if(jugador.infoEquipo.distPorteriaRival < 40 )
            socket.sendTo(golpearBalon("100", to_string(jugador.infoEquipo.oriPorteriaRival)), address);
        //Y 
        else{
            socket.sendTo(golpearBalon("100", "0"), address);
        
        }
    }
    else if((jugador.flags.orientacionBalon > 30 ))
        socket.sendTo(orientarJugador(to_string(jugador.flags.orientacionBalon)), address);
    else if(jugador.flags.orientacionBalon < -30)
        socket.sendTo(orientarJugador(to_string(jugador.flags.orientacionBalon)), address);
    else if (jugador.infoEquipo.distMiPorteria != -9343 && jugador.flags.distanciaBalon > 1){
        socket.sendTo("(dash 100 "+ to_string(jugador.flags.orientacionBalon) + ")", address);
    }
    else if(jugador.flags.distanciaBalon > 20 && fondoMasCercano < 75 && fondoMasCercano != -1){
        socket.sendTo("(dash 50 180)", address);
    }// si trngo el balon cerca corro hacia el al menos que este cerca del medio del campo
    else if(jugador.flags.distanciaBalon > 1 &&  jugador.flags.distanciaBalon < 35 && (fondoMasCercano >= 60 || fondoMasCercano == -1)){
        socket.sendTo("(dash 100 "+ to_string(jugador.flags.orientacionBalon) + ")", address);
    }
}
void arbolJugador7(Jugador jugador, MinimalSocket::udp::Udp<true> & socket, MinimalSocket::Address const & address){
    arbolJugador6(jugador, socket, address);
}
void arbolJugador8(Jugador jugador, MinimalSocket::udp::Udp<true> & socket, MinimalSocket::Address const & address){
    arbolJugador6(jugador, socket, address);
}

void arbolJugador9(Jugador jugador, MinimalSocket::udp::Udp<true> & socket, MinimalSocket::Address const & address){
    arbolJugador10(jugador, socket, address);
}

void arbolJugador10(Jugador jugador, MinimalSocket::udp::Udp<true> & socket, MinimalSocket::Address const & address){

    if(jugador.flags.distanciaBalon <= 1 && jugador.infoEquipo.distPorteriaRival <= 30 && jugador.infoEquipo.distPorteriaRival != -9343){ 
        socket.sendTo(golpearBalon("100", to_string(jugador.infoEquipo.oriPorteriaRival + 7)), address);       
    }
    else if(jugador.flags.distanciaBalon <= 1 && jugador.infoEquipo.distPorteriaRival > 30){ 
        socket.sendTo(golpearBalon("20", to_string(jugador.infoEquipo.oriPorteriaRival)), address);
    } //si no veo la porteria y tengo que moverme, p
    else if(jugador.flags.distanciaBalon <= 1 && jugador.infoEquipo.distPorteriaRival == -9343){ 
        if(((jugador.equipo=="l") && (jugador.flags.distanciaPorteriaIzq != -9343)) || ((jugador.equipo=="r") && (jugador.flags.distanciaPorteriaDer != -9343))){
            socket.sendTo(golpearBalon("40", "180"), address);
        }
        else if (((jugador.equipo=="l") && (jugador.flags.distanciaCornerIzq1!= -9343)) || ((jugador.equipo=="r") && (jugador.flags.distanciaCornerDer1 != -9343))){
            socket.sendTo(golpearBalon("25", "120"), address);
        }
        else if (((jugador.equipo=="l") && (jugador.flags.distanciaCornerIzq2!= -9343)) || ((jugador.equipo=="r") && (jugador.flags.distanciaCornerDer2 != -9343))){
            socket.sendTo(golpearBalon("25", "-120"), address);
        }
        else if (jugador.flags.distanciaCentroCampo1!= -9343){
            if(jugador.equipo=="l")
                socket.sendTo(golpearBalon("25", "90"), address);
            else
                socket.sendTo(golpearBalon("25", "-90"), address);
        }
        else if (jugador.flags.distanciaCentroCampo2!= -9343){
            if(jugador.equipo=="l")
                socket.sendTo(golpearBalon("25", "-90"), address);
            else
                socket.sendTo(golpearBalon("25", "90"), address);
        }

        else if (((jugador.equipo=="l") && (jugador.flags.distanciaCornerDer1 != -9343)) || ((jugador.equipo=="r") && (jugador.flags.distanciaCornerIzq2 != -9343))){
            socket.sendTo(golpearBalon("20", "20"), address);
        }
        else if (((jugador.equipo=="l") && (jugador.flags.distanciaCornerDer2 != -9343)) || ((jugador.equipo=="r") && (jugador.flags.distanciaCornerIzq1 != -9343))){
            socket.sendTo(golpearBalon("20", "-20"), address);        
        }
    } 
    else if((jugador.flags.orientacionBalon > 30 ))
        socket.sendTo(orientarJugador(to_string(jugador.flags.orientacionBalon)), address);
    else if(jugador.flags.orientacionBalon < -30)
        socket.sendTo(orientarJugador(to_string(jugador.flags.orientacionBalon)), address);
    else if(jugador.estadoPartido.saqueBanda && jugador.estadoPartido.saqueBandaYo){
            if((jugador.infoEquipo.minDistCompa < jugador.flags.distanciaBalon) && (jugador.infoEquipo.minDistCompa != -9343))
                socket.sendTo(("(dash 0)"),address);
            else{ 
                if(jugador.flags.distanciaBalon <= 1){
                socket.sendTo(golpearBalon("30", "180"), address);    
                }
                else if (jugador.flags.distanciaBalon > 1 && jugador.flags.distanciaBalon <= 1.5){
                    socket.sendTo(("(dash 50)"),address);
                }
                else{
                    socket.sendTo(("(dash 100)"),address);
                }
            }
    }
    else if (jugador.estadoPartido.saqueBanda && !jugador.estadoPartido.saqueBandaYo)
        socket.sendTo(("(dash 0)"),address);
    else if(jugador.estadoPartido.saquePorteria)
        socket.sendTo(("(dash 0)"),address);
    else if (((jugador.equipo=="l") && (((jugador.flags.distanciaPorteriaIzq > 45) && (jugador.flags.distanciaPorteriaIzq < 50)) && (jugador.flags.distanciaPorteriaIzq != -9343))) || ((jugador.equipo=="r") && (((jugador.flags.distanciaPorteriaDer > 45) && (jugador.flags.distanciaPorteriaDer < 50)) && (jugador.flags.distanciaPorteriaDer != -9343))))
        socket.sendTo("(dash 0)", address);
    else if (jugador.flags.distanciaBalon > 1){
        if((jugador.infoEquipo.minDistCompa < jugador.flags.distanciaBalon) && (jugador.flags.distanciaBalon  != -9343) && (jugador.infoEquipo.minDistCompa != -9343)){
            socket.sendTo("(dash 50)", address);
        }else{
            socket.sendTo("(dash 100)", address);
        }
    }

}

void arbolJugador11(Jugador jugador, MinimalSocket::udp::Udp<true> & socket, MinimalSocket::Address const & address){

    if(jugador.estadoPartido.enJuego && jugador.estadoPartido.kickOff && jugador.flags.distanciaBalon <= 1 && jugador.equipo=="l"){
            socket.sendTo(golpearBalon("45", "92"), address);
            jugador.estadoPartido.kickOff=false;
    }
    arbolJugador10(jugador, socket, address);

}

void informacionVision(Jugador & jugador){

    if(jugador.equipo == "l"){
        jugador.infoEquipo.distPorteriaRival = jugador.flags.distanciaPorteriaDer;
        jugador.infoEquipo.oriPorteriaRival = jugador.flags.orientacionPorteriaDer;
        jugador.infoEquipo.distMiPorteria = jugador.flags.distanciaPorteriaIzq;
    }else{
        jugador.infoEquipo.distPorteriaRival = jugador.flags.distanciaPorteriaIzq;
        jugador.infoEquipo.oriPorteriaRival = jugador.flags.orientacionPorteriaIzq;
        jugador.infoEquipo.distMiPorteria = jugador.flags.distanciaPorteriaDer;
    }

    if(jugador.flags.enemigosCerca.size() > 0){
    float minEnemigo = jugador.flags.enemigosCerca[0].first;
    float initOriEnem = jugador.flags.enemigosCerca[0].second;
        for (auto enemigo : jugador.flags.enemigosCerca){
            minEnemigo = min(minEnemigo, enemigo.first);
            initOriEnem = enemigo.second;
        }
        jugador.infoEquipo.minDistEnem = minEnemigo;
        jugador.infoEquipo.minOriEnem = initOriEnem;
    
    }

    if(jugador.flags.compañerosCerca.size() > 0){
    float initDistCompCerca = jugador.flags.compañerosCerca[0].first;
    float initOriCompCerca = jugador.flags.compañerosCerca[0].second;
    float initDistCompLejos = jugador.flags.compañerosCerca[0].first;
    float initOriCompLejos = jugador.flags.compañerosCerca[0].second;
        for (auto compañero : jugador.flags.compañerosCerca){
            initDistCompCerca = min(initDistCompCerca, compañero.first);
            initOriCompCerca = compañero.second;
            initDistCompLejos = max(initDistCompLejos, compañero.first);
            initOriCompLejos = compañero.second;
        }
    jugador.infoEquipo.minDistCompa = initDistCompCerca;
    jugador.infoEquipo.minOriCompa = initOriCompCerca;
    jugador.infoEquipo.maxDistCompa = initDistCompLejos;
    jugador.infoEquipo.maxOriCompa = initOriCompLejos;
    }
}