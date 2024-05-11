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
    {-51, 0}, {-30, -30}, {-35, -10}, {-35, 10}, {-30, 30}, 
    {-25, -10}, {-25, 10}, {-11, 0}, {-2, -27}, {-2, 27}, {-1, 0}, {-2, 0}
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
    if(jugador.estadoPartido.colocarse == true){
        colocarJugadorSegunNumero(jugador, socket, address);
        jugador.estadoPartido.colocarse = false;
        jugador.estadoPartido.enJuego = true;
    }else if (!jugador.siguienteComando.empty()){
        //Este case solo ocurre cuando queremos obligar a un jugador a hacer algo por ejemplo despues de kickoff el pase es hacia detras si o si
        jugador.siguienteComando.clear();//borramos el comando porque ya lo hemos ejecutado
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
            arbolJugador3(jugador, socket, address);
            break;
        case 4:
            arbolJugador4(jugador, socket, address);
            break;
        case 5:
            arbolJugador5(jugador, socket, address);
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
            arbolJugador9(jugador, socket, address);
            break;
        case 10:
            arbolJugador10(jugador, socket, address);
            break;
        case 11:
            arbolJugador11(jugador, socket, address);
            break;
        default:
            break;
        }

    }
}


void arbolJugador1(Jugador jugador, MinimalSocket::udp::Udp<true> & socket, MinimalSocket::Address const & address){
    static bool pelotaEnManos = false;
    if(pelotaEnManos){
        socket.sendTo(golpearBalon("100", to_string(jugador.flags.orientacionPorteriaDer)), address);
        pelotaEnManos=0;
    }else if((jugador.flags.orientacionBalon > 20)){
        socket.sendTo(orientarJugador(to_string(jugador.flags.orientacionBalon)), address);
    }else if(jugador.flags.orientacionBalon < -20){
        socket.sendTo(orientarJugador(to_string(jugador.flags.orientacionBalon)), address);
    }
    else if(jugador.flags.distanciaBalon<2){
        socket.sendTo("(catch " + to_string(jugador.flags.orientacionBalon) + ")", address);
        pelotaEnManos=0;
    }
}
void arbolJugador2(Jugador jugador, MinimalSocket::udp::Udp<true> & socket, MinimalSocket::Address const & address){
    
        float distEnemCerca = -1;
        float distCompCerca = -1;
        float oriCompCerca = -1;
        float compañeroMasLejano = -1;
        float flagFondoMasCercana = -1;
    
        /*
        
            Estos dos if sacan la distancia mas cercana de un enemigo y de un compañero
            La del enemigo para hacer tackle y la del compañero para pasarle el balon si hay
        
        */
    
        if(jugador.flags.enemigosCerca.size() > 0){
        float minEnemigo = jugador.flags.enemigosCerca[0].first;
            for (auto enemigo : jugador.flags.enemigosCerca){
                minEnemigo = min(minEnemigo, enemigo.first);
            }
            distEnemCerca = minEnemigo;
        }
        if(jugador.flags.compañerosCerca.size() > 0){
        float initDistCompCerca = jugador.flags.compañerosCerca[0].first;
        float initOriCompCerca = jugador.flags.compañerosCerca[0].second;
            for (auto compañero : jugador.flags.compañerosCerca){
                initDistCompCerca = min(initDistCompCerca, compañero.first);
                initOriCompCerca = compañero.second;
            }
            distCompCerca = initDistCompCerca;
            oriCompCerca = initOriCompCerca;
        }

        if(jugador.flags.flagsFondo.size() > 0){
            float intiFLagFondoMasCercana = jugador.flags.flagsFondo[0];
            for (auto flagFondo : jugador.flags.flagsFondo){
                intiFLagFondoMasCercana = min(intiFLagFondoMasCercana, flagFondo);
            }
            flagFondoMasCercana = intiFLagFondoMasCercana;
        }

         /* Aqui nos orientamos  */
        if((jugador.flags.orientacionBalon > 10))
            socket.sendTo(orientarJugador(to_string(jugador.flags.orientacionBalon)), address);
        else if(jugador.flags.orientacionBalon < -10)
            socket.sendTo(orientarJugador(to_string(jugador.flags.orientacionBalon)), address);
    

        /* Aqui la decision depende de la posicion del balon */
        if(jugador.flags.distanciaBalon < 0.6 && jugador.flags.distanciaPorteriaDer < 30){
            socket.sendTo(golpearBalon("60", to_string(jugador.orientacionPorteria)), address);
        }
        //si estamos demasiado cerca del centro del campo corremos hacia atras para volver a nuestra posicion
        else if(flagFondoMasCercana < 30){
            cout << "flag fondo mas cercana: " << flagFondoMasCercana << endl;
            if(jugador.flags.distanciaBalon < 0.6 && distCompCerca < 20){
                socket.sendTo(golpearBalon("20", to_string(oriCompCerca)), address);
                socket.sendTo("(dash 50 180)", address);
            }
            else
                socket.sendTo("(dash 50 180)", address);
        }// si tenemos el balon cerca pasamos a un compañero
        else if(jugador.flags.distanciaBalon < 0.6 && jugador.flags.distanciaPorteriaDer > 30 && distCompCerca > 10){
            socket.sendTo(golpearBalon("40", to_string(oriCompCerca)), address);
        } //si estamos lejos de la porteria, tenemos el balon cerca y no hay buen pase corremos con el balon
        else if(jugador.flags.distanciaBalon < 0.6 && jugador.flags.distanciaPorteriaDer > 30 && distCompCerca < 10){
            socket.sendTo(golpearBalon("10", to_string(oriCompCerca)), address);
        } // si no podemos pasar, correr o tirar entonces vamos a por el balon
        else if(jugador.flags.distanciaBalon > 3){
            socket.sendTo("(dash 70)", address);
        }
   
}
void arbolJugador3(Jugador jugador, MinimalSocket::udp::Udp<true> & socket, MinimalSocket::Address const & address){

}

