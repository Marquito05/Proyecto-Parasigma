#include "../include/indice.hpp"

#include <cctype>
#include <fstream>
#include <iostream>
#include <map>
#include <string>

bool procesarLibro(const Libro &libro, IndiceInvertido &indice) {
    // 1. Abrir el archivo en modo binario
    std::ifstream archivo(libro.ruta, std::ios::binary);
    if (!archivo.is_open()) {
        return false;
    }

    // 2. Conteo de palabras en un mapa local del thread (sin semáforo para no frenar el paralelismo)
    std::map<std::string, int> conteo;
    std::string palabraActual;

    auto guardarPalabra = [&]() {
        if (!palabraActual.empty()) {
            conteo[palabraActual]++;
            palabraActual.clear();
        }
    };

    char cRaw;
    while (archivo.get(cRaw)) {
        unsigned char c = static_cast<unsigned char>(cRaw);

        if (c == 0xC3) {
            // Posible caracter acentuado, diéresis o eñe en UTF-8 (2 bytes)
            char nextRaw;
            if (archivo.get(nextRaw)) {
                unsigned char b2 = static_cast<unsigned char>(nextRaw);
                // Rango 0x80 a 0xBF (excluyendo 0x97 que es '×' y 0xB7 que es '÷')
                if (b2 >= 0x80 && b2 <= 0xBF && b2 != 0x97 && b2 != 0xB7) {
                    // Si está en mayúscula (0x80 a 0x9E), sumamos 0x20 para pasar a minúscula
                    if (b2 >= 0x80 && b2 <= 0x9E) {
                        b2 = static_cast<unsigned char>(b2 + 0x20);
                    }
                    palabraActual.push_back(static_cast<char>(0xC3));
                    palabraActual.push_back(static_cast<char>(b2));
                } else {
                    guardarPalabra();
                    archivo.unget();
                }
            } else {
                guardarPalabra();
            }
        } else if (std::isalnum(c)) {
            // Letras a-z, A-Z o dígitos 0-9
            palabraActual.push_back(static_cast<char>(std::tolower(c)));
        } else {
            // Espacios, signos de puntuación, delimitadores
            guardarPalabra();
        }
    }
    // Guardar última palabra si no terminó con delimitador
    guardarPalabra();

    // 3. Volcar resultados al índice compartido dentro de la sección crítica
    sem_wait(&indice.semaforo);
    for (const auto &par : conteo) {
        indice.palabras[par.first][libro.id] = par.second;
    }
    sem_post(&indice.semaforo);

    return true;
}

bool escribirIndice(IndiceInvertido &indice, const std::string &archivo) {
    // 1. Abrir archivo de salida (ruta configurada en el .env)
    std::ofstream salida(archivo);
    if (!salida.is_open()) {
        std::cerr << "ERROR: no se pudo crear el archivo de índice '" << archivo << "'\n";
        return false;
    }

    // 2. Escribir formato: palabra;(id1,cant1);(id2,cant2);...
    // Como palabras y listas de libros son std::map, ya salen ordenadas alfabéticamente y por ID.
    for (const auto &parPalabra : indice.palabras) {
        salida << parPalabra.first;
        for (const auto &parLibro : parPalabra.second) {
            salida << ";(" << parLibro.first << ',' << parLibro.second << ')';
        }
        salida << '\n';
    }

    return true;
}
