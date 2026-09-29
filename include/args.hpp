#ifndef ARGS_HPP
#define ARGS_HPP

#include <iostream>
#include <string>

// Argumentos de ejecución: -u (usuario), -p (password), -f (archivo)
struct Argumentos {
    std::string usuario;
    std::string password;
    std::string archivo;
};

inline void mostrarUso(const std::string &programa) {
    std::cerr << "Uso: " << programa << " -u <usuario> -p <password> -f <archivo>\n"
              << "Ejemplo: " << programa << " -u lvc -p 1001 -f \"/home/lvc/archivo.txt\"\n";
}

// Lee y valida los argumentos. Retorna false (con mensaje de error) si son inválidos.
inline bool leerArgumentos(int argc, char* argv[], Argumentos &a) {
    const std::string prog = (argc > 0 && argv[0]) ? argv[0] : "main";
    bool tieneU = false, tieneP = false, tieneF = false;

    if (argc < 2) {
        std::cerr << "Error: faltan argumentos de ejecucion.\n";
        mostrarUso(prog);
        return false;
    }

    for (int i = 1; i < argc; i++) {
        const std::string flag = argv[i];

        if (flag != "-u" && flag != "-p" && flag != "-f") {
            std::cerr << "Error: argumento desconocido '" << flag << "'.\n";
            mostrarUso(prog);
            return false;
        }
        if (i + 1 >= argc) {
            std::cerr << "Error: el argumento " << flag << " requiere un valor.\n";
            mostrarUso(prog);
            return false;
        }

        const std::string valor = argv[++i];
        if (valor.empty()) {
            std::cerr << "Error: el valor de " << flag << " esta vacio.\n";
            return false;
        }
        if (valor == "-u" || valor == "-p" || valor == "-f") {
            std::cerr << "Error: el argumento " << flag << " requiere un valor (se encontro '" << valor << "').\n";
            mostrarUso(prog);
            return false;
        }

        bool &yaVisto = (flag == "-u") ? tieneU : (flag == "-p") ? tieneP : tieneF;
        if (yaVisto) {
            std::cerr << "Error: el argumento " << flag << " esta repetido.\n";
            return false;
        }
        yaVisto = true;

        if (flag == "-u") a.usuario = valor;
        else if (flag == "-p") a.password = valor;
        else a.archivo = valor;
    }

    if (!tieneU || !tieneP || !tieneF) {
        std::cerr << "Error: faltan argumentos obligatorios:";
        if (!tieneU) std::cerr << " -u";
        if (!tieneP) std::cerr << " -p";
        if (!tieneF) std::cerr << " -f";
        std::cerr << "\n";
        mostrarUso(prog);
        return false;
    }
    return true;
}

#endif