void arbolJugador4(Jugador jugador, MinimalSocket::udp::Udp<true> & socket, MinimalSocket::Address const & address){

}
void arbolJugador5(Jugador jugador, MinimalSocket::udp::Udp<true> & socket, MinimalSocket::Address const & address){

}
void arbolJugador6(Jugador jugador, MinimalSocket::udp::Udp<true> & socket, MinimalSocket::Address const & address){

}
void arbolJugador7(Jugador jugador, MinimalSocket::udp::Udp<true> & socket, MinimalSocket::Address const & address){

}
void arbolJugador8(Jugador jugador, MinimalSocket::udp::Udp<true> & socket, MinimalSocket::Address const & address){

}
void arbolJugador9(Jugador jugador, MinimalSocket::udp::Udp<true> & socket, MinimalSocket::Address const & address){

}
void arbolJugador10(Jugador jugador, MinimalSocket::udp::Udp<true> & socket, MinimalSocket::Address const & address){

}
void arbolJugador11(Jugador jugador, MinimalSocket::udp::Udp<true> & socket, MinimalSocket::Address const & address){

    float distEnemCerca = -1;
    float distCompCerca = -1;
    float oriCompCerca = -1;
    float compañeroMasLejano = -1;

    /*
    
        Estos dos if sacan la distancia mas cercana de un enemigo y de un compañero
        La del enemigo para hacer tackle y la del compañero para pasarle el balon si hay
    
    */

    if(jugador.flags.enemigosCerca.size() > 0){
    float minEnemigo = jugador.flags.enemigosCerca[0].first;
        for (auto enemigo : jugador.flags.enemigosCerca){
            minEnemigo = min(minEnemigo, enemigo.first);
        }
        distEnemCerca = minEnemigo;
    }


    if(jugador.flags.compañerosCerca.size() > 0){
    float initDistCompCerca = jugador.flags.compañerosCerca[0].first;
    float initOriCompCerca = jugador.flags.compañerosCerca[0].second;
        for (auto compañero : jugador.flags.compañerosCerca){
            initDistCompCerca = min(initDistCompCerca, compañero.first);
            initOriCompCerca = compañero.second;
        }
        distCompCerca = initDistCompCerca;
        oriCompCerca = initOriCompCerca;
    }

    /*
    
        Ahora sabiendo esto podemos empezar a pensar con la distancia del balon y las flags
        Además toda esta parte podria ser una funcion que compartan todos que sea algo como
        funcion defensiva
    
    */

   /*Falta una funcion que nos diga a que lado estamos mirando, si vemos las flags de nuestro campo entonces tenemos que hacer 180 en todo*/


    /* Aqui nos orientamos  */
    if((jugador.flags.orientacionBalon > 10))
        socket.sendTo(orientarJugador(to_string(jugador.flags.orientacionBalon)), address);
    else if(jugador.flags.orientacionBalon < -10)
        socket.sendTo(orientarJugador(to_string(jugador.flags.orientacionBalon)), address);
 

    /* Aqui la decision depende de la posicion del balon */
    if(jugador.flags.distanciaBalon < 0.6 && jugador.flags.distanciaPorteriaDer < 30){
        socket.sendTo(golpearBalon("60", to_string(jugador.orientacionPorteria)), address);
    }
      //si estamos lejos de la porteria o pasamos o continuamos avanzando
    else if (jugador.flags.distanciaBalon < 0.6 && jugador.flags.distanciaPorteriaDer > 30 && distCompCerca > 10){
        socket.sendTo(golpearBalon("40", to_string(oriCompCerca)), address);
    } //si estamos lejos de la porteria, tenemos el balon cerca y no hay buen pase corremos con el balon
    else if (jugador.flags.distanciaBalon < 0.6 && jugador.flags.distanciaPorteriaDer > 30 && distCompCerca < 10){
        socket.sendTo(golpearBalon("10", to_string(oriCompCerca)), address);
    } // si no podemos pasar, correr o tirar entonces vamos a por el balon
    else if(jugador.flags.distanciaBalon > 0.6){
        socket.sendTo("(dash 70)", address);
    }

}



