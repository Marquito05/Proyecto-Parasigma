// =============================================================================================
//  PARTE 2: conteo de palabras e índice invertido (ver PARTE2.md)
//  La Parte 1 (main.cpp) ya llama a estas dos funciones; la Parte 2 las implementa en src/indice.cpp
// =============================================================================================
#ifndef INDICE_HPP
#define INDICE_HPP

#include "libros.hpp"

#include <map>
#include <semaphore.h>
#include <string>

// Índice invertido compartido por todos los threads
struct IndiceInvertido {
    // palabra -> (ID del libro -> cantidad de veces que aparece la palabra en ese libro)
    // Ejemplo:  "computador" -> { 1: 15, 3: 8, 7: 21 }
    std::map<std::string, std::map<int, int>> palabras;

    // Semáforo que protege 'palabras' (main lo crea con valor 1)
    sem_t semaforo;
};

// Cuenta las palabras de un libro y las agrega al índice. Retorna false si no se pudo procesar.
// Se llama desde varios threads al mismo tiempo, cada uno con un libro distinto.
bool procesarLibro(const Libro &libro, IndiceInvertido &indice);

// Escribe el índice en el archivo, una palabra por línea:  computador;(1,15);(3,8);(7,21)
// Se llama una sola vez, cuando todos los threads ya terminaron. Retorna false si no se pudo escribir.
bool escribirIndice(IndiceInvertido &indice, const std::string &archivo);

#endif
