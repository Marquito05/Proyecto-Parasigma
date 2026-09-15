#ifndef PROFILE_REPOSITORY_HPP
#define PROFILE_REPOSITORY_HPP

#include <string>
#include <map>
#include "models.hpp"

void cargarPerfiles(Perfil* &perfiles, int &perfil_size, int &size_arr, std::map<std::string, int> &nameTable, const std::string &PROFILEFILE);
void gestionPerfiles(Perfil* &perfiles, int &perfil_size, User* &users, int &user_size, int &size_arr, std::map<std::string, int> &nameTable, std::map<int, int> &id_table, const std::string &PROFILEFILE, const std::string &USERFILE);

#endif