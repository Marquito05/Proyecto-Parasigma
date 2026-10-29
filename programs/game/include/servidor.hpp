#ifndef G_SERVER_HPP
#define G_SERVER_HPP

#include "models.hpp"
#include <vector>


void broadcast_roster(const std::vector<Jugador>& jugadores, int total_jugadores);

void servidor(const int &num_jugadores, const int &num_por_equipos, const int &tablero_meta, 
    const int &caras_dado, const int &rondas, const int &port);

void crearServidor(const std::string &playerName);

#endif