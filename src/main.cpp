#include "../include/env_config.hpp"
#include "../include/utils.hpp"
#include "../include/menu_repository.hpp"
#include "../include/args.hpp"

#include <map>
#include <iostream>

using namespace std;

int main(int argc, char* argv[]) {
    try {
        Argumentos args;
        if (!leerArgumentos(argc, argv, args)) return 1;

        string profile;
        map<int, bool> perms;

        string USER_FILE, PROFILE_FILE;
        if (!environmentVars(USER_FILE, PROFILE_FILE)) return 1;

        if (!userProfileSearch(USER_FILE, PROFILE_FILE, args.usuario, args.password, perms, profile)) {
            cerr << "\nERROR: autenticacion fallida (usuario, password o perfil invalido).\n";
            return 1;
        }
        mainMenu(USER_FILE, PROFILE_FILE, perms, args.usuario, profile, args.archivo);
    } catch (const std::exception &e) {
        cerr << "\nERROR inesperado: " << e.what() << '\n';
        return 1;
    } catch (...) {
        cerr << "\nERROR inesperado.\n";
        return 1;
    }
    return 0;
}


// g++ -std=c++17 -Wall -o main env_config.h utils.h main.cpp
// ./bin/main -u user -p password -f file