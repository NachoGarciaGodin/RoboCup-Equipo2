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
        jugador.estadoPartido.enJuego = false;
        cout << "Colocando jugador: " << endl;
        colocarJugadorSegunNumero(jugador, socket, address);
        jugador.estadoPartido.colocarse = false;
    }else if (!jugador.siguienteComando.empty()){
        //Este case solo ocurre cuando queremos obligar a un jugador a hacer algo por ejemplo despues de kickoff el pase es hacia detras si o si
        cout << "Siguiente comando: " << jugador.siguienteComando << endl;
        socket.sendTo(jugador.siguienteComando, address);
        jugador.siguienteComando.clear();
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
            arbolJugador6(jugador, socket, address);
            break;
        case 8:
            arbolJugador6(jugador, socket, address);
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
    }else if(jugador.flags.distanciaBalon <= 15 && jugador.flags.distanciaBalon != -9343){
        socket.sendTo(("(dash 50 " + to_string(jugador.flags.orientacionBalon) + ")"), address);
    }else if(jugador.flags.distanciaBalon > 15){
        socket.sendTo(("(dash 1 180)"), address);
    }
    else if(jugador.flags.distanciaBalon<2){
        socket.sendTo("(catch " + to_string(jugador.flags.orientacionBalon) + ")", address);
        pelotaEnManos=1;
    }
}
void arbolJugador2(Jugador jugador, MinimalSocket::udp::Udp<true> & socket, MinimalSocket::Address const & address){
    
    
    float distEnemCerca = -1;
    float oriEnemCerca = -1;
    float distCompCerca = -1;
    float oriCompCerca = -1;
    float compañeroMasLejano = -1;

    float auxDistPorteriaContraria = -9343;
    float auxOriPorteriaContraria = -9343;
     

    if(jugador.equipo == "l"){
        auxDistPorteriaContraria = jugador.flags.distanciaPorteriaDer;
        auxOriPorteriaContraria = jugador.flags.orientacionPorteriaDer;
    }else{
        auxDistPorteriaContraria = jugador.flags.distanciaPorteriaIzq;
        auxOriPorteriaContraria = jugador.flags.orientacionPorteriaIzq;
    }
        

    /*

        Estos dos if sacan la distancia mas cercana de un enemigo y de un compañero
        La del enemigo para hacer tackle y la del compañero para pasarle el balon si hay

    */

    if(jugador.flags.enemigosCerca.size() > 0){
    float minEnemigo = jugador.flags.enemigosCerca[0].first;
    float initOriEnem = jugador.flags.enemigosCerca[0].second;
        for (auto enemigo : jugador.flags.enemigosCerca){
            minEnemigo = min(minEnemigo, enemigo.first);
            initOriEnem = enemigo.second;
        }
        distEnemCerca = minEnemigo;
        oriEnemCerca = initOriEnem;

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

    if(jugador.flags.flagsPorteria.size() > 0){
            float intiFLagFondoMasCercana = jugador.flags.flagsPorteria[0];
            for (auto flagFondo : jugador.flags.flagsPorteria){
            }
        }

    /*
    
        Ahora sabiendo esto podemos empezar a pensar con la distancia del balon y las flags
        Además toda esta parte podria ser una funcion que compartan todos que sea algo como
        funcion defensiva
    
    */

    /*Falta una funcion que nos diga a que lado estamos mirando, si vemos las flags de nuestro campo entonces tenemos que hacer 180 en todo*/
    /* Aqui nos orientamos  */
    if((jugador.flags.orientacionBalon > 10 ))
        socket.sendTo(orientarJugador(to_string(jugador.flags.orientacionBalon)), address);
    else if(jugador.flags.orientacionBalon < -10)
        socket.sendTo(orientarJugador(to_string(jugador.flags.orientacionBalon)), address);



    /* Aqui la decision depende de la posicion del balon */
    if(jugador.flags.distanciaBalon <= 0.6 && auxDistPorteriaContraria <= 25 && auxDistPorteriaContraria != -9343){
        cout << "Chuto a puerta" << endl;
        cout << "Distancia porteria: " << auxDistPorteriaContraria << endl;

        socket.sendTo(golpearBalon("60", to_string(auxOriPorteriaContraria + 3)), address);        
    }// si tenemos el balon cerca pasamos a un compañero
     else if(jugador.flags.distanciaBalon <= 0.6 && auxDistPorteriaContraria > 25 && (distCompCerca > 10 || distCompCerca < 30 )
      && distCompCerca != -1 && auxDistPorteriaContraria != -9343){
        socket.sendTo(golpearBalon(to_string(distCompCerca), to_string(oriCompCerca)), address);
    } //si estamos lejos de la porteria, tenemos el balon cerca y no hay buen pase corremos con el balon
    else if(jugador.flags.distanciaBalon <= 0.6 && auxDistPorteriaContraria > 25 && (distCompCerca < 10 || distCompCerca > 30 )){
        socket.sendTo(golpearBalon("10", to_string(auxOriPorteriaContraria)), address);
    } //si no veo la porteria y tengo que moverme, p
     else if(jugador.flags.distanciaBalon <= 0.6 && auxDistPorteriaContraria == -9343 && (distCompCerca < 10 || distCompCerca > 30 )){
        socket.sendTo(golpearBalon("10", "0"), address); 
    } // si estamos cerca del balon y hay un enemigo cerca hacemos tackle
    else if (jugador.flags.distanciaBalon <= 0.6 && distEnemCerca <= 0.6 && distEnemCerca != -1){
        cout << "Tackleo" << endl;
        socket.sendTo(("(tackle " + to_string(oriEnemCerca) + ")"), address);
    }
    else if(jugador.flags.distanciaBalon > 0.6){
        //si estamos muy cerca del centro del campo volvemos a la porteria
        if (auxDistPorteriaContraria < 60 && auxDistPorteriaContraria != -9343)
        {
            cout << "Vuelvo a mi posicion" << endl;
            socket.sendTo("(dash 50 180)", address);
        }//si hay alguien mas cerca que yo del balon le acompaño aunque lento por si acaso
        else if(distCompCerca < jugador.flags.distanciaBalon && jugador.flags.distanciaBalon  != -9343 && distCompCerca != -1){
            socket.sendTo("(dash 10)", address);
        }
        //si no hay nadie cerca del balon es nuestro
        else if(distCompCerca == -1 ){
            socket.sendTo("(dash 70)", address);
        }else{
            //TODO  - No hace falta que intervengamos en el juego
            return;
        }
    }
   
}
void arbolJugador3(Jugador jugador, MinimalSocket::udp::Udp<true> & socket, MinimalSocket::Address const & address){

    accionesRepetidas(jugador, socket, address);


}

void arbolJugador4(Jugador jugador, MinimalSocket::udp::Udp<true> & socket, MinimalSocket::Address const & address){
    accionesRepetidas(jugador, socket, address);

}
void arbolJugador5(Jugador jugador, MinimalSocket::udp::Udp<true> & socket, MinimalSocket::Address const & address){
    accionesRepetidas(jugador, socket, address);

}
void arbolJugador6(Jugador jugador, MinimalSocket::udp::Udp<true> & socket, MinimalSocket::Address const & address){
    
    float distEnemCerca = -1;
    float oriEnemCerca = -1;
    float distCompCerca = -1;
    float oriCompCerca = -1;
    float compañeroMasLejano = -1;

    float auxDistPorteriaContraria = -1;
    float auxOriPorteriaContraria = -1;
     

    if(jugador.equipo == "l"){
        auxDistPorteriaContraria = jugador.flags.distanciaPorteriaDer;
        auxOriPorteriaContraria = jugador.flags.orientacionPorteriaDer;
    }else{
        auxDistPorteriaContraria = jugador.flags.distanciaPorteriaIzq;
        auxOriPorteriaContraria = jugador.flags.orientacionPorteriaIzq;
    }
        

    /*

        Estos dos if sacan la distancia mas cercana de un enemigo y de un compañero
        La del enemigo para hacer tackle y la del compañero para pasarle el balon si hay

    */

    if(jugador.flags.enemigosCerca.size() > 0){
    float minEnemigo = jugador.flags.enemigosCerca[0].first;
    float initOriEnem = jugador.flags.enemigosCerca[0].second;
        for (auto enemigo : jugador.flags.enemigosCerca){
            minEnemigo = min(minEnemigo, enemigo.first);
            initOriEnem = enemigo.second;
        }
        distEnemCerca = minEnemigo;
        oriEnemCerca = initOriEnem;

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

    if(jugador.flags.flagsPorteria.size() > 0){
            float intiFLagFondoMasCercana = jugador.flags.flagsPorteria[0];
            for (auto flagFondo : jugador.flags.flagsPorteria){
            }
        }

    /*
    
        Ahora sabiendo esto podemos empezar a pensar con la distancia del balon y las flags
        Además toda esta parte podria ser una funcion que compartan todos que sea algo como
        funcion defensiva
    
    */

    /*Falta una funcion que nos diga a que lado estamos mirando, si vemos las flags de nuestro campo entonces tenemos que hacer 180 en todo*/
    /* Aqui nos orientamos  */
    if((jugador.flags.orientacionBalon > 10 ))
        socket.sendTo(orientarJugador(to_string(jugador.flags.orientacionBalon)), address);
    else if(jugador.flags.orientacionBalon < -10)
        socket.sendTo(orientarJugador(to_string(jugador.flags.orientacionBalon)), address);



    /* Aqui la decision depende de la posicion del balon */
    if(jugador.flags.distanciaBalon <= 0.6 && auxDistPorteriaContraria <= 25 && auxDistPorteriaContraria != -9343){
        cout << "Chuto a puerta" << endl;
        cout << "Distancia porteria: " << auxDistPorteriaContraria << endl;
        socket.sendTo(golpearBalon("60", to_string(auxOriPorteriaContraria + 3)), address);        
    }// si tenemos el balon cerca pasamos a un compañero
     else if(jugador.flags.distanciaBalon <= 0.6 && auxDistPorteriaContraria > 25 && (distCompCerca > 10 || distCompCerca < 30 )
      && distCompCerca != -1 && auxDistPorteriaContraria != -9343){
        socket.sendTo(golpearBalon(to_string(distCompCerca), to_string(oriCompCerca)), address);
    } //si estamos lejos de la porteria, tenemos el balon cerca y no hay buen pase corremos con el balon
    else if(jugador.flags.distanciaBalon <= 0.6 && auxDistPorteriaContraria > 25 && (distCompCerca < 10 || distCompCerca > 30 )){
        socket.sendTo(golpearBalon("10", to_string(auxOriPorteriaContraria)), address);
    } //si no veo la porteria y tengo que moverme, p
     else if(jugador.flags.distanciaBalon <= 0.6 && auxDistPorteriaContraria == -9343 && (distCompCerca < 10 || distCompCerca > 30 )){
        socket.sendTo(golpearBalon("10", "0"), address); 
    } // si estamos cerca del balon y hay un enemigo cerca hacemos tackle
    else if (jugador.flags.distanciaBalon <= 0.6 && distEnemCerca <= 0.6 && distEnemCerca != -1){        
        cout << "Tackleo" << endl;
        socket.sendTo(("(tackle " + to_string(oriEnemCerca) + ")"), address);
    }
    else if(jugador.flags.distanciaBalon > 0.6){
        //si estamos muy cerca del centro del campo volvemos a la porteria
        if (auxDistPorteriaContraria < 30 && auxDistPorteriaContraria != -9343)
        {
            cout << "Vuelvo a mi posicion" << endl;
            socket.sendTo("(dash 50 180)", address);
        }//si hay alguien mas cerca que yo del balon le acompaño aunque lento por si acaso
        else if(distCompCerca < jugador.flags.distanciaBalon && jugador.flags.distanciaBalon  != -9343 && distCompCerca != -1){
            socket.sendTo("(dash 20)", address);
        }
        else{
            socket.sendTo("(dash 80)", address);
        }
    }

}
void arbolJugador7(Jugador jugador, MinimalSocket::udp::Udp<true> & socket, MinimalSocket::Address const & address){
    accionesRepetidas(jugador, socket, address);

}
void arbolJugador8(Jugador jugador, MinimalSocket::udp::Udp<true> & socket, MinimalSocket::Address const & address){
    accionesRepetidas(jugador, socket, address);

}
void arbolJugador9(Jugador jugador, MinimalSocket::udp::Udp<true> & socket, MinimalSocket::Address const & address){
    float distEnemCerca = -1;
    float oriEnemCerca = -1;
    float distCompCerca = -1;
    float oriCompCerca = -1;
    float compañeroMasLejano = -1;

    float auxDistPorteriaContraria = -9343;
    float auxOriPorteriaContraria = -9343;
     

    if(jugador.equipo == "l"){
        auxDistPorteriaContraria = jugador.flags.distanciaPorteriaDer;
        auxOriPorteriaContraria = jugador.flags.orientacionPorteriaDer;
    }else{
        auxDistPorteriaContraria = jugador.flags.distanciaPorteriaIzq;
        auxOriPorteriaContraria = jugador.flags.orientacionPorteriaIzq;
    }
        

    /*

        Estos dos if sacan la distancia mas cercana de un enemigo y de un compañero
        La del enemigo para hacer tackle y la del compañero para pasarle el balon si hay

    */

    if(jugador.flags.enemigosCerca.size() > 0){
    float minEnemigo = jugador.flags.enemigosCerca[0].first;
    float initOriEnem = jugador.flags.enemigosCerca[0].second;
        for (auto enemigo : jugador.flags.enemigosCerca){
            minEnemigo = min(minEnemigo, enemigo.first);
            initOriEnem = enemigo.second;
        }
        distEnemCerca = minEnemigo;
        oriEnemCerca = initOriEnem;

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

    if(jugador.flags.flagsPorteria.size() > 0){
            float intiFLagFondoMasCercana = jugador.flags.flagsPorteria[0];
            for (auto flagFondo : jugador.flags.flagsPorteria){
            }
        }

    /*
    
        Ahora sabiendo esto podemos empezar a pensar con la distancia del balon y las flags
        Además toda esta parte podria ser una funcion que compartan todos que sea algo como
        funcion defensiva
    
    */

    /*Falta una funcion que nos diga a que lado estamos mirando, si vemos las flags de nuestro campo entonces tenemos que hacer 180 en todo*/
    /* Aqui nos orientamos  */
    if((jugador.flags.orientacionBalon > 10 ))
        socket.sendTo(orientarJugador(to_string(jugador.flags.orientacionBalon)), address);
    else if(jugador.flags.orientacionBalon < -10)
        socket.sendTo(orientarJugador(to_string(jugador.flags.orientacionBalon)), address);
    else if(jugador.flags.distanciaBalon <= 0.6 && auxDistPorteriaContraria <= 25 && auxDistPorteriaContraria != -9343){ /* Aqui la decision depende de la posicion del balon */
        cout << "Chuto a puerta" << endl;
        cout << "Distancia porteria: " << auxDistPorteriaContraria << endl;

        socket.sendTo(golpearBalon("60", to_string(auxOriPorteriaContraria + 3)), address);        
    }// si tenemos el balon cerca pasamos a un compañero
     else if(jugador.flags.distanciaBalon <= 0.6 && auxDistPorteriaContraria > 25 && (distCompCerca > 10 || distCompCerca < 30 )
      && distCompCerca != -1 && auxDistPorteriaContraria != -9343){
        socket.sendTo(golpearBalon(to_string(distCompCerca), to_string(oriCompCerca)), address);
    } //si estamos lejos de la porteria, tenemos el balon cerca y no hay buen pase corremos con el balon
    else if(jugador.flags.distanciaBalon <= 0.6 && auxDistPorteriaContraria > 25 && (distCompCerca < 10 || distCompCerca > 30 )){
        socket.sendTo(golpearBalon("10", to_string(auxOriPorteriaContraria)), address);
    } //si no veo la porteria y tengo que moverme, p
    else if(jugador.flags.distanciaBalon <= 0.6 && auxDistPorteriaContraria == -9343 && (distCompCerca < 10 || distCompCerca > 30 )){
        if(((jugador.equipo=="l") && ((jugador.flags.distanciaPorteriaIzq != -9343)||(jugador.flags.distanciaCornerIzq1!= -9343)||(jugador.flags.distanciaCornerIzq2!= -9343))) || ((jugador.equipo=="r") && ((jugador.flags.distanciaPorteriaDer != -9343)||(jugador.flags.distanciaCornerDer1!= -9343)||(jugador.flags.distanciaCornerDer2!= -9343))))
            socket.sendTo(golpearBalon("30", "180"), address); 
        else if (((jugador.equipo=="l") && (jugador.flags.distanciaPorteriaDer != -9343)) || ((jugador.equipo=="r") && (jugador.flags.distanciaPorteriaIzq != -9343)))
            socket.sendTo(golpearBalon("10", "0"), address);
        else if (((jugador.equipo=="l") && (jugador.flags.distanciaCornerDer1 != -9343)) || ((jugador.equipo=="r") && (jugador.flags.distanciaCornerIzq1 != -9343)))
            socket.sendTo(golpearBalon("10", "-20"), address);
        else if (((jugador.equipo=="l") && (jugador.flags.distanciaCornerDer2 != -9343)) || ((jugador.equipo=="r") && (jugador.flags.distanciaCornerIzq2 != -9343)))
            socket.sendTo(golpearBalon("10", "20"), address);
    } // si estamos cerca del balon y hay un enemigo cerca hacemos tackle
    else if (jugador.flags.distanciaBalon <= 0.6 && distEnemCerca <= 0.6 && distEnemCerca != -1){
            cout << "Tackleo" << endl;
        socket.sendTo(("(tackle " + to_string(oriEnemCerca) + ")"), address);
    }
    else if(jugador.flags.distanciaBalon > 0.6){
        //si estamos muy cerca del centro del campo volvemos a la porteria
        //si hay alguien mas cerca que yo del balon le acompaño aunque lento por si acaso
        if(distCompCerca < jugador.flags.distanciaBalon && jugador.flags.distanciaBalon  != -9343 && distCompCerca != -1){
            if((jugador.flags.distanciaBalon>0.6) && (((jugador.equipo=="l" && jugador.flags.distanciaPorteriaIzq != -9343) || (jugador.equipo=="r" && jugador.flags.distanciaPorteriaDer != -9343)) || ((jugador.equipo=="l" && jugador.flags.distanciaCornerIzq1!= -9343)||(jugador.equipo=="r" && jugador.flags.distanciaCornerDer1 != -9343)) || ((jugador.equipo=="l" && jugador.flags.distanciaCornerIzq2!= -9343)||(jugador.equipo=="r" && jugador.flags.distanciaCornerDer2 != -9343)))){
                if (((jugador.equipo=="l") && (jugador.flags.distanciaPorteriaIzq > 47)) || ((jugador.equipo=="r") && (jugador.flags.distanciaPorteriaDer > 47)) )
                    socket.sendTo("(dash 15)", address);
                else    
                     socket.sendTo("(dash 0)", address);
            }
            else
                socket.sendTo("(dash 20)", address);
        }
        else{
            socket.sendTo("(dash 80)", address);
        }
    }
}
void arbolJugador10(Jugador jugador, MinimalSocket::udp::Udp<true> & socket, MinimalSocket::Address const & address){
    float distEnemCerca = -1;
    float oriEnemCerca = -1;
    float distCompCerca = -1;
    float oriCompCerca = -1;
    float compañeroMasLejano = -1;

    float auxDistPorteriaContraria = -9343;
    float auxOriPorteriaContraria = -9343;
     

    if(jugador.equipo == "l"){
        auxDistPorteriaContraria = jugador.flags.distanciaPorteriaDer;
        auxOriPorteriaContraria = jugador.flags.orientacionPorteriaDer;
    }else{
        auxDistPorteriaContraria = jugador.flags.distanciaPorteriaIzq;
        auxOriPorteriaContraria = jugador.flags.orientacionPorteriaIzq;
    }
        

    /*

        Estos dos if sacan la distancia mas cercana de un enemigo y de un compañero
        La del enemigo para hacer tackle y la del compañero para pasarle el balon si hay

    */

    if(jugador.flags.enemigosCerca.size() > 0){
    float minEnemigo = jugador.flags.enemigosCerca[0].first;
    float initOriEnem = jugador.flags.enemigosCerca[0].second;
        for (auto enemigo : jugador.flags.enemigosCerca){
            minEnemigo = min(minEnemigo, enemigo.first);
            initOriEnem = enemigo.second;
        }
        distEnemCerca = minEnemigo;
        oriEnemCerca = initOriEnem;

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

    if(jugador.flags.flagsPorteria.size() > 0){
            float intiFLagFondoMasCercana = jugador.flags.flagsPorteria[0];
            for (auto flagFondo : jugador.flags.flagsPorteria){
            }
        }

    /*
    
        Ahora sabiendo esto podemos empezar a pensar con la distancia del balon y las flags
        Además toda esta parte podria ser una funcion que compartan todos que sea algo como
        funcion defensiva
    
    */

    /*Falta una funcion que nos diga a que lado estamos mirando, si vemos las flags de nuestro campo entonces tenemos que hacer 180 en todo*/
    /* Aqui nos orientamos  */
    if((jugador.flags.orientacionBalon > 10 ))
        socket.sendTo(orientarJugador(to_string(jugador.flags.orientacionBalon)), address);
    else if(jugador.flags.orientacionBalon < -10)
        socket.sendTo(orientarJugador(to_string(jugador.flags.orientacionBalon)), address);
    else if(jugador.flags.distanciaBalon <= 0.6 && auxDistPorteriaContraria <= 25 && auxDistPorteriaContraria != -9343){ /* Aqui la decision depende de la posicion del balon */
        cout << "Chuto a puerta" << endl;
        cout << "Distancia porteria: " << auxDistPorteriaContraria << endl;

        socket.sendTo(golpearBalon("60", to_string(auxOriPorteriaContraria + 3)), address);        
    }// si tenemos el balon cerca pasamos a un compañero
     else if(jugador.flags.distanciaBalon <= 0.6 && auxDistPorteriaContraria > 25 && (distCompCerca > 10 || distCompCerca < 30 )
      && distCompCerca != -1 && auxDistPorteriaContraria != -9343){
        socket.sendTo(golpearBalon(to_string(distCompCerca), to_string(oriCompCerca)), address);
    } //si estamos lejos de la porteria, tenemos el balon cerca y no hay buen pase corremos con el balon
    else if(jugador.flags.distanciaBalon <= 0.6 && auxDistPorteriaContraria > 25 && (distCompCerca < 10 || distCompCerca > 30 )){
        socket.sendTo(golpearBalon("10", to_string(auxOriPorteriaContraria)), address);
    } //si no veo la porteria y tengo que moverme, p
    else if(jugador.flags.distanciaBalon <= 0.6 && auxDistPorteriaContraria == -9343 && (distCompCerca < 10 || distCompCerca > 30 )){
        if(((jugador.equipo=="l") && ((jugador.flags.distanciaPorteriaIzq != -9343)||(jugador.flags.distanciaCornerIzq1!= -9343)||(jugador.flags.distanciaCornerIzq2!= -9343))) || ((jugador.equipo=="r") && ((jugador.flags.distanciaPorteriaDer != -9343)||(jugador.flags.distanciaCornerDer1!= -9343)||(jugador.flags.distanciaCornerDer2!= -9343))))
            socket.sendTo(golpearBalon("30", "180"), address); 
        else if (((jugador.equipo=="l") && (jugador.flags.distanciaPorteriaDer != -9343)) || ((jugador.equipo=="r") && (jugador.flags.distanciaPorteriaIzq != -9343)))
            socket.sendTo(golpearBalon("10", "0"), address);
        else if (((jugador.equipo=="l") && (jugador.flags.distanciaCornerDer1 != -9343)) || ((jugador.equipo=="r") && (jugador.flags.distanciaCornerIzq1 != -9343)))
            socket.sendTo(golpearBalon("10", "-20"), address);
        else if (((jugador.equipo=="l") && (jugador.flags.distanciaCornerDer2 != -9343)) || ((jugador.equipo=="r") && (jugador.flags.distanciaCornerIzq2 != -9343)))
            socket.sendTo(golpearBalon("10", "20"), address);
    } // si estamos cerca del balon y hay un enemigo cerca hacemos tackle
    else if (jugador.flags.distanciaBalon <= 0.6 && distEnemCerca <= 0.6 && distEnemCerca != -1){
            cout << "Tackleo" << endl;
        socket.sendTo(("(tackle " + to_string(oriEnemCerca) + ")"), address);
    }
    else if(jugador.flags.distanciaBalon > 0.6){
        //si estamos muy cerca del centro del campo volvemos a la porteria
        //si hay alguien mas cerca que yo del balon le acompaño aunque lento por si acaso
        if(distCompCerca < jugador.flags.distanciaBalon && jugador.flags.distanciaBalon  != -9343 && distCompCerca != -1){
            if((jugador.flags.distanciaBalon>0.6) && (((jugador.equipo=="l" && jugador.flags.distanciaPorteriaIzq != -9343) || (jugador.equipo=="r" && jugador.flags.distanciaPorteriaDer != -9343)) || ((jugador.equipo=="l" && jugador.flags.distanciaCornerIzq1!= -9343)||(jugador.equipo=="r" && jugador.flags.distanciaCornerDer1 != -9343)) || ((jugador.equipo=="l" && jugador.flags.distanciaCornerIzq2!= -9343)||(jugador.equipo=="r" && jugador.flags.distanciaCornerDer2 != -9343)))){
                if (((jugador.equipo=="l") && (jugador.flags.distanciaPorteriaIzq > 47)) || ((jugador.equipo=="r") && (jugador.flags.distanciaPorteriaDer > 47)) )
                    socket.sendTo("(dash 15)", address);
            }
            else
                socket.sendTo("(dash 20)", address);
        }
        else{
            socket.sendTo("(dash 80)", address);
        }
    }
    

}
void arbolJugador11(Jugador jugador, MinimalSocket::udp::Udp<true> & socket, MinimalSocket::Address const & address){

    float distEnemCerca = -1;
    float oriEnemCerca = -1;
    float distCompCerca = -1;
    float oriCompCerca = -1;
    float compañeroMasLejano = -1;

    float auxDistPorteriaContraria = -9343;
    float auxOriPorteriaContraria = -9343;
     

    if(jugador.equipo == "l"){
        auxDistPorteriaContraria = jugador.flags.distanciaPorteriaDer;
        auxOriPorteriaContraria = jugador.flags.orientacionPorteriaDer;
    }else{
        auxDistPorteriaContraria = jugador.flags.distanciaPorteriaIzq;
        auxOriPorteriaContraria = jugador.flags.orientacionPorteriaIzq;
    }
        

    /*

        Estos dos if sacan la distancia mas cercana de un enemigo y de un compañero
        La del enemigo para hacer tackle y la del compañero para pasarle el balon si hay

    */

    if(jugador.flags.enemigosCerca.size() > 0){
    float minEnemigo = jugador.flags.enemigosCerca[0].first;
    float initOriEnem = jugador.flags.enemigosCerca[0].second;
        for (auto enemigo : jugador.flags.enemigosCerca){
            minEnemigo = min(minEnemigo, enemigo.first);
            initOriEnem = enemigo.second;
        }
        distEnemCerca = minEnemigo;
        oriEnemCerca = initOriEnem;

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

    if(jugador.flags.flagsPorteria.size() > 0){
            float intiFLagFondoMasCercana = jugador.flags.flagsPorteria[0];
            for (auto flagFondo : jugador.flags.flagsPorteria){
            }
        }

    /*
    
        Ahora sabiendo esto podemos empezar a pensar con la distancia del balon y las flags
        Además toda esta parte podria ser una funcion que compartan todos que sea algo como
        funcion defensiva
    
    */

    /*Falta una funcion que nos diga a que lado estamos mirando, si vemos las flags de nuestro campo entonces tenemos que hacer 180 en todo*/
    /* Aqui nos orientamos  */
    if((jugador.flags.orientacionBalon > 10 ))
        socket.sendTo(orientarJugador(to_string(jugador.flags.orientacionBalon)), address);
    else if(jugador.flags.orientacionBalon < -10)
        socket.sendTo(orientarJugador(to_string(jugador.flags.orientacionBalon)), address);
    else if(jugador.flags.distanciaBalon <= 0.6 && auxDistPorteriaContraria <= 25 && auxDistPorteriaContraria != -9343){ /* Aqui la decision depende de la posicion del balon */
        cout << "Chuto a puerta" << endl;
        cout << "Distancia porteria: " << auxDistPorteriaContraria << endl;

        socket.sendTo(golpearBalon("60", to_string(auxOriPorteriaContraria + 3)), address);        
    }// si tenemos el balon cerca pasamos a un compañero
    else if(jugador.flags.distanciaBalon <= 0.6 && auxDistPorteriaContraria > 25 && (distCompCerca > 10 || distCompCerca < 30 ) && distCompCerca != -1 && auxDistPorteriaContraria != -9343){
        socket.sendTo(golpearBalon(to_string(distCompCerca), to_string(oriCompCerca)), address);
    } //si estamos lejos de la porteria, tenemos el balon cerca y no hay buen pase corremos con el balon
    else if(jugador.flags.distanciaBalon <= 0.6 && auxDistPorteriaContraria > 25 && (distCompCerca < 10 || distCompCerca > 30 )){
        if(jugador.estadoPartido.enJuego && jugador.estadoPartido.kickOff){
            socket.sendTo(golpearBalon("40", "180"), address);
            jugador.estadoPartido.kickOff=false;
        }
        else
            socket.sendTo(golpearBalon("10", to_string(auxOriPorteriaContraria)), address);
    } //si no veo la porteria y tengo que moverme, p
    else if(jugador.flags.distanciaBalon <= 0.6 && auxDistPorteriaContraria == -9343 && (distCompCerca < 10 || distCompCerca > 30 )){
        if(((jugador.equipo=="l") && ((jugador.flags.distanciaPorteriaIzq != -9343)||(jugador.flags.distanciaCornerIzq1!= -9343)||(jugador.flags.distanciaCornerIzq2!= -9343))) || ((jugador.equipo=="r") && ((jugador.flags.distanciaPorteriaDer != -9343)||(jugador.flags.distanciaCornerDer1!= -9343)||(jugador.flags.distanciaCornerDer2!= -9343))))
            socket.sendTo(golpearBalon("30", "180"), address); 
        else if (((jugador.equipo=="l") && (jugador.flags.distanciaPorteriaDer != -9343)) || ((jugador.equipo=="r") && (jugador.flags.distanciaPorteriaIzq != -9343)))
            socket.sendTo(golpearBalon("10", "0"), address);
        else if (((jugador.equipo=="l") && (jugador.flags.distanciaCornerDer1 != -9343)) || ((jugador.equipo=="r") && (jugador.flags.distanciaCornerIzq1 != -9343)))
            socket.sendTo(golpearBalon("10", "-20"), address);
        else if (((jugador.equipo=="l") && (jugador.flags.distanciaCornerDer2 != -9343)) || ((jugador.equipo=="r") && (jugador.flags.distanciaCornerIzq2 != -9343)))
            socket.sendTo(golpearBalon("10", "20"), address);
    } // si estamos cerca del balon y hay un enemigo cerca hacemos tackle
    else if (jugador.flags.distanciaBalon <= 0.6 && distEnemCerca <= 0.6 && distEnemCerca != -1){
            cout << "Tackleo" << endl;
        socket.sendTo(("(tackle " + to_string(oriEnemCerca) + ")"), address);
    }
    else if(jugador.flags.distanciaBalon > 0.6){
        //si estamos muy cerca del centro del campo volvemos a la porteria
        //si hay alguien mas cerca que yo del balon le acompaño aunque lento por si acaso
        if(distCompCerca < jugador.flags.distanciaBalon && jugador.flags.distanciaBalon  != -9343 && distCompCerca != -1){
            if((jugador.flags.distanciaBalon>0.6) && (((jugador.equipo=="l" && jugador.flags.distanciaPorteriaIzq != -9343) || (jugador.equipo=="r" && jugador.flags.distanciaPorteriaDer != -9343)) || ((jugador.equipo=="l" && jugador.flags.distanciaCornerIzq1!= -9343)||(jugador.equipo=="r" && jugador.flags.distanciaCornerDer1 != -9343)) || ((jugador.equipo=="l" && jugador.flags.distanciaCornerIzq2!= -9343)||(jugador.equipo=="r" && jugador.flags.distanciaCornerDer2 != -9343)))){
                if (((jugador.equipo=="l") && (jugador.flags.distanciaPorteriaIzq > 47)) || ((jugador.equipo=="r") && (jugador.flags.distanciaPorteriaDer > 47)) )
                    socket.sendTo("(dash 15)", address);
            }
            else
                socket.sendTo("(dash 20)", address);
        }
        else{
            socket.sendTo("(dash 80)", address);
        }
    }

}

void accionesRepetidas(Jugador & jugador, MinimalSocket::udp::Udp<true> & socket, MinimalSocket::Address const & address){
    
    float distEnemCerca = -1;
    float oriEnemCerca = -1;
    float distCompCerca = -1;
    float oriCompCerca = -1;
    float compañeroMasLejano = -1;

    float auxDistPorteriaContraria = -9343;
    float auxOriPorteriaContraria = -9343;
     

    if(jugador.equipo == "l"){
        auxDistPorteriaContraria = jugador.flags.distanciaPorteriaDer;
        auxOriPorteriaContraria = jugador.flags.orientacionPorteriaDer;
    }else{
        auxDistPorteriaContraria = jugador.flags.distanciaPorteriaIzq;
        auxOriPorteriaContraria = jugador.flags.orientacionPorteriaIzq;
    }
        

    /*

        Estos dos if sacan la distancia mas cercana de un enemigo y de un compañero
        La del enemigo para hacer tackle y la del compañero para pasarle el balon si hay

    */

    if(jugador.flags.enemigosCerca.size() > 0){
    float minEnemigo = jugador.flags.enemigosCerca[0].first;
    float initOriEnem = jugador.flags.enemigosCerca[0].second;
        for (auto enemigo : jugador.flags.enemigosCerca){
            minEnemigo = min(minEnemigo, enemigo.first);
            initOriEnem = enemigo.second;
        }
        distEnemCerca = minEnemigo;
        oriEnemCerca = initOriEnem;

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

    if(jugador.flags.flagsPorteria.size() > 0){
            float intiFLagFondoMasCercana = jugador.flags.flagsPorteria[0];
            for (auto flagFondo : jugador.flags.flagsPorteria){
            }
        }

    /*
    
        Ahora sabiendo esto podemos empezar a pensar con la distancia del balon y las flags
        Además toda esta parte podria ser una funcion que compartan todos que sea algo como
        funcion defensiva
    
    */

    /*Falta una funcion que nos diga a que lado estamos mirando, si vemos las flags de nuestro campo entonces tenemos que hacer 180 en todo*/
    /* Aqui nos orientamos  */
    if((jugador.flags.orientacionBalon > 10 ))
        socket.sendTo(orientarJugador(to_string(jugador.flags.orientacionBalon)), address);
    else if(jugador.flags.orientacionBalon < -10)
        socket.sendTo(orientarJugador(to_string(jugador.flags.orientacionBalon)), address);



    /* Aqui la decision depende de la posicion del balon */
    if(jugador.flags.distanciaBalon <= 0.6 && auxDistPorteriaContraria <= 25 && auxDistPorteriaContraria != -9343){
        cout << "Chuto a puerta" << endl;
        cout << "Distancia porteria: " << auxDistPorteriaContraria << endl;

        socket.sendTo(golpearBalon("60", to_string(auxOriPorteriaContraria + 3)), address);        
    }// si tenemos el balon cerca pasamos a un compañero
     else if(jugador.flags.distanciaBalon <= 0.6 && auxDistPorteriaContraria > 25 && (distCompCerca > 10 || distCompCerca < 30 )
      && distCompCerca != -1 && auxDistPorteriaContraria != -9343){
        socket.sendTo(golpearBalon(to_string(distCompCerca), to_string(oriCompCerca)), address);
    } //si estamos lejos de la porteria, tenemos el balon cerca y no hay buen pase corremos con el balon
    else if(jugador.flags.distanciaBalon <= 0.6 && auxDistPorteriaContraria > 25 && (distCompCerca < 10 || distCompCerca > 30 )){
        socket.sendTo(golpearBalon("10", to_string(auxOriPorteriaContraria)), address);
    } //si no veo la porteria y tengo que moverme, p
     else if(jugador.flags.distanciaBalon <= 0.6 && auxDistPorteriaContraria == -9343 && (distCompCerca < 10 || distCompCerca > 30 )){
        socket.sendTo(golpearBalon("10", "0"), address); 
    } // si estamos cerca del balon y hay un enemigo cerca hacemos tackle
    else if (jugador.flags.distanciaBalon <= 0.6 && distEnemCerca <= 0.6 && distEnemCerca != -1){
            cout << "Tackleo" << endl;
        socket.sendTo(("(tackle " + to_string(oriEnemCerca) + ")"), address);
    }
    else if(jugador.flags.distanciaBalon > 0.6){
        //si estamos muy cerca del centro del campo volvemos a la porteria
        //si hay alguien mas cerca que yo del balon le acompaño aunque lento por si acaso
        if(distCompCerca < jugador.flags.distanciaBalon && jugador.flags.distanciaBalon  != -9343 && distCompCerca != -1){
            socket.sendTo("(dash 20)", address);
        }
        else{
            socket.sendTo("(dash 80)", address);
        }
    }
}

