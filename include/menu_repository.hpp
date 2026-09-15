#ifndef MENU_REPO_HPP
#define MENU_REPO_HPP

#include <map>
#include <string>

bool userProfileSearch(const std::string &user_file, const std::string &profile_file, const std::string &user, 
                        const std::string &password, std::map<int, bool> &permList);

void mainMenu(const std::string &user_file, const std::string &profile_file, std::map<int, bool> &perms);

#endif