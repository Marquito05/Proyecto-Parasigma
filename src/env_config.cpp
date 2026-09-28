#include <fstream>
#include <iostream>
#include <filesystem>
#include <string>

bool environmentVars(std::string &userFile, std::string &profileFile) {
    if (!std::filesystem::exists(".env")) {
        std::cout << ".env no existe en path de ejecucion, porfavor cree un archivo .env.\n";
        return false;
    }

    // Lectura segura del archivo .env buscando el '='
    std::ifstream envFile(".env");
    std::string line;

    while (getline(envFile, line)) {
        size_t eqPos = line.find('=');
        if (eqPos != std::string::npos) {
            std::string key = line.substr(0, eqPos);
            std::string value = line.substr(eqPos + 1);

            // Elimina posibles saltos de línea invisibles (\r) al final
            if (!value.empty() && value.back() == '\r') {
                value.pop_back();
            }

            if (key == "USER_FILE") userFile = value;
            if (key == "PROFILE_FILE") profileFile = value;
        }
    }

    if (userFile.empty() || profileFile.empty()) {
        std::cout << "Variables de entorno no validas.\n";
        return false;
    }

    envFile.close();

    //std::cout << "profile: " << profileFile << "\n";
    //std::cout << "User: " << userFile << "\n";

    // Crear archivos si no existen usando rutas válidas
    if (!std::filesystem::exists(profileFile)) {
        std::cout << "Archivo de perfil no existe en el path definido en las variables de entorno.\n";
        return false;
    }

    if (!std::filesystem::exists(userFile)) {
        std::cout << "Archivo de usuarios no existe en el path definido en las variables de entorno.\n";
        return false;
    }
    return true;
}

// Busca una variable en el archivo .env y retorna su valor.
// Ejemplo: si el .env tiene "MULTI_PROGRAM=./bin/multi", leerVariableEnv("MULTI_PROGRAM") retorna "./bin/multi".
// Si el archivo o la variable no existen, retorna un texto vacío.
std::string leerVariableEnv(const std::string &nombre) {
    std::ifstream envFile(".env");
    std::string line;

    while (getline(envFile, line)) {
        size_t eqPos = line.find('=');
        if (eqPos == std::string::npos) continue; // línea sin '=': no es una variable

        std::string key = line.substr(0, eqPos);
        std::string value = line.substr(eqPos + 1);

        // Elimina posibles saltos de línea invisibles (\r) al final
        if (!value.empty() && value.back() == '\r') {
            value.pop_back();
        }

        if (key == nombre) {
            envFile.close();
            return value;
        }
    }
    envFile.close();
    return "";
}
