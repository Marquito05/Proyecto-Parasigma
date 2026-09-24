#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <map>
#include "models.hpp"

bool isalpha_string(std::string s);
bool isdigit_string(std::string s);
std::string toUpperString(std::string s);

// Borra espacios de bordes de string
std::string get_trimmed_string(std::string str);

// Sanitiza el stream luego de usar un 'cin' para que no queden '\n'
void sanitizeStream();

// Función auxiliar para verificar si un string contiene únicamente letras y espacios
bool soloLetrasYEspacios(const std::string &str);

// Limpia los contadores y mapas para la recarga en vivo
void limpiarMemoria(Perfil* &perfiles, int &perfil_size, User* &users, int &user_size, std::map<std::string, int> &nameTable, std::map<int, int> &id_table);

#endif