#ifndef ARGS_HPP
#define ARGS_HPP

#include <string>

// Argumentos de ejecución: -u (usuario), -p (password), -f (archivo)
struct Argumentos {
    std::string usuario;
    std::string password;
    std::string archivo;
};

// Declaraciones de funciones
void mostrarUso(const std::string &programa);
bool leerArgumentos(int argc, char* argv[], Argumentos &a);

#endif