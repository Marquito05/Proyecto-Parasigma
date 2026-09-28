#include <iostream>
#include <string>
#include <algorithm>

bool esPalindromo(std::string texto) {
    std::string limpio = "";
    for (char c : texto) {
        if (isalnum(c)) {
            limpio += tolower(c);
        }
    }
    std::string reverso = limpio;
    reverse(reverso.begin(), reverso.end());
    return limpio == reverso;
}

void menuPalindromo() {
    int subOpcion = 0;
    std::string textoIngresado = "";
    
    std::cout << "          ¿ES PALÍNDROMO?              \n";
    std::cout << "";
    
    std::cout << "Ingrese el texto a evaluar: ";
    std::cin.ignore();
    getline(std::cin, textoIngresado);

    do {
        std::cout << "\n--- Submenú Palíndromo ---\n";
        std::cout << "(1) Validar\n";
        std::cout << "(2) Cancelar\n";
        std::cout << "Seleccione una opción: ";
        std::cin >> subOpcion;

        if (subOpcion == 1) {
            if (esPalindromo(textoIngresado)) {
                std::cout << "\n[Resultado]: ¡El texto SÍ es un palíndromo!\n";
            } else {
                std::cout << "\n[Resultado]: El texto NO es un palíndromo.\n";
            }
            break; 
        } 
        else if (subOpcion == 2) {
            std::cout << "\nRegresando...\n";
            break;
        } 
        else {
            std::cout << "\nOpción inválida. Intente de nuevo.\n";
        }
    } while (subOpcion != 2);
}