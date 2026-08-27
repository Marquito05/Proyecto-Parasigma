#include "env_config.h"
#include <fstream>
#include <filesystem>
#include <string>

using namespace std;

void environmentVars(string &userFile, string &profileFile) {
    if (!filesystem::exists(".env")) {
        filesystem::path p = filesystem::current_path();
        filesystem::path filefs = p / "USUARIOS.txt";
        filesystem::path profilefs = p / "PERFILES.txt";

        string file = filefs.string();
        string profile = profilefs.string();

        ofstream File(".env");
        File << "USER_FILE=" << file << "\n";
        File << "PROFILE_FILE=" << profile << "\n";
        File.close();

        if (!filesystem::exists("PERFILES.txt")) {
            ofstream fileProfile("PERFILES.txt");
            fileProfile << "ADMIN;0,1,2,3,4" << endl << "GENERAL;0,1,3" << endl;
            fileProfile.close();
        }

        if (!filesystem::exists("USUARIOS.txt")) {
            ofstream fileUser("USUARIOS.txt");
            fileUser.close();
        }
    }

    // Lectura segura del archivo .env buscando el '='
    ifstream envFile(".env");
    string line;

    while (getline(envFile, line)) {
        size_t eqPos = line.find('=');
        if (eqPos != string::npos) {
            string key = line.substr(0, eqPos);
            string value = line.substr(eqPos + 1);

            // Elimina posibles saltos de línea invisibles (\r) al final
            if (!value.empty() && value.back() == '\r') {
                value.pop_back();
            }

            if (key == "USER_FILE") userFile = value;
            if (key == "PROFILE_FILE") profileFile = value;
        }
    }
    envFile.close();

    // Crear archivos si no existen usando rutas válidas
    if (!profileFile.empty() && !filesystem::exists(profileFile)) {
        filesystem::path p = profileFile;
        if (p.has_parent_path()) filesystem::create_directories(p.parent_path());
        ofstream fileProfile(p);
        fileProfile << "ADMIN;0,1,2,3,4" << endl << "GENERAL;0,1,3" << endl;
        fileProfile.close();
    }

    if (!userFile.empty() && !filesystem::exists(userFile)) {
        filesystem::path p = userFile;
        if (p.has_parent_path()) filesystem::create_directories(p.parent_path());
        ofstream fileUser(p);
        fileUser.close();
    }
}