#include <iostream>
#include <string>
#include <algorithm>
#include "palindromo.hpp"

using namespace std;

bool esPalindromo(string texto) {
    string limpio = "";
    for (char c : texto) {
        if (isalnum(c)) {
            limpio += tolower(c);
        }
    }
    string reverso = limpio;
    reverse(reverso.begin(), reverso.end());
    return limpio == reverso;
}

void menuPalindromo() {
    int subOpcion = 0;
    string textoIngresado = "";
    
    cout << "          ¿ES PALÍNDROMO?              \n";
    cout << "";
    
    cout << "Ingrese el texto a evaluar: ";
    cin.ignore();
    getline(cin, textoIngresado);

    do {
        cout << "\n--- Submenú Palíndromo ---\n";
        cout << "(1) Validar\n";
        cout << "(2) Cancelar\n";
        cout << "Seleccione una opción: ";
        cin >> subOpcion;

        if (subOpcion == 1) {
            if (esPalindromo(textoIngresado)) {
                cout << "\n[Resultado]: ¡El texto SÍ es un palíndromo!\n";
            } else {
                cout << "\n[Resultado]: El texto NO es un palíndromo.\n";
            }
            break; 
        } 
        else if (subOpcion == 2) {
            cout << "\nRegresando...\n";
            break;
        } 
        else {
            cout << "\nOpción inválida. Intente de nuevo.\n";
        }
    } while (subOpcion != 2);
}