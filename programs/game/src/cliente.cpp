#include <arpa/inet.h>  //AF_INET, SOCK_STREAM, etc
#include <cerrno>       // errno
#include <unistd.h>     // close
#include <cstring>      // strerror

#include "../../../include/utils.hpp"
#include <iostream>


void cliente(const std::string &IP, const int &port, const std::string &playerName) {
    std::cout << "going into client\n";
    std::cout << "IP: " << IP << '\n';
    std::cout << "puerto: " << port << '\n';

    const int puerto = port == 0 ? 9090 : port;

    const int cliente = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in direccion{};
    direccion.sin_family = AF_INET;
    direccion.sin_port = htons(static_cast<uint16_t>(puerto));

    if (cliente == -1 
        || inet_pton(AF_INET, IP.c_str(), &direccion.sin_addr) != 1 
        || connect(cliente, reinterpret_cast<sockaddr*>(&direccion), sizeof(direccion)) == -1) {
        std::cerr << "Error al conectar: " << strerror(errno) << '\n';
        if (cliente != -1) close(cliente);
        return;
    }

    send(cliente, playerName.c_str(), playerName.length(), MSG_NOSIGNAL);

    char respuesta[256];
    std::string recvStream, cmd;
    size_t pos;
    ssize_t n;
    bool getClientID = true;

    while (true) {
        n = recv(cliente, respuesta, sizeof(respuesta) - 1, 0);
        if (n <= 0) {
            std::cout << "[CLIENT] Lost connection to the server, disconnecting...\n";
            close(cliente);
            break;
        }

        respuesta[n] = '\0';
        recvStream += respuesta;

        while ((pos = recvStream.find('\n')) != std::string::npos) {
            cmd = recvStream.substr(0, pos);
            recvStream.erase(0, pos + 1);

            if (cmd.empty()) {
                std::cout << '\n';
            }
            else if (getClientID) {
                std::cout << "You're player " << cmd << '\n';
                getClientID = false;
            }
            else if (cmd == "YOUR_TURN") {
                std::cout << "¡Es tu turno! Presiona ENTER para lanzar el dado...\n";
                std::cin.get();
                send(cliente, "PLAY\n", 5, MSG_NOSIGNAL);
            }
            else if (cmd == "WAIT_TURN") {
                std::cout << "Esperando turno...\n";
            }
            else {
                std::cout << "[SERVER]: " << cmd << '\n';
            }
        }
    }
}

void ingresarCliente(const std::string &playerName) {
    std::string IP;
    int port;

    std::cout << "Ingrese la IP: ";
    getline(std::cin, IP);
    IP = get_trimmed_string(IP);
    while (IP.empty()) {
        std::cout << "ERROR! Ingrese caracteres válidos: ";
        getline(std::cin, IP);
        IP = get_trimmed_string(IP);
    }

    std::cout << "Ingrese el puerto ('0' para puerto predeterminado): ";
    std::cin >> std::noskipws >> port; sanitizeStream();
    while (!std::cin || port < 0 ) {
        std::cout << "\nPuerto no válido!" 
                  << "\nIngrese el puerto ('0' para puerto predeterminado): ";
        std::cin >> std::noskipws >> port; sanitizeStream();
    }

    cliente(IP, port, playerName);
}
