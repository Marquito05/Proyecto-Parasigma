#include <iostream>
#include "../../../include/utils.hpp"

using namespace std;

void menuFunction() {
    int opcion = 0;

    do {
        
        cout << "     EVALUAR f(x) = x^2 + 2*x + 8       \n";
        cout << "";
        cout << "(1) Ingresar valor de x\n";
        cout << "(2) Volver\n";
        cout << "Seleccione una opción: ";
        cin >> noskipws >> opcion;

        while (!std::cin) {
            cout << "Ingrese un valor numerico!\n\n";
            cout << "     EVALUAR f(x) = x^2 + 2*x + 8       \n";
            cout << "";
            cout << "(1) Ingresar valor de x\n";
            cout << "(2) Volver\n";
            cout << "Seleccione una opción: ";
            sanitizeStream();
            cin >> noskipws >> opcion;

        }

        if (opcion == 1) {
            double x;

            cout << "\nIngrese el valor de x: ";
            sanitizeStream();
            cin >> noskipws >> x;

            while (!std::cin) {
                cout << "Ingrese un valor numerico!: ";
                sanitizeStream();
                cin >> noskipws >> x;
            }

            sanitizeStream();
            // Evaluación directa de la función
            double resultado = (x * x) + (2 * x) + 8;

            cout << "\n----------------------------------------\n";
            cout << "[Resultado]: f(" << x << ") = " << x*x << " + " << 2*x << " + 8\n";
            cout << "[Resultado]: f(" << x << ") = " << resultado << "\n";
            cout << "----------------------------------------\n";

        } else if (opcion == 2) {
            cout << "\nRegresando...\n";
        } else {
            cout << "\nOpción inválida. Intente de nuevo.\n";
        }

    } while (opcion != 2);
}