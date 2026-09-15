#ifndef MODELS_HPP
#define MODELS_HPP

#include <string>
#include <vector>

struct User {
    int id;
    std::string name;
    std::string username;
    std::string password;
    std::string profile;
};

struct Perfil {
    std::string name;
    std::vector<int> perm;
    std::vector<int> users_id;
};

#endif
