#include "../include/conteo.hpp"

#include <iostream>

// Programa de conteo sobre texto (lo llama el menu principal con system()).
//   ./bin/conteo <archivo>   -> opcion 6: cuenta el archivo recibido con -f
//   ./bin/conteo             -> opcion 7: pide la ruta del archivo a contar
int main(int argc, char* argv[]) {
    if (argc == 2) {
        opcionConteoTexto(argv[1]);
    } else if (argc == 1) {
        opcionConteoArchivo();
    } else {
        return 1;
    }
    return 0;
}