#include "../../../include/utils.hpp"
#include "../include/servidor.hpp"
#include "../include/cliente.hpp"

#include <iostream>

static void mostrarMenu() {
    std::cout << "-----------------------------\n"
              << "           JUEGO\n"
              << "-----------------------------\n"
              << "1. Crear partida\n"
              << "2. Unirse a una partida\n"
              << "0. Regresar\n"
              << "Ingresar opcion: ";
}



int main(int argc, char* argv[]) {

    std::string playerName = argv[1];

    int opt=-1;
    while (opt != 0) {
        mostrarMenu();
        std::cin >> std::noskipws >> opt;
        while (!std::cin || opt <= -1 || opt > 3) {
            std::cout << "\nERROR! Ingresar valor numérico valido.";
            mostrarMenu();
            sanitizeStream();
            std::cin >> std::noskipws >> opt;
        }

        sanitizeStream();
        if (opt == 1) {
            crearServidor(playerName); // ejecuta ./bin/user_admin con system()
        }

        if (opt == 2) {
            ingresarCliente(playerName);
        }
    }
    return 0;
}
