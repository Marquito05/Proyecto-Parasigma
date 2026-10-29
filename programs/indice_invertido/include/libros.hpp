#ifndef LIBROS_HPP
#define LIBROS_HPP

#include <string>
#include <vector>

// Un libro pendiente de procesar (es lo que se guarda en la cola de trabajo)
struct Libro {
    int id;             // identificador numérico único: 1, 2, 3, ...
    std::string nombre; // nombre del archivo, por ejemplo "don_quijote.txt"
    std::string ruta;   // ruta para abrir el archivo
};

// Revisa la carpeta de libros, busca los archivos .txt y les asigna un ID.
// Retorna false si hay algún problema que impida comenzar el procesamiento.
bool buscarLibros(const std::string &carpeta, std::vector<Libro> &libros);

// Guarda la asociación ID - libro en el archivo, una línea por libro:  1;don_quijote.txt
bool guardarMapaLibros(const std::vector<Libro> &libros, const std::string &archivo);

#endif
