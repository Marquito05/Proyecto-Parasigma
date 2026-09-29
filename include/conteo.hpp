#ifndef CONTEO_HPP
#define CONTEO_HPP

#include <cctype>
#include <cstdint>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <system_error>

struct Conteo {
    long vocales = 0;
    long consonantes = 0;
    long especiales = 0;
    long palabras = 0;
    long bytes = 0;
};

// ---------- Clasificación de caracteres (Unicode, cubre español y Latin-1) ----------

inline bool cpEsEspacio(uint32_t c) {
    return c == ' ' || (c >= 9 && c <= 13) || c == 0xA0 || c == 0x2028 || c == 0x2029 || c == 0x3000
        || (c >= 0x2000 && c <= 0x200A);
}

inline bool cpEsDigito(uint32_t c) { return c >= '0' && c <= '9'; }

inline bool cpEsVocal(uint32_t c) {
    if (c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u') return true;
    if (c == 'A' || c == 'E' || c == 'I' || c == 'O' || c == 'U') return true;
    return (c >= 0xC0 && c <= 0xC5) || (c >= 0xC8 && c <= 0xCF) || (c >= 0xD2 && c <= 0xD6)
        || (c >= 0xD9 && c <= 0xDC) || (c >= 0xE0 && c <= 0xE5) || (c >= 0xE8 && c <= 0xEF)
        || (c >= 0xF2 && c <= 0xF6) || (c >= 0xF9 && c <= 0xFC);
}

inline bool cpEsLetra(uint32_t c) {
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) return true;
    if (c >= 0xC0 && c <= 0x17F && c != 0xD7 && c != 0xF7) return true; // Latin-1 y Latin Extendido-A
    return false;
}

// ---------- Contador con decodificador UTF-8 incremental ----------
// Si un byte no forma UTF-8 válido se interpreta como Latin-1 (archivos antiguos).

struct Contador {
    Conteo r;
    bool enPalabra = false;
    bool palabraValida = false;
    uint32_t cp = 0;
    int faltan = 0;
    unsigned char pend[4] = {0, 0, 0, 0};
    int nPend = 0;

    void caracter(uint32_t c) {
        if (c == 0xFEFF) return; // BOM
        if (cpEsEspacio(c)) {
            cerrarPalabra();
            return;
        }
        enPalabra = true;
        if (cpEsLetra(c)) {
            if (cpEsVocal(c)) r.vocales++; else r.consonantes++;
            palabraValida = true;
        } else if (cpEsDigito(c)) {
            palabraValida = true;
        } else {
            r.especiales++;
        }
    }

    void cerrarPalabra() {
        if (enPalabra && palabraValida) r.palabras++;
        enPalabra = false;
        palabraValida = false;
    }

    void vaciarPendientes() {
        for (int i = 0; i < nPend; i++) caracter(pend[i]); // como Latin-1
        nPend = 0;
        faltan = 0;
    }

    void byte(unsigned char b) {
        if (faltan > 0) {
            if ((b & 0xC0) == 0x80) {
                cp = (cp << 6) | (b & 0x3Fu);
                pend[nPend++] = b;
                if (--faltan == 0) { caracter(cp); nPend = 0; }
                return;
            }
            vaciarPendientes(); // secuencia inválida: se procesa b como byte nuevo
        }
        if (b < 0x80) caracter(b);
        else if (b >= 0xC2 && b <= 0xDF) { faltan = 1; cp = b & 0x1Fu; pend[0] = b; nPend = 1; }
        else if (b >= 0xE0 && b <= 0xEF) { faltan = 2; cp = b & 0x0Fu; pend[0] = b; nPend = 1; }
        else if (b >= 0xF0 && b <= 0xF4) { faltan = 3; cp = b & 0x07u; pend[0] = b; nPend = 1; }
        else caracter(b); // byte suelto Latin-1
    }

    void terminar() {
        vaciarPendientes();
        cerrarPalabra();
    }
};

// ---------- Manejo de rutas ----------

