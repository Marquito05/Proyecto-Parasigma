#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <string>
#include <vector>

// Codigos de salida del programa (el valor que retorna main).
// El menu principal los recibe a través de system() para saber cómo termino el programa.
const int EXITO = 0;              // la multiplicación se realizó correctamente
const int ERROR_ARGUMENTOS = 1;   // faltan argumentos o el separador no es válido
const int ERROR_ARCHIVO = 2;      // no se pudo abrir alguno de los archivos
const int ERROR_FORMATO = 3;      // el contenido de un archivo no tiene el formato correcto
const int ERROR_DIMENSIONES = 4;  // las matrices no se pueden multiplicar

// Una matriz se guarda como una lista de filas, cada fila es una lista de numeros.
struct Matriz {
    int filas;
    int columnas;
    std::vector<std::vector<double>> datos; // datos[fila][columna]
};

// Funciones auxiliares de texto
bool esEspacio(char c);
std::string quitarEspacios(const std::string &texto);
bool esNumero(const std::string &texto);
bool separadorValido(const std::string &texto);
std::vector<std::string> separarLinea(const std::string &linea, char separador);

// Funciones de matrices
int leerMatriz(const std::string &ruta, char separador, Matriz &matriz);
void multiplicarMatrices(const Matriz &a, const Matriz &b, Matriz &resultado);
void imprimirMatriz(const Matriz &matriz);

#endif
