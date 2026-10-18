#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>

// Configuración del programa, leída de las variables de entorno del archivo .env
struct ConfigIndice {
    std::string carpetaLibros; // LIBROS_DIR
    std::string archivoMapa;   // MAPA_LIBROS_FILE
    std::string archivoIndice; // INDICE_FILE
    int nThreads;              // N_THREADS
};

// Lee las variables del .env y revisa que sean válidas. Retorna false si hay algún error.
bool cargarConfiguracion(ConfigIndice &config);

#endif
