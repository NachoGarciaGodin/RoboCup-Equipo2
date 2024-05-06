#include <iostream>
#include <vector>
#include <algorithm>
#include <memory>
#include <functional>


#include <chrono>
#include <thread>

#include "funcionalidad.h"
#include "envioPaquetes.h"
#include "jugador.h"
#include "tictoc.h"

#include <MinimalSocket/udp/UdpSocket.h>

using namespace std;

int main(int argc, char *argv[] )
{
    Jugador jugador;
    string quienSaca;
    bool kickOff=0;

    if (argc != 3) {
        cout << "Falta indicar si es goalie" << endl;
        return 1;
    }
    srand(time(NULL));
    MinimalSocket::Port this_socket_port = rand() % (7001 - 6000) + 6000;

    //cout << "Creating a UDP socket" << endl;

    MinimalSocket::udp::Udp<true> udp_socket(this_socket_port, MinimalSocket::AddressFamily::IP_V6);

    //cout << "Socket created" << endl;

    bool success = udp_socket.open();

    if (!success)
    {
        cout << "Error opening socket" << endl;
        return 1;
    }

    // send a message to another udp
    MinimalSocket::Address other_recipient_udp = MinimalSocket::Address{"127.0.0.1", 6000};
    string nombre_equipo=argv[1];
    if(argv[2]=="goalie"){
        udp_socket.sendTo("(init "+nombre_equipo+"(version 19)(goalie))", other_recipient_udp);
    }else{
        udp_socket.sendTo("(init "+nombre_equipo+"(version 19))", other_recipient_udp);
    }
        
    std::size_t message_max_size = 1000;
    //cout << "Waiting for a message" << endl;
    auto received_message = udp_socket.receive(message_max_size);
    // check the sender address
    MinimalSocket::Address other_sender_udp = received_message->sender;
    MinimalSocket::Address server_udp = MinimalSocket::Address{"127.0.0.1", other_sender_udp.getPort()};

    iniciarJugador(received_message->received_message, jugador);

    

    std::string received_message_content = received_message->received_message;

    string auxiliar= "l";
    colocarJugadorSegunNumero(jugador, auxiliar,udp_socket, server_udp);   

    received_message = udp_socket.receive(message_max_size);
    
    if(jugador.equipo == "r" && kickOff==0){ //
        std::this_thread::sleep_for(std::chrono::milliseconds(150));
        girarEquipoVisitante(udp_socket, server_udp);
    }

    TicToc clock;
    clock.tic();

    while(1){

    std::this_thread::sleep_for(std::chrono::milliseconds(1));

    // receive a message from another udp reaching this one
    std::size_t message_max_size = 1000;
   
    auto received_message = udp_socket.receive(message_max_size);
    MinimalSocket::Address other_sender_udp = received_message->sender;
    
    std::string received_message_content = received_message->received_message;

    int posHear=0;
    posHear=received_message_content.find("hear",0);
    if (posHear != -1){
        kickOff=comprobarKickOff(received_message_content, quienSaca,jugador, udp_socket, server_udp);
    }
    
    if((received_message_content.find("(see)")==-1) && (clock.toc()>1000)){
        jugador.distanciaAlBalon=50;
        jugador.orientacionAlBalon=50;
        clock.tic();
    }
    
    if(kickOff == 1){
        parseSeverMessage(received_message_content, jugador);
        decidirComando(jugador, udp_socket, server_udp);
    }  

      
    
}
}