// Quita espacios y comillas de los bordes; convierte "C:\dir\a.txt" a "/mnt/c/dir/a.txt" (WSL)
inline std::string normalizarRuta(std::string ruta) {
    auto recortar = [](std::string &s) {
        size_t i = s.find_first_not_of(" \t\r\n");
        size_t j = s.find_last_not_of(" \t\r\n");
        s = (i == std::string::npos) ? "" : s.substr(i, j - i + 1);
    };
    recortar(ruta);
    if (ruta.size() >= 2 && (ruta.front() == '"' || ruta.front() == '\'') && ruta.back() == ruta.front())
        ruta = ruta.substr(1, ruta.size() - 2);
    recortar(ruta);

    if (ruta.size() >= 3 && std::isalpha(static_cast<unsigned char>(ruta[0])) && ruta[1] == ':'
        && (ruta[2] == '\\' || ruta[2] == '/')) {
        std::string resto = ruta.substr(3);
        for (char &c : resto) if (c == '\\') c = '/';
        ruta = std::string("/mnt/") + static_cast<char>(std::tolower(static_cast<unsigned char>(ruta[0]))) + "/" + resto;
    }
    return ruta;
}

// Cuenta el archivo. Retorna true si se pudo; si no, deja el motivo en "error".
inline bool contarArchivo(const std::string &rutaOriginal, Conteo &resultado, std::string &error) {
    try {
        const std::string ruta = normalizarRuta(rutaOriginal);

        if (ruta.empty()) { error = "la ruta esta vacia."; return false; }
        if (ruta.size() > 4096) { error = "la ruta es demasiado larga."; return false; }
        if (ruta.find('\0') != std::string::npos) { error = "la ruta contiene caracteres invalidos."; return false; }

        std::error_code ec;
        std::filesystem::path p(ruta);
        if (!std::filesystem::exists(p, ec)) {
            error = ec ? "no se pudo acceder a la ruta (" + ec.message() + ")." : "el archivo no existe.";
            return false;
        }
        if (std::filesystem::is_directory(p, ec)) { error = "la ruta es una carpeta, no un archivo."; return false; }
        if (!std::filesystem::is_regular_file(p, ec)) { error = "la ruta no es un archivo de texto regular."; return false; }

        std::ifstream in(ruta, std::ios::binary);
        if (!in.is_open()) { error = "no se pudo abrir el archivo (revise los permisos)."; return false; }

        Contador c;
        char buf[65536];
        while (in) {
            in.read(buf, sizeof(buf));
            std::streamsize n = in.gcount();
            for (std::streamsize i = 0; i < n; i++) {
                unsigned char b = static_cast<unsigned char>(buf[i]);
                if (b == 0) { error = "el archivo parece binario (contiene bytes nulos), no es un archivo de texto."; return false; }
                c.byte(b);
            }
            c.r.bytes += static_cast<long>(n);
        }
        if (in.bad()) { error = "ocurrio un error al leer el archivo."; return false; }

        c.terminar();
        resultado = c.r;
        return true;
    } catch (const std::exception &e) {
        error = std::string("error inesperado: ") + e.what();
        return false;
    } catch (...) {
        error = "error inesperado.";
        return false;
    }
}

// ---------- Interfaz ----------

inline void esperarVolver() {
    std::cout << "\nPresione Enter para VOLVER...";
    std::string basura;
    std::getline(std::cin, basura);
}

inline void mostrarConteo(const std::string &ruta) {
    Conteo r;
    std::string error;
    if (!contarArchivo(ruta, r, error)) {
        std::cout << "\nERROR: " << error << '\n';
    } else {
        std::cout << "\n===== CONTEO =====\n"
                  << "Archivo: " << normalizarRuta(ruta) << '\n'
                  << "Vocales: " << r.vocales << '\n'
                  << "Consonantes: " << r.consonantes << '\n'
                  << "Caracteres especiales: " << r.especiales << '\n'
                  << "Palabras: " << r.palabras << '\n'
                  << "==================\n";
        if (r.bytes == 0) std::cout << "Aviso: el archivo esta vacio.\n";
    }
    esperarVolver();
}

// Opción 6: cuenta el archivo recibido con -f
inline void opcionConteoTexto(const std::string &archivo) {
    std::cout << "\n--- CONTEO SOBRE TEXTO ---\n";
    mostrarConteo(archivo);
}

// Opción 7: pide la ruta del archivo
inline void opcionConteoArchivo() {
    std::cout << "\n--- CONTEO SOBRE ARCHIVO ---\n";
    std::cout << "Ingrese la ruta del archivo (vacio para VOLVER): ";
    std::string ruta;
    if (!std::getline(std::cin, ruta)) { std::cin.clear(); return; }
    if (normalizarRuta(ruta).empty()) return;
    mostrarConteo(ruta);
}

#endif
