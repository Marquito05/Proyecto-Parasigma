#ifndef CONTEO_HPP
#define CONTEO_HPP

#include <cstdint>
#include <string>

// Estructuras

struct Conteo {
    long vocales = 0;
    long consonantes = 0;
    long especiales = 0;
    long palabras = 0;
    long bytes = 0;
};

struct Contador {
    Conteo r;
    bool enPalabra = false;
    bool palabraValida = false;
    uint32_t cp = 0;
    int faltan = 0;
    unsigned char pend[4] = {0, 0, 0, 0};
    int nPend = 0;

    void caracter(uint32_t c);
    void cerrarPalabra();
    void vaciarPendientes();
    void byte(unsigned char b);
    void terminar();
};

// Declaraciones de funciones

bool cpEsEspacio(uint32_t c);
bool cpEsDigito(uint32_t c);
bool cpEsVocal(uint32_t c);
bool cpEsLetra(uint32_t c);

std::string normalizarRuta(std::string ruta);
bool contarArchivo(const std::string &rutaOriginal, Conteo &resultado, std::string &error);

void esperarVolver();
void mostrarConteo(const std::string &ruta);
void opcionConteoTexto(const std::string &archivo);
void opcionConteoArchivo();

#endif