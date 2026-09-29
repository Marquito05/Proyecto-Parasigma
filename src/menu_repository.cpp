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


static void mostrarMenu(const std::string &user, const std::string &profile) {
    std::cout << "\n========================================\n"
              << "          MENU PRINCIPAL - SistOpe\n"
              << "========================================\n"
              << "Usuario: " << user << "\n"
              << "Perfil:  " << profile << "\n"
              << "----------------------------------------\n"
              << "1) Administrar usuarios y perfiles\n"
              << "2) Multiplicar matrices NxM\n"
              << "3) Juego\n"
              << "4) Es palindromo?\n"
              << "5) Calcular f(x) = x*x + 2x + 8\n"
              << "6) Conteo sobre texto (archivo -f)\n"
              << "7) Conteo sobre archivo\n"
              << "0) Salir\n"
              << "----------------------------------------\n"
              << "Ingresar opcion: ";
}

// Lee una opcion del menu (0-7). Retorna false si se cerro la entrada (EOF).
static bool leerOpcion(int &option) {
    std::string linea;
    while (true) {
        if (!std::getline(std::cin, linea)) return false;
        linea = get_trimmed_string(linea);
        if (!linea.empty() && linea.size() <= 2 && isdigit_string(linea)) {
            option = std::stoi(linea);
            if (option >= 0 && option <= 7) return true;
        }
        std::cout << "\nERROR: opcion invalida. Ingrese un numero entre 0 y 7.\nIngresar opcion: ";
    }
}

void mainMenu(const std::string &user_file, const std::string &profile_file, std::map<int, bool> &perms,
              const std::string &user, const std::string &profile, const std::string &file) {

    int option = -1;
    while (option != 0) {
        mostrarMenu(user, profile);
        if (!leerOpcion(option)) {
            std::cout << "\nEntrada finalizada. Saliendo del sistema.\n";
            return;
        }

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
            llamarPalindromo(); // ejecuta ./bin/palindromo con system()
        }
        else if (option == 5) {
            llamarFuncion(); // ejecuta ./bin/funcion_fx con system()
        }
        else if (option == 6) {
            llamarConteoTexto(file); // ejecuta ./bin/conteo con el archivo de -f
        }
        else if (option == 7) {
            llamarConteoArchivo(); // ejecuta ./bin/conteo, que pide la ruta
        }
    }
    std::cout << "\nSaliendo del sistema. Hasta luego!\n";
}
