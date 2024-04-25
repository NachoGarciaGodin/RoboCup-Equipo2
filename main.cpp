#include <iostream>
#include <vector>
#include <algorithm>
#include <memory>
#include <functional>


#include <chrono>
#include <thread>

#include "funcionalidad.h"
#include "jugador.h"
#include "tictoc.h"

#include <MinimalSocket/udp/UdpSocket.h>

using namespace std;

int main(int argc, char *argv[] )
{
    Jugador jugador;
    float distanciaAlBalon = 10;
    float orientacionAlBalon = 70;
    string quienSaca;
    bool kickOff=0;

    if (argc != 3) {
        cout << "Falta indicar si es goalie" << endl;
        return 1;
    }
    srand(time(NULL));
    MinimalSocket::Port this_socket_port = rand() % (10000-5000+1) + 5000;

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
        cout << "Message sent" << endl;
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
    //cout << received_message_content << endl; 
    colocarJugadorSegunNumero(jugador, udp_socket, server_udp);   

    received_message = udp_socket.receive(message_max_size);
    
    if(jugador.equipo == "r" && kickOff==0){
        std::this_thread::sleep_for(std::chrono::milliseconds(130));
        girarEquipoVisitante(udp_socket, server_udp);
    }
    TicToc clock;
    clock.tic();
    while(1){
   
    while (clock.toc() < 100){
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    clock.tic();
    


    // receive a message from another udp reaching this one
    std::size_t message_max_size = 1000;
   
    auto received_message = udp_socket.receive(message_max_size);
    MinimalSocket::Address other_sender_udp = received_message->sender;
    
    std::string received_message_content = received_message->received_message;
    //cout << received_message_content << endl;

   

    int posSee=0;
    posSee=received_message_content.find("see",0);
    if (posSee != -1){
        auto posBall= received_message_content.find("(b)",posSee);
        if (posBall != -1){
            auto aux=received_message_content.substr(posBall+4,8); 
            distanciaAlBalon = distanciaBalon(aux);
            orientacionAlBalon = orientacionBalon(aux);
        }
    }

    int posHear=0;
    posHear=received_message_content.find("hear",0);
    if (posHear != -1){
        auto posKickOff= received_message_content.find("kick_off",posHear);
        if (posKickOff != -1){
            quienSaca=received_message_content.substr(posKickOff+9,1); 
            kickOff=1;
        }
    }
    if(kickOff==1)
        decidirComando(jugador, distanciaAlBalon, orientacionAlBalon, udp_socket, server_udp);
    }
}