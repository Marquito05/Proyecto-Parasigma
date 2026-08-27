#include "user_repository.h"
#include "profile_repository.h"
#include "utils.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <filesystem>
#include <cctype>

using namespace std;

void cargarUsuarios(User* &users, Perfil* &perfiles, map<string, int> &nameTable, int &user_size,
                    int &size_arr, map<int, int> &id_table, const string &USERFILE) {
    string line;
    ifstream ReadFile(USERFILE);
    user_size = -1;

    if (!ReadFile.is_open()) return;

    while (getline(ReadFile, line)) {
        line = get_trimmed_string(line);
        if (line.empty()) continue; // Ignora líneas vacías

        stringstream st(line);
        string token;
        User dataex;

        // 1. Validar ID: Debe contener SOLO dígitos
        if (!getline(st, token, ',')) continue; 
        token = get_trimmed_string(token);
        if (token.empty() || !all_of(token.begin(), token.end(), ::isdigit)) continue;

        try {
            dataex.id = stoi(token);
            if (dataex.id <= 0) continue; // ID debe ser > 0
        } catch (...) {
            continue; // Si el ID supera el límite numérico o está corrupto, lo salta
        }

        //
        if (!getline(st, dataex.name, ',')) continue; 
        if (!getline(st, dataex.username, ',')) continue;
        if (!getline(st, dataex.password, ',')) continue;
        if (!getline(st, dataex.profile, ',')) continue;

        // Limpieza de espacios
        dataex.name = get_trimmed_string(dataex.name);
        dataex.username = get_trimmed_string(dataex.username);
        dataex.password = get_trimmed_string(dataex.password);
        dataex.profile = toUpperString(get_trimmed_string(dataex.profile));

        // Validar Nombre: SOLO letras y espacios (se lo salta si tiene números o simbolos)
        if (dataex.name.empty() || !soloLetrasYEspacios(dataex.name)) continue;

        // Validar Username y Password: No vacios
        if (dataex.username.empty() || dataex.password.empty()) continue;

        // Validar Perfil: SOLO letras y que exista previamente en la tabla de perfiles
        if (dataex.profile.empty() || !isalpha_string(dataex.profile)) continue;

        if (nameTable.find(dataex.profile) == nameTable.end()) continue; // Se lo salta si el perfil no existe

        // 6. Insertar usuario valido
        user_size++;
        if (user_size >= size_arr) {
            size_arr *= 2;
            User* newUsers = new User[size_arr];
            copy(users, users + user_size, newUsers);
            delete[] users;
            users = newUsers;
        }

        users[user_size] = dataex;
        id_table[dataex.id] = user_size;

        // Vincular ID con la lista de usuarios del perfil
        perfiles[nameTable[dataex.profile]].users_id.push_back(dataex.id);
    }
    ReadFile.close();
}

