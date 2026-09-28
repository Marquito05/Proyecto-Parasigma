#include "../include/matrix.hpp"

#include <iostream>
#include <fstream>
#include <iomanip>

using namespace std;

// Retorna true si el carácter es un espacio en blanco (espacio, tabulación o salto de línea)
bool esEspacio(char c) {
    return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

// Quita los espacios en blanco del inicio y del final de un texto.
// Ejemplo: "  12.5 \r" -> "12.5"
string quitarEspacios(const string &texto) {
    int inicio = 0;
    int fin = (int) texto.size() - 1;

    while (inicio <= fin && esEspacio(texto[inicio])) {
        inicio++;
    }
    while (fin >= inicio && esEspacio(texto[fin])) {
        fin--;
    }
    return texto.substr(inicio, fin - inicio + 1);
}

// Revisa que el texto sea un número: signo opcional (+ o -), dígitos y a lo más un punto decimal.
// Válidos: "5", "-3", "2.75", "+0.5"      Inválidos: "", "abc", "1,5", "2..3", "-"
bool esNumero(const string &texto) {
    if (texto.empty()) return false;

    int inicio = 0;
    if (texto[0] == '-' || texto[0] == '+') {
        inicio = 1; // el signo solo puede ir al principio
    }

    int cantidadDigitos = 0;
    int cantidadPuntos = 0;
    for (int i = inicio; i < (int) texto.size(); i++) {
        if (texto[i] >= '0' && texto[i] <= '9') {
            cantidadDigitos++;
        } else if (texto[i] == '.') {
            cantidadPuntos++;
        } else {
            return false; // cualquier otro carácter hace que no sea un número
        }
    }
    return cantidadDigitos > 0 && cantidadPuntos <= 1;
}

// El separador debe ser exactamente un carácter y no puede confundirse con parte de un número.
bool separadorValido(const string &texto) {
    if (texto.size() != 1) return false;

    char c = texto[0];
    if (c >= '0' && c <= '9') return false;             // los dígitos forman los números
    if (c == '.' || c == '-' || c == '+') return false; // el punto y los signos también
    if (esEspacio(c)) return false;                     // los espacios se eliminan al leer
    return true;
}

// Divide una línea en elementos usando el separador, recorriéndola carácter por carácter.
// Ejemplo con '#': "1#2#3" -> {"1", "2", "3"}      "1##3" -> {"1", "", "3"}
vector<string> separarLinea(const string &linea, char separador) {
    vector<string> elementos;
    string actual = "";

    for (int i = 0; i < (int) linea.size(); i++) {
        if (linea[i] == separador) {
            elementos.push_back(actual); // se encontró un separador: terminó un elemento
            actual = "";
        } else {
            actual = actual + linea[i];  // el carácter es parte del elemento actual
        }
    }
    elementos.push_back(actual); // el último elemento no tiene separador después
    return elementos;
}

// Lee una matriz desde un archivo de texto: cada línea es una fila y los elementos
// de la fila van separados por el separador. Valida el formato y el contenido.
// Retorna EXITO o el código de error que corresponda (y muestra el detalle del error).
int leerMatriz(const string &ruta, char separador, Matriz &matriz) {
    ifstream archivo(ruta);
    if (!archivo.is_open()) {
        cout << "ERROR! No se pudo abrir el archivo: " << ruta << "\n";
        cout << "       Revise que la ruta sea correcta y que el archivo exista.\n";
        return ERROR_ARCHIVO;
    }

    matriz.filas = 0;
    matriz.columnas = 0;
    matriz.datos.clear();

    string linea;
    int numeroLinea = 0;

    while (getline(archivo, linea)) {
        numeroLinea++;
        linea = quitarEspacios(linea);
        if (linea.empty()) continue; // las líneas vacías se ignoran

        vector<string> elementos = separarLinea(linea, separador);
        vector<double> fila;

        // Validar y convertir cada elemento de la fila
        for (int j = 0; j < (int) elementos.size(); j++) {
            string elemento = quitarEspacios(elementos[j]);

            if (elemento.empty()) {
                cout << "ERROR! Archivo " << ruta << ", línea " << numeroLinea
                     << ": el elemento " << (j + 1) << " está vacío.\n";
                cout << "       Revise que no haya dos separadores seguidos ni un separador al inicio o al final de la línea.\n";
                archivo.close();
                return ERROR_FORMATO;
            }

            if (!esNumero(elemento)) {
                cout << "ERROR! Archivo " << ruta << ", línea " << numeroLinea
                     << ": el elemento '" << elemento << "' no es un número válido.\n";
                cout << "       Revise que el separador del archivo sea '" << separador
                     << "' y que los decimales usen punto (ej: 2.5).\n";
                archivo.close();
                return ERROR_FORMATO;
            }

            // stod convierte el texto a número real; lanza una excepción si el número es demasiado grande
            try {
                fila.push_back(stod(elemento));
            } catch (...) {
                cout << "ERROR! Archivo " << ruta << ", línea " << numeroLinea
                     << ": el número '" << elemento << "' es demasiado grande.\n";
                archivo.close();
                return ERROR_FORMATO;
            }
        }

        // La primera fila define cuántas columnas tiene la matriz; las demás deben tener las mismas
        if (matriz.filas == 0) {
            matriz.columnas = (int) fila.size();
        } else if ((int) fila.size() != matriz.columnas) {
            cout << "ERROR! Archivo " << ruta << ", línea " << numeroLinea << ": tiene " << fila.size()
                 << " elementos, pero la primera fila tiene " << matriz.columnas << ".\n";
            cout << "       Todas las filas de una matriz deben tener la misma cantidad de columnas.\n";
            archivo.close();
            return ERROR_FORMATO;
        }

        matriz.datos.push_back(fila);
        matriz.filas++;
    }

    // bad() indica que hubo un error al leer (por ejemplo, la ruta es una carpeta y no un archivo)
    if (archivo.bad()) {
        cout << "ERROR! No se pudo leer el archivo: " << ruta << "\n";
        cout << "       Revise que la ruta sea un archivo de texto y no una carpeta.\n";
        archivo.close();
        return ERROR_ARCHIVO;
    }
    archivo.close();

    if (matriz.filas == 0) {
        cout << "ERROR! El archivo " << ruta << " está vacío, no contiene ninguna matriz.\n";
        return ERROR_FORMATO;
    }
    return EXITO;
}

// Multiplica A x B. Cada casilla del resultado es: resultado[i][j] = suma de A[i][k] * B[k][j]
// Antes de llamarla se debe verificar que las columnas de A sean iguales a las filas de B.
void multiplicarMatrices(const Matriz &a, const Matriz &b, Matriz &resultado) {
    resultado.filas = a.filas;        // el resultado tiene las filas de A
    resultado.columnas = b.columnas;  // y las columnas de B
    resultado.datos.clear();

    for (int i = 0; i < a.filas; i++) {
        vector<double> fila;
        for (int j = 0; j < b.columnas; j++) {
            double suma = 0.0;
            for (int k = 0; k < a.columnas; k++) {
                suma = suma + a.datos[i][k] * b.datos[k][j];
            }
            fila.push_back(suma);
        }
        resultado.datos.push_back(fila);
    }
}

// Muestra la matriz por pantalla con 2 decimales y las columnas alineadas
void imprimirMatriz(const Matriz &matriz) {
    cout << fixed << setprecision(2);

    for (int i = 0; i < matriz.filas; i++) {
        for (int j = 0; j < matriz.columnas; j++) {
            double valor = matriz.datos[i][j];
            // Un valor negativo muy cercano a cero se mostraría como "-0.00"; se muestra como 0.00
            if (valor > -0.005 && valor < 0.005) valor = 0.0;
            cout << setw(12) << valor;
        }
        cout << "\n";
    }
}
