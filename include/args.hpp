#ifndef ARGS_HPP
#define ARGS_HPP

#include <string>

// Argumentos de ejecución: -u (usuario), -p (password), -f (archivo)
struct Argumentos;

// Declaraciones de funciones
void mostrarUso(const std::string &programa);
bool leerArgumentos(int argc, char* argv[], Argumentos &a);

#endif