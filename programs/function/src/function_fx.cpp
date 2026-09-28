#include <iostream>
#include "function_fx.hpp"

using namespace std;

void menuFunction() {
    int opcion = 0;

    do {
        
        cout << "     EVALUAR f(x) = x^2 + 2*x + 8       \n";
        cout << "";
        cout << "(1) Ingresar valor de x\n";
        cout << "(2) Volver\n";
        cout << "Seleccione una opción: ";
        cin >> opcion;

        if (opcion == 1) {
            double x;

            cout << "\nIngrese el valor de x: ";
            cin >> x;

            // Evaluación directa de la función
            double resultado = (x * x) + (2 * x) + 8;

            cout << "\n----------------------------------------\n";
            cout << "[Resultado]: f(" << x << ") = " << resultado << "\n";
            cout << "----------------------------------------\n";

            cout << "\nPresione Enter para continuar...";
            cin.ignore();
            cin.get();

        } else if (opcion == 2) {
            cout << "\nRegresando...\n";
        } else {
            cout << "\nOpción inválida. Intente de nuevo.\n";
        }

    } while (opcion != 2);
}