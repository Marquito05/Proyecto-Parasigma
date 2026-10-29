#include "../include/config.hpp"
#include "../../../include/env_config.hpp" // leerVariableEnv(): la misma función que usa el menú principal
#include "../../../include/utils.hpp"      // isdigit_string()

#include <filesystem>
#include <iostream>
#include <string>

// Cantidad máxima de threads permitida, para no agotar los recursos del sistema
const int MAX_THREADS = 256;

// Lee una variable del .env. Si no existe, muestra un error y deja 'ok' en false.
static std::string leerVariable(const std::string &nombre, bool &ok) {
    std::string valor = leerVariableEnv(nombre);
    if (valor.empty()) {
        std::cout << "ERROR! Falta la variable " << nombre << " en el archivo .env\n";
        ok = false;
    }
    return valor;
}

// Revisa que exista la carpeta donde se va a guardar un archivo
static bool carpetaExiste(const std::string &archivo) {
    std::string carpeta = std::filesystem::path(archivo).parent_path().string();
    if (carpeta == "" || std::filesystem::is_directory(carpeta)) return true;

    std::cout << "ERROR! No existe la carpeta '" << carpeta << "' del archivo " << archivo << "\n";
    return false;
}

bool cargarConfiguracion(ConfigIndice &config) {
    // 1. Las cuatro variables deben estar en el .env
    bool ok = true;
    config.carpetaLibros = leerVariable("LIBROS_DIR", ok);
    config.archivoMapa = leerVariable("MAPA_LIBROS_FILE", ok);
    config.archivoIndice = leerVariable("INDICE_FILE", ok);
    std::string threads = leerVariable("N_THREADS", ok);
    if (!ok) return false;

    // 2. N_THREADS debe ser un número entero entre 1 y MAX_THREADS.
    //    Primero se revisa que tenga solo dígitos (máximo 3) y después se convierte a número.
    bool valido = isdigit_string(threads) && threads.size() <= 3;
    if (valido) {
        config.nThreads = std::stoi(threads);
        valido = config.nThreads >= 1 && config.nThreads <= MAX_THREADS;
    }
    if (!valido) {
        std::cout << "ERROR! N_THREADS debe ser un número entero entre 1 y " << MAX_THREADS << " (valor actual: "
                  << threads << ")\n";
        return false;
    }

    // 3. Deben existir las carpetas donde se van a guardar el mapa de libros y el índice
    if (!carpetaExiste(config.archivoMapa) || !carpetaExiste(config.archivoIndice)) return false;

    return true;
}
