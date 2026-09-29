#ifndef PROGRAM_RUNNER_HPP
#define PROGRAM_RUNNER_HPP

#include <string>

// Rodea un argumento con comillas simples para que la shell lo trate como texto literal
std::string protegerArgumento(const std::string &argumento);

// Revisa que un argumento se pueda pasar de forma segura a la shell (no vacío y sin comillas simples)
bool argumentoSeguro(const std::string &argumento);

// Ejecuta un comando con system() y muestra como termino el programa llamado.
// Retorna el codigo de salida del programa (0 = sin errores) o -1 si no se pudo ejecutar.
int ejecutarPrograma(const std::string &comando);

// abre el administrador de usuarios y perfiles
void llamarAdminUsuarios(const std::string &userFile, const std::string &profileFile);

// pide rutas de matrices y el separador, y llama al multiplicador
void llamarMultiplicador(const std::string &usuario, const std::string &perfil);

void llamarPalindrome();
void llamarFx();

#endif
