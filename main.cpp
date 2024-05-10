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
    TicToc clock;
    
    Jugador jugador;
    Flags flags;


    clock.tic();

    if (argc != 4)
    {
        throw std::invalid_argument("Uso: <nombre_equipo> <tipo_jugador> <puerto>");
    }

    string puerto = argv[3];
    cout << "Puerto: " << puerto << endl;
    MinimalSocket::Port this_socket_port = stoi(puerto);
    cout << "Creating a UDP socket" << endl;
    MinimalSocket::udp::Udp<true> udp_socket(this_socket_port, MinimalSocket::AddressFamily::IP_V6);
    cout << "Socket created" << endl;

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
        udp_socket.sendTo("(init "+nombre_equipo+"(version 19) (goalie))", other_recipient_udp);
    }else{
        udp_socket.sendTo("(init "+nombre_equipo+"(version 19))", other_recipient_udp);
    }

    cout << "Message sent" << endl;

    std::size_t message_max_size = 5000;
    cout << "Waiting for a message" << endl;
    auto received_message = udp_socket.receive(message_max_size);
    // check the sender address
    MinimalSocket::Address other_sender_udp = received_message->sender;

    std::string received_message_content = received_message->received_message;

    MinimalSocket::Address server_udp = MinimalSocket::Address{"127.0.0.1", other_sender_udp.getPort()};
    cout << "Received message: " << received_message_content << endl;
    iniciarJugador(received_message->received_message, jugador);

    
    colocarJugadorSegunNumero(jugador, udp_socket, server_udp);   

    if(jugador.equipo == "r"){ 
        this_thread::sleep_for(chrono::milliseconds(1));
        girarEquipoVisitante(udp_socket, server_udp);
    }


    cout << "Esperando a que empiece el partido" << endl;
    clock.tic();
    while(1){
        
    do{
    received_message = udp_socket.receive(message_max_size);
    received_message_content = received_message->received_message;

    jugador.distanciaAlBalon=50;
    jugador.orientacionAlBalon=50;

    try
    {
        parseSeverMessage(received_message_content, jugador, flags);
        
    }
    catch (const std::exception &e)
    {
        cout << e.what() << endl;
    }
    } while (received_message_content.find("(see") == -1);
        
    if(jugador.estadoPartido.enJuego ){
        decidirComando(jugador, udp_socket, server_udp);
    }  

    
    }
}