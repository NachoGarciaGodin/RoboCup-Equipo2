#include <iostream>

using namespace std;

#include <MinimalSocket/udp/UdpSocket.h>

int main()
{

    MinimalSocket::Port this_socket_port = 5555;

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
    udp_socket.sendTo("(move -10 10)", server_udp);
    udp_socket.sendTo("(say (hola))", server_udp);
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