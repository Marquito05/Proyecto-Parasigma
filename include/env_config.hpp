#ifndef ENV_CONFIG_HPP
#define ENV_CONFIG_HPP

#include <string>
bool environmentVars(std::string &userFile, std::string &profileFile);

// Retorna el valor de una variable del archivo .env ("" si la variable no existe)
std::string leerVariableEnv(const std::string &nombre);

#endif