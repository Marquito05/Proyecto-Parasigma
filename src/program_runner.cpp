#include "../include/program_runner.hpp"
#include "../include/env_config.hpp"
#include "../include/utils.hpp"

#include <iostream>
#include <string>
#include <cstdlib> // system()
#include <sys/wait.h> // WIFSEXITED, WEXITSTATUS

// Rodea el argumento con comillas simples: texto -> 'texto'
// Dentro de comillas simples la shell NO interpreta caracteres especiales (; | & $ > espacios, etc.),
// así el programa llamado recibe el texto tal cual y nadie puede "colar" otro comando.
std::string protegerArgumento(const std::string &argumento) {
    return "'" + argumento + "'";
}

// Un argumento es seguro si no está vacío y no tiene comillas simples,
// porque una comilla simple cerraría la protección que agrega protegerArgumento().
bool argumentoSeguro(const std::string &argumento) {
    if (argumento.empty()) return false;

    for (size_t i = 0; i < argumento.size(); i++) {
        if (argumento[i] == '\'') return false;
    }
    return true;
}

// Ejecuta un comando con system() y revisa cómo terminó el programa llamado.
int ejecutarPrograma(const std::string &comando) {
    std::cout << "\n[Menu] Ejecutando: " << comando << "\n";
    std::cout.flush(); // el mensaje sale antes de que el otro programa empiece a escribir

    // system() crea un proceso hijo que ejecuta el comando en la shell (/bin/sh)
    // y el menú queda esperando hasta que ese proceso termine
    int estado = system(comando.c_str());
    std::cout << "\n";

    // Prueba 2: -1 significa que no se pudo crear el proceso hijo
    if (estado == -1) {
        std::cout << "[Menu] ERROR! No se pudo crear el proceso para ejecutar el programa.\n";
        return -1;
    }

    // Prueba 3: el programa pudo ser detenido por una señal (ej: kill o Ctrl+C) antes de terminar
    if (WIFSIGNALED(estado)) {
        std::cout << "[Menu] El programa fue interrumpido por la señal " << WTERMSIG(estado) << ".\n";
        return -1;
    }

    // Prueba 4: WEXITSTATUS obtiene el número que retornó el main() del programa llamado
    int codigo = WEXITSTATUS(estado);

    if (codigo == 0) {
        std::cout << "[Menu] El programa terminó correctamente (código 0).\n";
    } else if (codigo == 127) {
        std::cout << "[Menu] ERROR! No se encontró el programa (código 127). " << "Compile con 'make' y revise la ruta en el .env.\n";
    } else if (codigo == 126) {
        std::cout << "[Menu] ERROR! El programa no tiene permisos de ejecución (código 126).\n";
    } else {
        std::cout << "[Menu] El programa terminó con errores (código " << codigo << ").\n";
    }
    return codigo;
}

// Opcion 1 del menu: abre el administrador de usuarios y perfiles (programa de la entrega 1)
void llamarAdminUsuarios(const std::string &userFile, const std::string &profileFile) {
    // La ruta del programa se define en el .env
    std::string programa = leerVariableEnv("USER_ADMIN_PROGRAM");
    if (programa.empty()) {
        std::cout << "ERROR! Falta la variable USER_ADMIN_PROGRAM en el archivo .env.\n";
        return;
    }

    if (!argumentoSeguro(programa) || !argumentoSeguro(userFile) || !argumentoSeguro(profileFile)) {
        std::cout << "ERROR! Las rutas del .env no pueden estar vacías ni contener comillas simples (').\n";
        return;
    }

    // Comando que se ejecuta:  './bin/user_admin' 'USUARIOS.txt' 'PERFILES.txt'
    std::string comando = protegerArgumento(programa) + " " + protegerArgumento(userFile) + " " + protegerArgumento(profileFile);
    ejecutarPrograma(comando);
}

