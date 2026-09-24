#include <iostream>
#include <map>
#include "../include/models.hpp"
#include "../../../include/utils.hpp"
#include "../include/user_repository.hpp"
#include "../include/profile_repository.hpp"

using namespace std;

int main(int argc, char* argv[]) {
    string USER_FILE, PROFILE_FILE;
    USER_FILE = argv[1];
    PROFILE_FILE = argv[2];

    int size_arr = 100;

    int perfil_size_arr = size_arr;
    Perfil* perfiles = new Perfil[perfil_size_arr];
    map<string, int> nameTable; // Verificar si perfil ya existe
    int perfil_size = -1;
    cargarPerfiles(perfiles, perfil_size, perfil_size_arr, nameTable, PROFILE_FILE);

    int user_size = -1;
    map<int, int> idTable; // para busqueda y eliminacion O(1) del arreglo dinamico
    int user_size_arr = size_arr;
    User* users = new User[user_size_arr];
    cargarUsuarios(users, perfiles, nameTable, user_size, size_arr, idTable, USER_FILE);

    cout << "user: " << USER_FILE << endl;
    cout << "profile: " << PROFILE_FILE << endl;

    int option = -1;
    while (option != 0) {
        cout << "\n0) Salir \n1) Gestionar usuarios \n2) Gestionar perfiles\nIngresar opcion: ";
        cin >> noskipws >> option;
        while (!cin || option <= -1 || option > 2) {
            cout << "\nERROR! Ingresar valor numerico valido.";
            cout << "\n0) Salir \n1) Gestionar usuarios \n2) Gestionar perfiles\nIngresar opcion: ";
            sanitizeStream();
            cin >> noskipws >> option;
        }

        sanitizeStream();
        cout << "option: " << option << "\n";
        if (option == 1)
            gestionUsuarios(users, perfiles, user_size, perfil_size, user_size_arr, idTable, nameTable, USER_FILE, PROFILE_FILE);
        if (option == 2)
            gestionPerfiles(perfiles, perfil_size, users, user_size, perfil_size_arr, nameTable, idTable, PROFILE_FILE, USER_FILE);
    }
}

// g++ -std=c++17 -Wall -o main main.cpp models.h utils.h env_config.cpp profile_repository.cpp; ./main

/*
15/8:
-Sanitizacion de input añadida

19/8
-Logica de listas dinamicas añadida
-Formateo de print para listar usuarios
-Funcion de insercion añadida

21/8
-Logica de eliminacion de usuarios añadida
-Logica de variables de ambiente

23/8
-Logica de gestion de perfiles añadida
*/
