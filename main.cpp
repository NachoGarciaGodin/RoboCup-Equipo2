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


    colocarJugadorSegunNumero(jugador,udp_socket, server_udp);   

    received_message = udp_socket.receive(message_max_size);
    
    if(jugador.equipo == "r"){ 
        std::this_thread::sleep_for(std::chrono::milliseconds(150));
        girarEquipoVisitante(udp_socket, server_udp);
    }

    TicToc clock;
    clock.tic();

    while (true){

    do{
        received_message = udp_socket.receive(message_max_size);
        received_message_content = received_message->received_message;
    
        try
        {
            parseSeverMessage(received_message_content, jugador);
        }
        catch (const std::exception &e)
        {
            cout << e.what() << endl;
        }
    } while (received_message_content.find("(see") == -1);

    
    if(jugador.estadoPartido.enJuego ){
        decidirComando(jugador, udp_socket, server_udp);
        limpiarJugador(jugador);
    }  
    
    }
}