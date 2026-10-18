#include "../include/config.hpp"
#include "../include/indice.hpp"
#include "../include/libros.hpp"

#include <iostream>
#include <queue>
#include <semaphore.h>
#include <thread>
#include <vector>

// Programa que crea el índice invertido de los libros usando varios threads (Entrega 3, problema 1).
// Lo llama el menú principal con system() (opción 8). Su configuración se lee del archivo .env.

// ---------------- Datos compartidos por todos los threads ----------------
std::queue<Libro> cola;    // cola de trabajo: libros que faltan por procesar
sem_t semCola;             // protege la cola
IndiceInvertido indice;    // índice invertido (lo llena la PARTE 2; tiene su propio semáforo)
int librosProcesados = 0;  // cantidad de libros procesados sin error
sem_t semPantalla;         // protege librosProcesados y la pantalla

// Saca el primer libro de la cola. Retorna false si la cola está vacía (ya no queda trabajo).
bool sacarLibro(Libro &libro) {
    sem_wait(&semCola); // inicio de la sección crítica
    if (cola.empty()) {
        sem_post(&semCola); // fin de la sección crítica
        return false;
    }
    libro = cola.front();
    cola.pop();
    sem_post(&semCola); // fin de la sección crítica
    return true;
}

// Función que ejecuta cada thread: saca libros de la cola y los procesa hasta que la cola queda vacía
void hiloTrabajador(int numero) {
    Libro libro;
    while (sacarLibro(libro)) {
        bool ok = procesarLibro(libro, indice); // PARTE 2: contar las palabras y agregarlas al índice

        sem_wait(&semPantalla); // inicio de la sección crítica
        if (ok) {
            librosProcesados++;
            std::cout << "[Thread " << numero << "] libro " << libro.id << " listo (" << libro.nombre << ")\n";
        } else {
            std::cout << "[Thread " << numero << "] ERROR: no se pudo procesar el libro " << libro.id << " ("
                      << libro.nombre << ")\n";
        }
        sem_post(&semPantalla); // fin de la sección crítica
    }
}

int main() {
    std::cout << "\n===== CREAR INDICE INVERTIDO =====\n";

    // 1. Leer y validar las variables de entorno del .env
    ConfigIndice config;
    if (!cargarConfiguracion(config)) return 1;

    // 2. Revisar la carpeta de libros e identificar los libros que se van a procesar
    std::vector<Libro> libros;
    if (!buscarLibros(config.carpetaLibros, libros)) return 1;

    // 3. Guardar la asociación ID - libro
    if (!guardarMapaLibros(libros, config.archivoMapa)) return 1;

    // 4. Poner todos los libros en la cola de trabajo.
    //    Todavía no hay threads, por eso aquí no se necesita semáforo.
    for (size_t i = 0; i < libros.size(); i++) {
        cola.push(libros[i]);
    }

    // 5. Crear los semáforos con valor 1: solo un thread a la vez puede estar en cada sección crítica
    sem_init(&semCola, 0, 1);
    sem_init(&semPantalla, 0, 1);
    sem_init(&indice.semaforo, 0, 1);

    // 6. Crear los N threads trabajadores
    std::cout << "Procesando " << libros.size() << " libros con " << config.nThreads << " threads...\n";
    std::vector<std::thread> threads;
    for (int i = 1; i <= config.nThreads; i++) {
        threads.push_back(std::thread(hiloTrabajador, i));
    }

    // 7. Esperar a que todos los threads terminen (join)
    for (size_t i = 0; i < threads.size(); i++) {
        threads[i].join();
    }

    // 8. Escribir el índice invertido en el archivo (PARTE 2). Ya no hay threads trabajando.
    bool indiceGuardado = escribirIndice(indice, config.archivoIndice);

    // 9. Destruir los semáforos
    sem_destroy(&semCola);
    sem_destroy(&semPantalla);
    sem_destroy(&indice.semaforo);

    // 10. Resumen: verificar que el procesamiento terminó completo
    std::cout << "\n===== RESUMEN =====\n";
    std::cout << "Libros pendientes en la cola: " << cola.size() << "\n";
    std::cout << "Libros procesados: " << librosProcesados << " de " << libros.size() << "\n";
    std::cout << "Threads terminados: " << threads.size() << " de " << config.nThreads << "\n";
    if (indiceGuardado) {
        std::cout << "Indice invertido guardado en " << config.archivoIndice << "\n";
    } else {
        std::cout << "ERROR: el indice invertido no se guardo\n";
    }

    if (indiceGuardado && librosProcesados == (int) libros.size()) return 0;
    return 1;
}
