#include "../include/libros.hpp"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>

bool buscarLibros(const std::string &carpeta, std::vector<Libro> &libros) {
    std::vector<std::string> rutas;

    try {
        // 1. La carpeta debe existir
        if (!std::filesystem::is_directory(carpeta)) {
            std::cout << "ERROR! No existe la carpeta de libros '" << carpeta << "' (revise LIBROS_DIR en el .env)\n";
            return false;
        }

        // 2. Buscar los archivos .txt de la carpeta (y de sus subcarpetas).
        //    Si la carpeta no se puede leer, se produce un error que se atrapa en el catch.
        for (const std::filesystem::directory_entry &archivo : std::filesystem::recursive_directory_iterator(carpeta)) {
            if (archivo.is_regular_file() && archivo.path().extension() == ".txt") {
                rutas.push_back(archivo.path().string());
            }
        }
    } catch (...) {
        std::cout << "ERROR! No se pudo leer la carpeta de libros '" << carpeta << "' (revise los permisos)\n";
        return false;
    }

    if (rutas.empty()) {
        std::cout << "ERROR! No hay libros (.txt) en la carpeta '" << carpeta << "'\n";
        return false;
    }

    // 3. Ordenar por nombre y asignar los IDs 1, 2, 3, ... (así los IDs no cambian entre ejecuciones)
    std::sort(rutas.begin(), rutas.end());
    for (size_t i = 0; i < rutas.size(); i++) {
        Libro libro;
        libro.id = (int) i + 1;
        libro.ruta = rutas[i];
        libro.nombre = std::filesystem::path(rutas[i]).filename().string();
        libros.push_back(libro);
    }

    std::cout << "Libros encontrados en '" << carpeta << "': " << libros.size() << "\n";
    return true;
}

bool guardarMapaLibros(const std::vector<Libro> &libros, const std::string &archivo) {
    std::ofstream salida(archivo);
    if (!salida.is_open()) {
        std::cout << "ERROR! No se pudo crear el archivo " << archivo << "\n";
        return false;
    }

    for (size_t i = 0; i < libros.size(); i++) {
        salida << libros[i].id << ";" << libros[i].nombre << "\n";
    }
    salida.close();

    std::cout << "Asociación ID-libro guardada en " << archivo << "\n";
    return true;
}
