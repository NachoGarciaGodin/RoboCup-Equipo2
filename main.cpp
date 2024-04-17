#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <memory>
#include <functional>
#include <cstdlib>



using namespace std;

#include <MinimalSocket/udp/UdpSocket.h>

vector<string> quitarParentesis(const string& cadena){
    vector<string> palabras;
    string palabra_actual; // (hola)(como)(estas)
    int parentesis=0,posicion_inicial,i=0;
    for (char caracter : cadena) {
        if (caracter == '(') {
            parentesis++;
            if(parentesis==1)
                posicion_inicial=i+1;
        }else if(caracter==')'){
            parentesis--;
            if(parentesis == 0){
                palabra_actual=cadena.substr(posicion_inicial,i-posicion_inicial);
                if (!palabra_actual.empty()) {
                    palabras.push_back(palabra_actual);
                    palabra_actual.clear();
                }
            }
        }
        i++;
    }
    if (!palabra_actual.empty()) {
        palabras.push_back(palabra_actual);
    }
    return palabras;
}

int leerNumeroJugador(string mensajeRecibido){
    auto mensaje=quitarParentesis(mensajeRecibido).at(0);
    string mensaje_aux;
    int numero_jugador=0,pos;
    pos=mensaje.find(' ',7);
    mensaje_aux=mensaje.substr(7,pos-7);
    numero_jugador=stoi(mensaje_aux);
    return numero_jugador;
}


int main()
{
    srand(time(NULL));
    MinimalSocket::Port this_socket_port = rand() % (10000-5000+1) + 5000;

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
    udp_socket.sendTo("(init NottinghamMiedo(version 19))", other_recipient_udp);
    cout << "Message sent" << endl;
    
    //calcular posicion en funcion de argv ??
    
    cout << "Message sent" << endl;
    std::size_t message_max_size = 1000;
    cout << "Waiting for a message" << endl;
    auto received_message = udp_socket.receive(message_max_size);
    // check the sender address
    MinimalSocket::Address other_sender_udp = received_message->sender;
    cout << "puerto" <<received_message->sender.getPort() << endl;
    MinimalSocket::Address server_udp = MinimalSocket::Address{"127.0.0.1",received_message->sender.getPort() };
    auto numero_jugador=leerNumeroJugador(received_message->received_message);
    cout << "Numero jugador: " << numero_jugador << endl;
    switch(numero_jugador){
        case 1:
            udp_socket.sendTo("(move -51 0)", server_udp); 
            break;
        case 2:
            udp_socket.sendTo("(move -30 -30)", server_udp);
            break;
        case 3:
            udp_socket.sendTo("(move -35 -10)", server_udp);
            break;
        case 4:
            udp_socket.sendTo("(move -35 10)", server_udp);
            break;
        case 5:
            udp_socket.sendTo("(move -30 30)", server_udp);
            break;
        case 6:
            udp_socket.sendTo("(move -25 -10)", server_udp);
            break;
        case 7:
            udp_socket.sendTo("(move -25 10)", server_udp);
            break;
        case 8:
            udp_socket.sendTo("(move -11 0)", server_udp);
            break;
        case 9:
            udp_socket.sendTo("(move -0.5 -27)", server_udp);
            break;
        case 10:
            udp_socket.sendTo("(move -0.5 27)", server_udp);
            break;
        case 11:
            udp_socket.sendTo("(move -0.75 10)", server_udp);
            break;
        default:
            udp_socket.sendTo("(move 0 0)", server_udp);
            break;
    }

    // access the received message
    // resized to the nunber of bytes
    // actually received
    std::string received_message_content = received_message->received_message;
    cout << received_message_content << endl; 

    while(1){
       // receive a message from another udp reaching this one
    std::size_t message_max_size = 1000;
    
    auto received_message = udp_socket.receive(message_max_size);
    // check the sender address
    MinimalSocket::Address other_sender_udp = received_message->sender;
    
    // access the received message
    // resized to the nunber of bytes
    // actually received
    std::string received_message_content = received_message->received_message;
      
    }


}