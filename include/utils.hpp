#ifndef UTILS_HPP
#define UTILS_HPP

#include <map>
#include <string>

struct Perfil;
struct User;

bool isalpha_string(std::string s);
bool isdigit_string(std::string s);
std::string toUpperString(std::string s);
std::string get_trimmed_string(std::string str);
void sanitizeStream();
bool soloLetrasYEspacios(const std::string &str);

void limpiarMemoria(Perfil* &perfiles, int &perfil_size, User* &users, int &user_size, 
                           std::map<std::string, int> &nameTable, std::map<int, int> &id_table);


#endif