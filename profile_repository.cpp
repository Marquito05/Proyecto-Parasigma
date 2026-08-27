#include "profile_repository.h"
#include "user_repository.h"
#include "utils.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <filesystem>
#include <cctype>

using namespace std;

void cargarPerfiles(Perfil* &perfiles, int &perfil_size, int &size_arr, map<string, int> &nameTable, const string &PROFILEFILE) {
    string line;
    ifstream ReadFile(PROFILEFILE);

    if (!ReadFile.is_open()) return;

    while (getline(ReadFile, line)) {
        line = get_trimmed_string(line);
        if (line.empty()) continue; // Ignora líneas vacías

        size_t e = line.find(';');
        if (e == string::npos) continue; // Omite líneas corruptas sin ';'

        Perfil dataex;
        dataex.name = toUpperString(get_trimmed_string(line.substr(0, e)));

        // Validar que el nombre del perfil contenga SOLO letras (se salta si tiene números o símbolos)
        if (dataex.name.empty() || !isalpha_string(dataex.name)) continue;

        string dataline = line.substr(e + 1);
        stringstream st(dataline);
        string permToken;
        bool permisosValidos = true;
        vector<int> tempPerms;

        // Validar que cada token de permiso contenga SOLO números
        while (getline(st, permToken, ',')) {
            permToken = get_trimmed_string(permToken);
            if (permToken.empty() || !isdigit_string(permToken)) {
                permisosValidos = false; // Si un permiso tiene letras, se marca todo el perfil como invalido
                break;
            }
            try {
                tempPerms.push_back(stoi(permToken));
            } catch (...) {
                permisosValidos = false;
                break;
            }
        }

        // Si la línea tiene permisos inválidos o está vacía, se salta la línea completa
        if (permisosValidos==false || tempPerms.empty()) continue;

        dataex.perm = tempPerms;
        
        perfil_size++;
        if (perfil_size >= size_arr) {
            size_arr *= 2;
            Perfil* newPerfiles = new Perfil[size_arr];
            copy(perfiles, perfiles + perfil_size, newPerfiles);
            delete[] perfiles;
            perfiles = newPerfiles;
        }

        perfiles[perfil_size] = dataex;
        nameTable[dataex.name] = perfil_size;
    }
    ReadFile.close();
}

