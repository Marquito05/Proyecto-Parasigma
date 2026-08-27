#ifndef USER_REPOSITORY_H
#define USER_REPOSITORY_H

#include "models.h"
#include <string>
#include <map>

void cargarUsuarios(User* &users, Perfil* &perfiles, std::map<std::string, int> &nameTable, int &user_size,
                    int &size_arr, std::map<int, int> &id_table, const std::string &USERFILE);

void gestionUsuarios(User* &users, Perfil* &perfiles, int &user_size, int &perfil_size, int &size_arr,
                     std::map<int, int> &id_table, std::map<std::string, int> &name_table, 
                     const std::string &USERFILE, const std::string &PROFILEFILE);

#endif