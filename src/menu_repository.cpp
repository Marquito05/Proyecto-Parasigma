#include "../include/utils.hpp"
#include "../include/program_runner.hpp"

#include <filesystem>
#include <fstream>
#include <algorithm>
#include <iostream>

bool userProfileSearch(const std::string &user_file, const std::string &profile_file, const std::string &user, 
                        const std::string &password, std::map<int, bool> &permList, std::string &profileName) {
    // cargar usuarios
    std::string line, usr, pswd, profile, token;
    std::ifstream usrFile(user_file);

    while (getline(usrFile, line)) {
        line = get_trimmed_string(line);
        if (line.empty()) continue;

        std::stringstream st(line);

        if (!getline(st, token, ',')) continue; 
        token = get_trimmed_string(token);
        if (token.empty() || !std::all_of(token.begin(), token.end(), ::isdigit)) continue;

        try {
            if (stoi(token) <= 0) continue; // ID debe ser > 0
        } catch (...) {
            continue; // Si el ID supera el límite numérico o está corrupto, lo salta
        }

        if (!getline(st, token, ',')) continue; //name
        if (!getline(st, token, ',')) continue; //username
        if (token == user)  {
            usr = token;
        }
        else continue;

        if (!getline(st, token, ',')) continue; //password
        if (token == password) pswd = password;
        else { // caso: usuario valido pero contraseña invalida (usuarios no se pueden repetir)
            std::cout << "Usuario y/o contraseña invalidos.";
            return false;
        }

        if (!getline(st, profile, ',')) continue; //profile
        if (user == usr && password == pswd) { // terminar iteracion si usuario y contraseña son validos
            break;
        }
    }
    usrFile.close();
    //std::cout << "profile: " << profile << '\n';

    if (user == usr && password == pswd) {
        std::ifstream profFile(profile_file); // Checkear perfil
        while (getline(profFile, line)) {
            line = get_trimmed_string(line);
            if (line.empty()) continue; // Ignora líneas vacías

            size_t e = line.find(';');
            if (e == std::string::npos) continue; // Omite líneas corruptas sin ';'

            token = toUpperString(get_trimmed_string(line.substr(0, e))); // name

            // Validar que el nombre del perfil contenga SOLO letras (se salta si tiene números o símbolos)
            if (token.empty() || !isalpha_string(token)) continue; 
            if (token != profile) continue;

            std::string dataline = line.substr(e + 1);
            std::stringstream st(dataline);
            std::string permToken;
            bool permisosValidos = true;
            std::map<int, bool> tempPerms;

            // Validar que cada token de permiso contenga SOLO números
            while (getline(st, permToken, ',')) {
                permToken = get_trimmed_string(permToken);
                if (permToken.empty() || !isdigit_string(permToken)) {
                    permisosValidos = false; // Si un permiso tiene letras, se marca todo el perfil como invalido
                    break;
                }
                try {
                    tempPerms[stoi(permToken)] = true;
                    //std::cout << "permiso: " << permToken << '\n';
                } catch (...) {
                    permisosValidos = false;
                    break;
                }
            }

            // Si la línea tiene permisos inválidos o está vacía, se salta la línea completa
            if (permisosValidos==false || tempPerms.empty()) continue;
            permList = tempPerms;
            profileName = profile; // se devuelve el perfil para mostrarlo y enviarlo a otros programas
            return true;
        }
    }
    return false;

}


void mainMenu(const std::string &user_file, const std::string &profile_file, std::map<int, bool> &perms,
              const std::string &user, const std::string &profile) {

    int option = -1;
    while (option != 0) {
        std::cout << "\n1) Administrar usuarios\n2) Multiplicar Matrices NXN\n3) Juego\n4) Test palindrome\n5) Ejecutar f(x) = x^2 + 2x + g\n6) Conteo sobre texto\n0) Salir\nIngresar opcion: ";
        std::cin >> std::noskipws >> option;
        while (!std::cin || option <= -1 || option > 6) {
            std::cout << "\nERROR! Ingresar valor numerico valido.";
            std::cout << "\n1) Administrar usuarios\n2) Multiplicar Matrices NXN\n3) Juego\n4) Test palindrome\n5) Ejecutar f(x) = x^2 + 2x + g\n6) Conteo sobre texto\n0) Salir\nIngresar opcion: ";
            sanitizeStream();
            std::cin >> std::noskipws >> option;
        }

        sanitizeStream();
        //std::cout << "option: " << option << "\n";
        if (option == 1 && perms.find(option) != perms.end()) {
            llamarAdminUsuarios(user_file, profile_file); // ejecuta ./bin/user_admin con system()
        }
        else if (option == 1 && perms.find(option) == perms.end()) std::cout << "Usuario no tiene permisos para este modulo." << '\n';

        else if (option == 2 && perms.find(option) != perms.end()) {
            llamarMultiplicador(user, profile); // ejecuta ./bin/multi con system()
        }
        else if (option == 2 && perms.find(option) == perms.end()) std::cout << "Usuario no tiene permisos para este modulo." << '\n';

        else if (option == 3) {
            std::cout << "Opcion en construccion!" << '\n';
        }
        else if (option == 4) {
            llamarPalindrome();
        }
        else if (option == 5) {
            llamarFx();
        }
        else if (option == 6) {
            std::cout << "Opcion en construccion!" << '\n';
        }
        else if (option == 7) {
            std::cout << "Opcion en construccion!" << '\n';
        }
    }
}