void gestionPerfiles(Perfil* &perfiles, int &perfil_size, User* &users, int &user_size,
                     int &size_arr, map<string, int> &nameTable, map<int, int> &id_table,
                     const string &PROFILEFILE, const string &USERFILE) {
    int option = -1;
    while (option != 0) {
        cout << "\n0) Regresar \n1) Ingresar perfil \n2) Listar perfiles \n3) Eliminar perfil\nIngresar opcion: ";
        cin >> noskipws >> option;
        while (!cin || option <= -1 || option > 3) {
            cout << "\nERROR! Ingresar valor numerico valido.";
            cout << "\n0) Regresar \n1) Ingresar perfil \n2) Listar perfiles \n3) Eliminar perfil\nIngresar opcion: ";
            sanitizeStream();
            cin >> noskipws >> option;
        }
        sanitizeStream();

        // 1. INSERTAR PERFIL
        if (option == 1) {
            limpiarMemoria(perfiles, perfil_size, users, user_size, nameTable, id_table);
            cargarPerfiles(perfiles, perfil_size, size_arr, nameTable, PROFILEFILE);
            cargarUsuarios(users, perfiles, nameTable, user_size, size_arr, id_table, USERFILE);

            Perfil newPerfil;
            cout << "Ingresar nombre del perfil (solo letras, '.' para cancelar): ";
            getline(cin, newPerfil.name);
            string nameTrimmed = get_trimmed_string(newPerfil.name);

            if (nameTrimmed == ".") {
                cout << "Ingreso de perfil cancelado.\n";
                continue;
            }

            while (!isalpha_string(nameTrimmed) || nameTable.find(toUpperString(nameTrimmed)) != nameTable.end()) {
                if (nameTrimmed == ".") break;
                cout << "\nERROR! Solo letras (sin numeros/especiales) y no debe existir previamente.\n";
                cout << "Ingrese nombre del perfil ('.' para cancelar): ";
                getline(cin, newPerfil.name);
                nameTrimmed = get_trimmed_string(newPerfil.name);
            }

            if (nameTrimmed == ".") {
                cout << "Ingreso de perfil cancelado.\n";
                continue;
            }

            newPerfil.name = toUpperString(nameTrimmed);
            cout << "Perfil: " << newPerfil.name << "\n";
            cout << "A continuacion, ingrese numeros de permisos (-1 para terminar, '.' para cancelar completamente):\n";

            string permInput;
            bool cancelado = false;

            while (true) {
                cout << ": "; 
                getline(cin, permInput);
                permInput = get_trimmed_string(permInput);

                if (permInput == ".") {
                    cancelado = true;
                    cout << "Ingreso de perfil cancelado. No se guardo nada.\n";
                    break;
                }
                if (permInput == "-1") break;

                bool esNumeroValido = !permInput.empty() && all_of(permInput.begin(), permInput.end(), ::isdigit);
                if (!esNumeroValido) {
                    cout << "ERROR! Ingrese un valor numerico >= 0, -1 para terminar o '.' para cancelar.\n";
                    continue;
                }

                int perms = stoi(permInput);

                if (find(newPerfil.perm.begin(), newPerfil.perm.end(), perms) != newPerfil.perm.end()) {
                    cout << "Este perfil ya tiene ese permiso, ingrese otro que no tenga.\n";
                } else {
                    newPerfil.perm.push_back(perms);
                }
            }    

            if (cancelado) continue;

            if (newPerfil.perm.empty()) {
                cout << "ERROR! Debe ingresar al menos un permiso. Perfil no creado.\n";
                continue;
            }

            ofstream File(PROFILEFILE, ios_base::app);
            if (!File.is_open()) {
                cout << "ERROR! No se pudo abrir el archivo para guardar el perfil.\n";
            } else {
                File << newPerfil.name << ";";
                for (size_t i = 0; i < newPerfil.perm.size(); i++) {
                    File << newPerfil.perm[i] << (i + 1 < newPerfil.perm.size() ? "," : "\n");
                }
                File.close();
            }

            perfil_size++;
            if (perfil_size >= size_arr) {
                size_arr *= 2;
                Perfil* newPerfiles = new Perfil[size_arr];
                copy(perfiles, perfiles + perfil_size, newPerfiles);
                delete[] perfiles;
                perfiles = newPerfiles;
            }
            perfiles[perfil_size] = newPerfil;
            nameTable[newPerfil.name] = perfil_size;
            cout << "Perfil creado con exito.\n";
        }

        // 2. LISTAR PERFILES
        if (option == 2) {
            limpiarMemoria(perfiles, perfil_size, users, user_size, nameTable, id_table);
            cargarPerfiles(perfiles, perfil_size, size_arr, nameTable, PROFILEFILE);
            cargarUsuarios(users, perfiles, nameTable, user_size, size_arr, id_table, USERFILE);
            
            cout << "\n" << setw(15) << left << "Profile Name" << setw(25) << right << "Permissions" << setw(28) << "Related users\n";
            for (int i = 0; i <= perfil_size; i++) {
                cout << setw(15) << left << perfiles[i].name << setw(25) << right << "[";
                for (size_t p = 0; p < perfiles[i].perm.size(); p++) {
                    cout << perfiles[i].perm[p] << (p + 1 < perfiles[i].perm.size() ? "," : "");
                }
                cout << "]" << setw(28) << "[";
                for (size_t u = 0; u < perfiles[i].users_id.size(); u++) {
                    cout << perfiles[i].users_id[u] << (u + 1 < perfiles[i].users_id.size() ? "," : "");
                }
                cout << "]\n";
            }
        }

        // 3. ELIMINAR PERFIL
        if (option == 3) {
            limpiarMemoria(perfiles, perfil_size, users, user_size, nameTable, id_table);
            cargarPerfiles(perfiles, perfil_size, size_arr, nameTable, PROFILEFILE);
            cargarUsuarios(users, perfiles, nameTable, user_size, size_arr, id_table, USERFILE);

            string del_name;
            cout << "Ingrese el nombre del perfil a eliminar ('.' para regresar): ";
            getline(cin, del_name);
            string search_name = toUpperString(get_trimmed_string(del_name));

            while (nameTable.find(search_name) == nameTable.end()) {
                if (get_trimmed_string(del_name) == ".") break;
                cout << "ERROR! Ingresar nombre de perfil existente: ";
                getline(cin, del_name);
                search_name = toUpperString(get_trimmed_string(del_name));
            }

            if (get_trimmed_string(del_name) != ".") {
                filesystem::path p = PROFILEFILE;
                p = p.parent_path() / "temp_profiles.txt";

                ofstream newFile(p.string());
                ifstream oldFile(PROFILEFILE);
                string line;
                Perfil* newProfiles = new Perfil[size_arr];

                // Verificación de usuarios vinculados y borrado en cascada
                if (!perfiles[nameTable[search_name]].users_id.empty()) {
                    cout << "Existen usuarios usando este perfil, si decide eliminar el perfil, todos esos usuarios seran eliminados!\nDesea continuar? (y/n): ";
                    string ans;
                    getline(cin, ans);
                    while (ans != "y" && ans != "n") {
                        cout << "ERROR, ingrese 'y' o 'n': ";
                        getline(cin, ans);
                    }

                    if (ans == "y") {
                        filesystem::path pu = USERFILE;
                        pu = pu.parent_path() / "temp_users.txt";

                        ofstream newFileUsers(pu.string());
                        ifstream oldFileUsers(USERFILE);
                        string line1;
                        User* newUsers = new User[size_arr];

                        int i = 0, j = 0;
                        unsigned int k = 0;
                        auto &users_to_del = perfiles[nameTable[search_name]].users_id;

                        while (getline(oldFileUsers, line1)) {
                            if (k < users_to_del.size() && i == id_table[users_to_del[k]]) {
                                user_size--;
                                id_table.erase(users_to_del[k]);
                                k++;
                            } else {
                                newFileUsers << line1 << "\n";
                                id_table[users[i].id] = j;
                                newUsers[j] = users[i];
                                j++;
                            }
                            i++;
                        }
                        delete[] users;
                        users = newUsers;
                        newFileUsers.close();
                        oldFileUsers.close();

                        filesystem::path pu2 = USERFILE;
                        filesystem::remove(pu2);
                        filesystem::rename(pu, pu2);
                    } else {
                        continue; // Cancela la eliminación
                    }
                }

                // Eliminar del archivo de perfiles
                int i = 0, j = 0;
                int pos_a_eliminar = nameTable[search_name];
                while (getline(oldFile, line)) {
                    if (i == pos_a_eliminar) {
                        i++;
                        continue;
                    }
                    newFile << line << "\n";
                    nameTable[perfiles[i].name] = j;
                    newProfiles[j] = perfiles[i];
                    i++;
                    j++;
                }
                delete[] perfiles;
                perfiles = newProfiles;
                perfil_size--;
                nameTable.erase(search_name);
                newFile.close();
                oldFile.close();

                filesystem::path p_orig = PROFILEFILE;
                filesystem::remove(p_orig);
                filesystem::rename(p, p_orig);
                cout << "Perfil eliminado con exito.\n";
            }
        }
    }
}