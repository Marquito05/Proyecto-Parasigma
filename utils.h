#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <map>
#include "models.h"

bool isalpha_string(std::string s);
bool isdigit_string(std::string s);
std::string toUpperString(std::string s);

// Corta espacios vacios de la izquierda y derecha
std::string get_trimmed_string(std::string str);

// Limpia el stream de cin para el proximo input
void sanitizeStream();

// Limpia los contadores y mapas para la recarga en vivo
void limpiarMemoria(Perfil* &perfiles, int &perfil_size, User* &users, int &user_size, 
                           std::map<std::string, int> &nameTable, std::map<int, int> &id_table);

bool soloLetrasYEspacios(const std::string &str);

#endif