void decidirComando(Jugador & jugador, MinimalSocket::udp::Udp<true> & socket, MinimalSocket::Address const & address){
    static bool aux=0;
    const float velocidadBase = 50;
    static bool PelotaenManos=0;

    if((jugador.estadoPartido.colocarse == false) && (aux == 0) ){ //&& (jugador.EnJuego==true)
        switch(jugador.tipoJugador){
            case 0:
                if(PelotaenManos){
                    socket.sendTo(golpearBalon("100", to_string(jugador.orientacionPorteria)), address);
                    PelotaenManos=0;
                }else if((jugador.flags.orientacionBalon > 10)){
                    socket.sendTo(orientarJugador(to_string(jugador.flags.orientacionBalon)), address);
                }else if(jugador.flags.orientacionBalon < -10){
                    socket.sendTo(orientarJugador(to_string(jugador.flags.orientacionBalon)), address);
                }
                else if(jugador.flags.distanciaBalon<2){
                    socket.sendTo("(catch " + to_string(jugador.flags.orientacionBalon) + ")", address);
                    PelotaenManos=1;
                }
                break;
            case 1:
                if((jugador.flags.orientacionBalon > 10))
                    socket.sendTo(orientarJugador(to_string(jugador.flags.orientacionBalon)), address);
                else if(jugador.flags.orientacionBalon < -10)
                    socket.sendTo(orientarJugador(to_string(jugador.flags.orientacionBalon)), address);
                else if(jugador.flags.distanciaBalon > 30)
                    socket.sendTo("(dash " + to_string(30) + ")", address);
                else if((jugador.flags.distanciaBalon < 30) && (jugador.flags.distanciaBalon > 5))
                    socket.sendTo("(dash 5)", address);
                else if((jugador.flags.distanciaBalon < 5) && (jugador.flags.distanciaBalon > 0.6))
                    socket.sendTo("(dash 50)", address);
                else if((jugador.flags.distanciaBalon < 0.6) && (jugador.hayPase==false))
                    socket.sendTo(golpearBalon("60", to_string(jugador.orientacionPorteria)), address);
                else if((jugador.hayPase) && (jugador.flags.distanciaBalon<0.6)){
                    socket.sendTo(golpearBalon("10", to_string(jugador.orientacionPase)), address);
                    jugador.hayPase=false;
                }
                break;
            case 2:
                if((jugador.flags.orientacionBalon > 10))
                    socket.sendTo(orientarJugador(to_string(jugador.flags.orientacionBalon)), address);
                else if(jugador.flags.orientacionBalon < -10)
                    socket.sendTo(orientarJugador(to_string(jugador.flags.orientacionBalon)), address);
                else if(jugador.flags.distanciaBalon > 15)
                    socket.sendTo("(dash " + to_string(30) + ")", address);
                else if((jugador.flags.distanciaBalon < 15) && (jugador.flags.distanciaBalon > 5))
                    socket.sendTo("(dash 5)", address);
                    else if((jugador.flags.distanciaBalon < 5) && (jugador.flags.distanciaBalon > 0.6))
                    socket.sendTo("(dash 50)", address);
                else if((jugador.flags.distanciaBalon < 0.6) && (jugador.hayPase==false))
                    socket.sendTo(golpearBalon("60", to_string(jugador.orientacionPorteria)), address);
                else if((jugador.hayPase) && (jugador.flags.distanciaBalon<0.6)){
                    socket.sendTo(golpearBalon("10", to_string(jugador.orientacionPase)), address);
                    jugador.hayPase=false;
                }
                break;
            case 3:
                if((jugador.flags.orientacionBalon > 10))
                    socket.sendTo(orientarJugador(to_string(jugador.flags.orientacionBalon)), address);
                else if(jugador.flags.orientacionBalon < -10)
                    socket.sendTo(orientarJugador(to_string(jugador.flags.orientacionBalon)), address);
                else if((jugador.flags.distanciaBalon < 0.6) && (jugador.hayPase==false))
                    socket.sendTo(golpearBalon("60", to_string(jugador.orientacionPorteria)), address);
                else if((jugador.hayPase) && (jugador.flags.distanciaBalon<0.6)){
                    socket.sendTo(golpearBalon("10", to_string(jugador.orientacionPase)), address);
                    jugador.hayPase=false;
                }    
                else{
                    float velocidad = (jugador.flags.distanciaBalon * 100) / velocidadBase ;
                    if (velocidad < velocidadBase)
                        velocidad = velocidadBase;
                    socket.sendTo("(dash " + to_string(velocidad) + ")", address);
                }
                break;
        }
    }else if((aux==1) ){ //&& (jugador.EnJuego==false)
        aux=0;
        
        socket.sendTo("(turn "+to_string(jugador.flags.orientacionBalon)+")", address);
        jugador.estadoPartido.enJuego = true;
        
    }else if((jugador.estadoPartido.colocarse == true) ){

        cout << "colocar jugador - Estado del partido "  << jugador.estadoPartido.enJuego << endl;
        colocarJugadorSegunNumero(jugador, socket, address);
        jugador.estadoPartido.colocarse = false;
        jugador.estadoPartido.enJuego = false;
        aux=1;
    }
}