void gestionUsuarios(User* &users, Perfil* &perfiles, int &user_size, int &perfil_size, int &size_arr,
                     map<int, int> &id_table, map<string, int> &name_table, 
                     const string &USERFILE, const string &PROFILEFILE) {
    int option = -1;
    while (option != 0) {
        cout << "\n0) Regresar \n1) Ingresar usuarios \n2) Listar usuarios \n3) Eliminar usuarios\nIngresar opcion: ";
        cin >> noskipws >> option;
        while (!cin || option <= -1 || option > 3) {
            cout << "\nERROR! Ingresar valor numerico valido.";
            cout << "\n0) Regresar \n1) Ingresar usuarios \n2) Listar usuarios \n3) Eliminar usuarios\nIngresar opcion: ";
            sanitizeStream();
            cin >> noskipws >> option;
        }

        sanitizeStream();

        // INSERTAR USUARIOS
        if (option == 1) {
            limpiarMemoria(perfiles, perfil_size, users, user_size, name_table, id_table);
            cargarPerfiles(perfiles, perfil_size, size_arr, name_table, PROFILEFILE);
            cargarUsuarios(users, perfiles, name_table, user_size, size_arr, id_table, USERFILE);
            User newUser;

            //SOLICITAR ID
            string idInput;
            cout << "id (-1 para cancelar): ";
            getline(cin, idInput);
            idInput = get_trimmed_string(idInput);

            if (idInput == "-1") {
                cout << "Ingreso de usuario cancelado.\n";
                continue;
            }

            bool idValido = false;
            while (!idValido) {
                if (!idInput.empty() && all_of(idInput.begin(), idInput.end(), ::isdigit)) {
                    try {
                        int tempId = stoi(idInput);
                        if (tempId > 0 && id_table.find(tempId) == id_table.end()) {
                            newUser.id = tempId;
                            idValido = true;
                        } else if (tempId <= 0) {
                            cout << "ERROR! Ingresar valor numerico > 0.\nid (-1 para cancelar): ";
                        } else {
                            cout << "ERROR! El ID ingresado ya existe. Ingrese un ID unico.\nid (-1 para cancelar): ";
                        }
                    } catch (const out_of_range &e) {
                        cout << "ERROR! El ID ingresado es demasiado grande (supera el limite).\nid (-1 para cancelar): ";
                    }
                } else {
                    cout << "ERROR! Debe ingresar un numero entero valido > 0.\nid (-1 para cancelar): ";
                }

                if (!idValido) {
                    getline(cin, idInput);
                    idInput = get_trimmed_string(idInput);
                    if (idInput == "-1") break;
                }
            }

            if (idInput == "-1") {
                cout << "Ingreso de usuario cancelado.\n";
                continue;
            }

            // SOLICITAR NOMBRE
            cout << "Nombre (-1 para cancelar): "; 
            getline(cin, newUser.name);
            newUser.name = get_trimmed_string(newUser.name);

            while (newUser.name.empty() || !soloLetrasYEspacios(newUser.name)) {
                if (newUser.name == "-1") break;
                cout << "ERROR! Debe ingresar únicamente letras y espacios (-1 para cancelar): ";
                getline(cin, newUser.name);
                newUser.name = get_trimmed_string(newUser.name);
            }

            if (newUser.name == "-1") {
                cout << "Ingreso de usuario cancelado.\n";
                continue;
            }

            // SOLICITAR USERNAME
            cout << "Nombre de usuario (-1 para cancelar): "; 
            getline(cin, newUser.username);
            newUser.username = get_trimmed_string(newUser.username);

            while (newUser.username.empty()) {
                cout << "ERROR! Ingrese algún carácter (-1 para cancelar): ";
                getline(cin, newUser.username);
                newUser.username = get_trimmed_string(newUser.username);
            }

            if (newUser.username == "-1") {
                cout << "Ingreso de usuario cancelado.\n";
                continue;
            }

            // SOLICITAR CONTRASEÑA
            cout << "Contraseña (espacios se eliminaran, -1 para cancelar): "; 
            getline(cin, newUser.password);
            newUser.password = get_trimmed_string(newUser.password);

            while (newUser.password.empty()) {
                cout << "ERROR! Ingrese algún carácter (-1 para cancelar): ";
                getline(cin, newUser.password);
                newUser.password = get_trimmed_string(newUser.password);
            }

            if (newUser.password == "-1") {
                cout << "Ingreso de usuario cancelado.\n";
                continue;
            }

            // Eliminar espacios de la contraseña
            string::iterator end_pos = remove(newUser.password.begin(), newUser.password.end(), ' ');
            newUser.password.erase(end_pos, newUser.password.end());

            // SOLICITAR PERFIL
            cout << "Perfil (-1 para cancelar): "; 
            getline(cin, newUser.profile);
            newUser.profile = toUpperString(get_trimmed_string(newUser.profile));

            while (name_table.find(newUser.profile) == name_table.end()) {
                if (newUser.profile == "-1") break;
                cout << "ERROR! Ingresar perfil existente (-1 para cancelar): ";
                getline(cin, newUser.profile);
                newUser.profile = toUpperString(get_trimmed_string(newUser.profile));
            }

            if (newUser.profile == "-1") {
                cout << "Ingreso de usuario cancelado.\n";
                continue;
            }

            // GUARDADO DE DATOS
            ofstream File(USERFILE, ios_base::app);
            if (!File.is_open()) {
                cout << "ERROR! No se pudo abrir el archivo para guardar el usuario.\n";
            } else {
                File << newUser.id << "," << newUser.name << "," << newUser.username << "," << newUser.password << "," << newUser.profile << endl;
                File.close();
            }

            user_size++;
            if (user_size >= size_arr) {
                size_arr *= 2;
                User* newUsers = new User[size_arr];
                copy(users, users + user_size, newUsers);
                delete[] users;
                users = newUsers;
            }

            users[user_size] = newUser;
            id_table[newUser.id] = user_size;
            perfiles[name_table[newUser.profile]].users_id.push_back(newUser.id);
            cout << "Usuario creado con éxito.\n";
        }

        // LISTAR USUARIOS
        if (option == 2) {
            limpiarMemoria(perfiles, perfil_size, users, user_size, name_table, id_table);
            cargarPerfiles(perfiles, perfil_size, size_arr, name_table, PROFILEFILE);
            cargarUsuarios(users, perfiles, name_table, user_size, size_arr, id_table, USERFILE);
            cout << "\n";
            cout << setw(10) << left << "id" << right << setw(20) << "name" << setw(28) << "username" << setw(28) << "password" << setw(28) << "profile\n";
            for (int i = 0; i <= user_size; i++) {
                cout << setw(10) << left << users[i].id << right << setw(20) << users[i].name << setw(28) << users[i].username << setw(28) << users[i].password << setw(28) << users[i].profile << "\n";
            }
        }

        // ELIMINAR USUARIOS
        if (option == 3) {
            limpiarMemoria(perfiles, perfil_size, users, user_size, name_table, id_table);
            cargarPerfiles(perfiles, perfil_size, size_arr, name_table, PROFILEFILE);
            cargarUsuarios(users, perfiles, name_table, user_size, size_arr, id_table, USERFILE);

            string idDelInput;
            int id_del = -1;
            cout << "Ingrese id de usuario a eliminar (-1 para regresar): ";
            getline(cin, idDelInput);
            idDelInput = get_trimmed_string(idDelInput);

            while (true) {
                if (idDelInput == "-1") {
                    id_del = -1;
                    break;
                }
                if (!idDelInput.empty() && all_of(idDelInput.begin(), idDelInput.end(), ::isdigit)) {
                    try {
                        int tempId = stoi(idDelInput);
                        if (id_table.find(tempId) != id_table.end()) {
                            id_del = tempId;
                            break;
                        }
                    } catch (const out_of_range &e) {
                        // Si supera el limite numerico, no existe
                    }
                }
                cout << "ERROR! Ingrese un ID de usuario existente (-1 para regresar): ";
                getline(cin, idDelInput);
                idDelInput = get_trimmed_string(idDelInput);
            }

            if (id_del != -1) {
                filesystem::path pu = USERFILE;
                pu = pu.parent_path() / "temp_users.txt";

                ofstream newFileUsers(pu.string());
                ifstream oldFileUsers(USERFILE);
                string line;

                User* newUsers = new User[size_arr];
                int j = 0;

                while (getline(oldFileUsers, line)) {
                    if (line.empty()) continue;

                    size_t commaPos = line.find(',');
                    if (commaPos == string::npos) continue;

                    int currentId = stoi(line.substr(0, commaPos));

                    if (currentId == id_del) {
                        int userIdx = id_table[id_del];
                        string profileName = users[userIdx].profile;
                        int profileIdx = name_table[profileName];

                        auto &uList = perfiles[profileIdx].users_id;
                        uList.erase(remove(uList.begin(), uList.end(), id_del), uList.end());

                        id_table.erase(id_del);
                        user_size--;
                        continue;
                    }
                    newFileUsers << line << "\n";
                    int oldIdx = id_table[currentId];
                    id_table[currentId] = j;
                    newUsers[j] = users[oldIdx];
                    j++;
                }

                delete[] users;
                users = newUsers;

                newFileUsers.close();
                oldFileUsers.close();

                filesystem::path pu2 = USERFILE;
                filesystem::remove(pu2);
                filesystem::rename(pu, pu2);
                cout << "Usuario eliminado con éxito.\n";
            }
        }
    }
}