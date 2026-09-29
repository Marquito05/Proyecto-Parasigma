#include "../include/env_config.hpp"
#include "../include/utils.hpp"
#include "../include/menu_repository.hpp"

#include <map>
#include <iostream>

using namespace std;

int main(int argc, char* argv[]) {
    //cout << "you have entered " << argc << " arguments: " << '\n';
    string usr, file, pswd, profile;
    map<int, bool> perms;

    
    /*int i=0;
    while (i < argc) {
        cout << "argument " << i+1 << ": " << argv[i] << endl;
        i++;
    }*/

    usr = argv[1];
    pswd = argv[2];
    file = argv[3];

    //cout << "user: " << usr << "\npswd: " << pswd << "\nfile: " << file << '\n';

    string USER_FILE, PROFILE_FILE;
    if (environmentVars(USER_FILE, PROFILE_FILE)) {
        if (userProfileSearch(USER_FILE, PROFILE_FILE, usr, pswd, perms, profile)) {
            mainMenu(USER_FILE, PROFILE_FILE, perms, usr, profile);
        }
    }
}


// g++ -std=c++17 -Wall -o main env_config.h utils.h main.cpp
// ./main user password file


/*
fix admin code:
-remove safeguards to create .env and text files   (DONE)
-add exclusivity to usernames                      (DONE)
-do .h files only                                  (DONE)

fix main code:
-add makefiles                                     (DONE)
-do debugging for whole project, not just one file

*/