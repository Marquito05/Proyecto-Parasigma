#ifndef CONTEO_HPP
#define CONTEO_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <cctype>

struct Conteo {
    long vocales = 0;
    long consonantes = 0;
    long especiales = 0;
    long palabras = 0;
};

inline bool esVocalAscii(unsigned char c) {
    int l = std::tolower(c);
    return l == 'a' || l == 'e' || l == 'i' || l == 'o' || l == 'u';
}

// Cuenta vocales, consonantes, caracteres especiales y palabras de un archivo
inline bool contarArchivo(const std::string &ruta, Conteo &r) {
    std::ifstream in(ruta, std::ios::binary);
    if (!in.is_open()) return false;

    r = Conteo();
    bool enPalabra = false;
    char ch;

    while (in.get(ch)) {
        unsigned char c = static_cast<unsigned char>(ch);

        // Letras UTF-8 con tilde/ñ (empiezan con 0xC3)
        if (c == 0xC3) {
            char sig;
            if (in.get(sig)) {
                unsigned char s = static_cast<unsigned char>(sig);
                if (s==0xA1||s==0xA9||s==0xAD||s==0xB3||s==0xBA||s==0xBC||
                    s==0x81||s==0x89||s==0x8D||s==0x93||s==0x9A||s==0x9C)
                    r.vocales++;
                else if (s==0xB1||s==0x91)
                    r.consonantes++;
                else
                    r.especiales++;
                if (!enPalabra) { r.palabras++; enPalabra = true; }
            }
            continue;
        }

        if (std::isalpha(c)) {
            if (esVocalAscii(c)) r.vocales++;
            else r.consonantes++;
            if (!enPalabra) { r.palabras++; enPalabra = true; }
        } else if (std::isspace(c)) {
            enPalabra = false;
        } else if (std::isdigit(c)) {
            if (!enPalabra) { r.palabras++; enPalabra = true; }
        } else if (c < 0x80) {
            r.especiales++;
        }
    }
    return true;
}

inline void mostrarConteo(const std::string &ruta) {
    Conteo r;
    if (!contarArchivo(ruta, r)) {
        std::cout << "Error: no se pudo abrir el archivo '" << ruta << "'.\n";
    } else {
        std::cout << "\n===== CONTEO =====\n";
        std::cout << "Archivo: " << ruta << "\n";
        std::cout << "Vocales: " << r.vocales << "\n";
        std::cout << "Consonantes: " << r.consonantes << "\n";
        std::cout << "Caracteres especiales: " << r.especiales << "\n";
        std::cout << "Palabras: " << r.palabras << "\n";
        std::cout << "==================\n";
    }
    std::cout << "\n(Enter para volver)";
    std::cin.get();
}

// Opción 6: usa el archivo recibido por argumento
inline void opcionConteoTexto(const std::string &archivo) {
    if (archivo.empty()) {
        std::cout << "Error: no se indico archivo.\n(Enter para volver)";
        std::cin.get();
        return;
    }
    mostrarConteo(archivo);
}

// Opción 7: pide la ruta al usuario
inline void opcionConteoArchivo() {
    std::string ruta;
    std::cout << "Ingrese la ruta del archivo: ";
    std::getline(std::cin, ruta);
    if (ruta.empty()) {
        std::cout << "Error: ruta vacia.\n(Enter para volver)";
        std::cin.get();
        return;
    }
    mostrarConteo(ruta);
}

#endif