// Opción 2 del menú: pide las rutas de las matrices y el separador, y llama al multiplicador
void llamarMultiplicador(const std::string &usuario, const std::string &perfil) {
    // La ruta del programa se define en el .env
    std::string programa = leerVariableEnv("MULTI_PROGRAM");
    if (programa.empty()) {
        std::cout << "ERROR! Falta la variable MULTI_PROGRAM en el archivo .env.\n";
        return;
    }

    std::string rutaA, rutaB, separador;

    std::cout << "\n=== MULTIPLICAR MATRICES ===\n";
    std::cout << "Cada archivo debe tener una fila de la matriz por linea, con los numeros separados por el separador.\n";
    std::cout << "Para cancelar, deje un dato vacio y presione ENTER.\n\n";

    std::cout << "Ruta del archivo de la matriz A: ";
    getline(std::cin, rutaA);
    rutaA = get_trimmed_string(rutaA);
    if (rutaA.empty()) {
        std::cout << "Operacion cancelada.\n";
        return;
    }

    std::cout << "Ruta del archivo de la matriz B: ";
    getline(std::cin, rutaB);
    rutaB = get_trimmed_string(rutaB);
    if (rutaB.empty()) {
        std::cout << "Operacion cancelada.\n";
        return;
    }

    std::cout << "Separador de los elementos (ej: #): ";
    getline(std::cin, separador);
    separador = get_trimmed_string(separador);
    if (separador.empty()) {
        std::cout << "Operacion cancelada.\n";
        return;
    }

    // Proteger la integridad del sistema: se rechaza cualquier dato con comillas simples
    if (!argumentoSeguro(programa) || !argumentoSeguro(rutaA) || !argumentoSeguro(rutaB) || 
        !argumentoSeguro(separador) || !argumentoSeguro(usuario) || !argumentoSeguro(perfil)) {
        std::cout << "ERROR! Los datos no pueden contener comillas simples ('). Operacion cancelada.\n";
        return;
    }

    // Comando que se ejecuta:  './bin/multi' 'rutaA' 'rutaB' 'separador' 'usuario' 'perfil'
    // (el programa multi se encarga de validar los archivos, el separador y las dimensiones)
    std::string comando = protegerArgumento(programa) + " " + protegerArgumento(rutaA) + " " 
                        + protegerArgumento(rutaB) + " " + protegerArgumento(separador) + " "
                        + protegerArgumento(usuario) + " " + protegerArgumento(perfil);
    ejecutarPrograma(comando);

    // Pausa para alcanzar a leer el resultado antes de que vuelva a aparecer el menú
    std::cout << "\nPresione ENTER para volver al menu principal...";
    std::string pausa;
    getline(std::cin, pausa);
}


void llamarPalindromo() {
    // La ruta del programa se define en el .env
    std::string programa = leerVariableEnv("PALINDROMO_PROGRAM");
    if (programa.empty()) {
        std::cout << "ERROR! Falta la variable PALINDROMO_PROGRAM en el archivo .env.\n";
        return;
    }

    if (!argumentoSeguro(programa)) {
        std::cout << "ERROR! La ruta del .env no puede contener comillas simples (').\n";
        return;
    }

    // Comando que se ejecuta:  './bin/palindromo'
    ejecutarPrograma(protegerArgumento(programa));
}


void llamarFx() {
    // La ruta del programa se define en el .env
    std::string programa = leerVariableEnv("FX_PROGRAM");
    if (programa.empty()) {
        std::cout << "ERROR! Falta la variable FX_PROGRAM en el archivo .env.\n";
        return;
    }

    if (!argumentoSeguro(programa)) {
        std::cout << "ERROR! La ruta del .env no puede contener comillas simples (').\n";
        return;
    }

    // Comando que se ejecuta:  './bin/funcion_fx'
    ejecutarPrograma(protegerArgumento(programa));
}

void llamarConteoTexto(const std::string &archivo) {
    // La ruta del programa se define en el .env
    std::string programa = leerVariableEnv("CONTEO_PROGRAM");
    if (programa.empty()) {
        std::cout << "ERROR! Falta la variable CONTEO_PROGRAM en el archivo .env.\n";
        return;
    }

    if (!argumentoSeguro(programa) || !argumentoSeguro(archivo)) {
        std::cout << "ERROR! Las rutas no pueden estar vacias ni contener comillas simples (').\n";
        return;
    }

    // Comando que se ejecuta:  './bin/conteo' 'archivo'
    ejecutarPrograma(protegerArgumento(programa) + " " + protegerArgumento(archivo));
}


void llamarConteoArchivo() {
    std::cout << "estoy dentro\n";
    // La ruta del programa se define en el .env
    std::string programa = leerVariableEnv("CONTEO_PROGRAM");
    if (programa.empty()) {
        std::cout << "ERROR! Falta la variable CONTEO_PROGRAM en el archivo .env.\n";
        return;
    }

    if (!argumentoSeguro(programa)) {
        std::cout << "ERROR! La ruta del .env no puede contener comillas simples (').\n";
        return;
    }

    // Comando que se ejecuta:  './bin/conteo'
    ejecutarPrograma(protegerArgumento(programa));
}