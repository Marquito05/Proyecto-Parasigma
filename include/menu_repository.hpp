#ifndef MENU_REPO_HPP
#define MENU_REPO_HPP

#include <map>
#include <string>

// Valida usuario y contraseña; si son correctos, carga los permisos y el nombre del perfil
bool userProfileSearch(const std::string &user_file, const std::string &profile_file, const std::string &user, 
                        const std::string &password, std::map<int, bool> &permList, std::string &profileName);

// Menú principal; recibe el usuario y su perfil para enviarlos a los programas que se llaman con system()
void mainMenu(const std::string &user_file, const std::string &profile_file, std::map<int, bool> &perms,
              const std::string &user, const std::string &profile, const std::string &file);

#endif