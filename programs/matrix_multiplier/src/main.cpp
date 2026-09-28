#include "../include/matrix.hpp"

#include <iostream>
#include <string>

using namespace std;

// Programa multiplicador de matrices (lo llama el menú principal con system()).
// Uso:     ./bin/multi <rutaA> <rutaB> <separador> <usuario> <perfil>
// Ejemplo: ./bin/multi "/home/lvc/A.TXT" "/home/lvc/B.TXT" "#" lvc ADMIN
int main(int argc, char* argv[]) {
    // 1. Validar la cantidad de argumentos (argv[0] es el nombre del programa, por eso son 6)
    if (argc != 6) {
        cout << "ERROR! Cantidad de argumentos incorrecta (se recibieron " << (argc - 1) << " y se esperan 5).\n";
        cout << "Uso:     ./bin/multi <rutaA> <rutaB> <separador> <usuario> <perfil>\n";
        cout << "Ejemplo: ./bin/multi \"/home/lvc/A.TXT\" \"/home/lvc/B.TXT\" \"#\" lvc ADMIN\n";
        return ERROR_ARGUMENTOS;
    }

    string rutaA = argv[1];
    string rutaB = argv[2];
    string textoSeparador = argv[3];
    string usuario = argv[4];
    string perfil = argv[5];

    if (usuario.empty() || perfil.empty()) {
        cout << "ERROR! El usuario y el perfil no pueden estar vacíos.\n";
        return ERROR_ARGUMENTOS;
    }

    // 2. Título con el usuario y el perfil que envió el menú principal
    cout << "\n==================================================\n";
    cout << "            MULTIPLICADOR DE MATRICES\n";
    cout << "  Usuario: " << usuario << "  |  Perfil: " << perfil << "\n";
    cout << "==================================================\n";

    // 3. Validar el separador
    if (!separadorValido(textoSeparador)) {
        cout << "ERROR! Separador inválido: '" << textoSeparador << "'.\n";
        cout << "       Debe ser un solo carácter y no puede ser un dígito, un punto, un signo (+ -) ni un espacio.\n";
        return ERROR_ARGUMENTOS;
    }
    char separador = textoSeparador[0];

    // 4. Leer y validar las dos matrices (si hay un error, leerMatriz ya mostró el detalle)
    Matriz a;
    Matriz b;

    int codigo = leerMatriz(rutaA, separador, a);
    if (codigo != EXITO) return codigo;

    codigo = leerMatriz(rutaB, separador, b);
    if (codigo != EXITO) return codigo;

    cout << "\nMatriz A (" << a.filas << "x" << a.columnas << ") - archivo: " << rutaA << "\n";
    imprimirMatriz(a);
    cout << "\nMatriz B (" << b.filas << "x" << b.columnas << ") - archivo: " << rutaB << "\n";
    imprimirMatriz(b);

    // 5. Validar que se puedan multiplicar: las columnas de A deben ser iguales a las filas de B
    if (a.columnas != b.filas) {
        cout << "\nERROR! No se puede multiplicar A (" << a.filas << "x" << a.columnas
             << ") por B (" << b.filas << "x" << b.columnas << ").\n";
        cout << "       Las columnas de A (" << a.columnas << ") deben ser iguales a las filas de B ("
             << b.filas << ").\n";
        return ERROR_DIMENSIONES;
    }

    // 6. Multiplicar y mostrar el resultado
    Matriz resultado;
    multiplicarMatrices(a, b, resultado);

    cout << "\nResultado A x B (" << resultado.filas << "x" << resultado.columnas << "):\n";
    imprimirMatriz(resultado);

    return EXITO;
}
