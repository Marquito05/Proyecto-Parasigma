#include <iostream>
#include <string>
#include "function_fx.hpp"
#include "exprtk.hpp"

using namespace std;

void menuFuncionMatematica() {
    int opcion = 0;

    do {
        cout << "          CALCULAR FUNCIÓN f(x)        \n";
        cout << "";
        cout << "(1) Ingresar función\n";
        cout << "(2) Volver\n";
        cout << "Seleccione una opción: ";
        cin >> opcion;

        if (opcion == 1) {
            string expresion_str;
            double valor_x;

            cout << "\nIngrese la función en términos de x (ej: x^2 + 2*x + 8): ";
            cin.ignore();
            getline(cin, expresion_str);

            cout << "Ingrese el valor de x (número real): ";
            cin >> valor_x;

            // Configuración de ExprTk
            double x = valor_x;
            exprtk::symbol_table<double> symbol_table;
            symbol_table.add_variable("x", x);
            symbol_table.add_constants();

            exprtk::expression<double> expression;
            expression.register_symbol_table(symbol_table);

            exprtk::parser<double> parser;

            if (parser.compile(expresion_str, expression)) {
                double resultado = expression.value();
                cout << "\n----------------------------------------\n";
                cout << "[Resultado]: f(" << x << ") = " << resultado << "\n";
                cout << "----------------------------------------\n";
            } else {
                cout << "\n[Error]: La función ingresada no es válida. Verifique la sintaxis.\n";
            }

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