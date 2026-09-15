#include "../include/models.hpp"

#include <map>
#include <iostream>
#include <limits>
#include <cctype>

bool isalpha_string(std::string s) {
    for (char c : s) {
        if (!isalpha((unsigned char)c)) return false;
    }
    return true;
}

bool isdigit_string(std::string s) {
    for (char c : s) {
        if (!isdigit((unsigned char)c)) return false;
    }
    return true;
}


std::string toUpperString(std::string s) {
    for (auto &c : s) {
        c = toupper((unsigned char)c);
    }
    return s;
}

// Borra espacios de bordes de string
std::string get_trimmed_string(std::string str) {
    long unsigned int s = str.find_first_not_of(" \t\n\r");
    long unsigned int e = str.find_last_not_of(" \t\n\r");

    if (s != -1 && e != -1) return str.substr(s, e - s + 1);
    return "";
}

// Sanitiza el stream luego de usar un 'cin' para que no queden '\n'
void sanitizeStream() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

// Función auxiliar para verificar si un string contiene únicamente letras y espacios
bool soloLetrasYEspacios(const std::string &str) {
    if (str.empty()) return false;
    for (unsigned char c : str) {
        if (!isalpha(c) && !isspace(c) && (c < 128)) {
            return false;
        }
    }
    return true;
}

// Limpia los contadores y mapas para la recarga en vivo
void limpiarMemoria(Perfil* &perfiles, int &perfil_size, User* &users, int &user_size, 
                           std::map<std::string, int> &nameTable, std::map<int, int> &id_table) {
    perfil_size = -1;
    user_size = -1;
    nameTable.clear();
    id_table.clear();
}