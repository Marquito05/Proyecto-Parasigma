#include <cstring>      // strerror
#include <netinet/in.h> // sockaddr, sin, addr, etc
#include <unistd.h>     // close
#include <thread>       // this_thread
#include <fcntl.h>      // F_GETFL, F_SETFL, O_NONBLOCK, etc

#include <fstream>
#include <ctime>


#include "../../../include/utils.hpp"
#include "../include/cliente.hpp"
#include <iostream>
#include <vector>

struct Jugador {
    std::string nombre;
    int socket;
    int posicion;
    int equipo;
};

void write_log(const std::string& jugador, const std::string& equipo, const std::string& accion, const std::string& info = "") {
    std::ofstream fout("game_log.txt", std::ios::app);
    if (!fout.is_open()) return;

    // Get current date and time
    std::time_t now = std::time(nullptr);
    char buf[100];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));

    fout << buf << ";" << jugador << ";" << equipo << ";" << accion;
    if (!info.empty()) {
        fout << ";" << info;
    }
    fout << "\n";
    fout.close();
}


void broadcast_roster(const std::vector<Jugador>& jugadores, int total_jugadores) {
    std::string roster = "\nConnected players (" + std::to_string(jugadores.size()) + "/" + std::to_string(total_jugadores) + "):\n";
    for (const auto& jug : jugadores) {
        roster += "- " + jug.nombre + "\n";
    }

    for (size_t i = 0; i < jugadores.size(); i++) {
        std::string msg = roster;
        if (i == 0) {
            msg += "You are the HOST. Waiting for remaining players to join...\n";
        } else {
            msg += "Waiting for remaining players to join...\n";
        }
        send(jugadores[i].socket, msg.c_str(), msg.length(), MSG_NOSIGNAL);
    }
}



void servidor(const int &num_jugadores, const int &num_por_equipos, const int &tablero_meta, 
    const int &caras_dado, const int &rondas, const int &port) {

    std::ofstream log("game_log.txt", std::ios::out);
    log.close();

    std::cout << "going into server.\n";
    // game conditions
    srand(static_cast<unsigned int>(time(nullptr)));

    int total_jugadores = num_jugadores;
    std::string ganador = "";
    int ronda = 1;

    //server init
    const int puerto = port == 0 ? 9090 : port; //sets port | default port
    const int servidor = socket(AF_INET, SOCK_STREAM, 0);

    std::cout << "puerto: " << puerto << '\n';
    std::cout << "servidor: " << servidor << '\n';

    int reutilizar = 1;
    setsockopt(servidor, SOL_SOCKET, SO_REUSEADDR, &reutilizar, sizeof(reutilizar));
    sockaddr_in direccion{};


    direccion.sin_family = AF_INET;
    direccion.sin_addr.s_addr = htonl(INADDR_ANY);
    direccion.sin_port = htons(static_cast<uint16_t>(puerto));

    //cout << "AF_INET " << AF_INET << endl;
    //cout << "sin_family " <<  direccion.sin_family << endl;
    //cout << "sin_addr.s_addr " << direccion.sin_addr.s_addr << endl;
    //cout << "sin_port " << direccion.sin_port << endl;
    

    if (servidor == -1 
        || bind(servidor, reinterpret_cast<sockaddr*>(&direccion), sizeof(direccion)) == -1 
        || listen(servidor, 8) == -1) {
        std::cerr << "error al escuchar: " << strerror(errno) << '\n';
        return;
    }


    std::cout << "Mostrador esperando una conexión en " << puerto << "...\n";
    std::cout << "total_jugadores: " << total_jugadores << '\n';

    std::vector<Jugador> jugadores;
    int num=0;
    
    int flags = fcntl(servidor, F_GETFL, 0);
    fcntl(servidor, F_SETFL, flags | O_NONBLOCK);
    std::cout << "[SERVER] Lobby open. Players necessary: " << num_jugadores << '\n';
    bool state_changed;

    while (jugadores.size() < static_cast<size_t>(total_jugadores)) {
        state_changed = false;
        const int cliente = accept(servidor, nullptr, nullptr);
        if (cliente != -1) {
            char nameBuf[256]{};
            std::string clientName = std::to_string(num+1);
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            ssize_t n = recv(cliente, nameBuf, sizeof(nameBuf) - 1, MSG_DONTWAIT);
            
            if (n > 0) {
                nameBuf[n] = '\0';
                std::string raw_name(nameBuf);
                size_t nl = raw_name.find('\n');
                if (nl != std::string::npos) {
                    clientName = raw_name.substr(0, nl);
                } else {
                    clientName = raw_name;
                }
            }


            // CHECK FOR DUPLICATE NAMES
            bool already_connected = false;
            for (const auto& existing : jugadores) {
                if (existing.nombre == clientName) {
                    already_connected = true;
                    break;
                }
            }

            if (already_connected) {
                std::string error_msg = "ERROR_DUPLICATE_USER\n";
                send(cliente, error_msg.c_str(), error_msg.length(), MSG_NOSIGNAL);
                close(cliente);
                std::cout << "[SERVER] Rejected connection attempt: User '" << clientName << "' is already connected.\n";
                continue;
            }

            Jugador newPlayer;
            newPlayer.socket = cliente;
            newPlayer.posicion = 0;
            newPlayer.equipo = 0;
            newPlayer.nombre = clientName;
            jugadores.push_back(newPlayer);

            std::cout << "[SERVER] Player " << newPlayer.nombre << " connected (Socket " << cliente << ")\n";
            write_log(newPlayer.nombre, "SinAsignar", "CONECTA");

            std::string msg = newPlayer.nombre + '\n';
            send(cliente, msg.c_str(), msg.length(), MSG_NOSIGNAL);
            num++;
            state_changed = true;
        }

        //monitor disconnects
        for (auto it=jugadores.begin(); it != jugadores.end();) {
            char checkBuf;
            ssize_t res = recv(it->socket, &checkBuf, 1, MSG_PEEK | MSG_DONTWAIT);
            if (res == 0 || (res < 0 && errno != EAGAIN && errno != EWOULDBLOCK)) {
                std::cout << "[SERVER] " << it->nombre << " Disconnected from lobby.\n";
                close(it->socket);
                it = jugadores.erase(it);
                state_changed = true;

                std::string eq_str = it->equipo > 0 ? "Equipo" + std::to_string(it->equipo) : "SinAsignar";
                write_log(it->nombre, eq_str, "DESCONECTA");
            } else {
                ++it;
            }
        }

        if (state_changed && !jugadores.empty()) {
            broadcast_roster(jugadores, total_jugadores);  
        }    
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    fcntl(servidor, F_SETFL, flags);
    std::cout << "[SERVER] Lobby full! Starting match...\n";



    // assign to teams
    std::vector<std::vector<Jugador>> equipos;
    int current_team_id = 1;
    std::vector<Jugador> current_team_bucket;
    for (size_t i = 0; i < jugadores.size(); i++) {
        jugadores[i].equipo = current_team_id;
        current_team_bucket.push_back(jugadores[i]);

        write_log(jugadores[i].nombre, "Equipo" + std::to_string(current_team_id), "INCORPORA_EQUIPO");

        if (current_team_bucket.size() == static_cast<size_t>(num_por_equipos) || i == jugadores.size() - 1) {
            equipos.push_back(current_team_bucket);
            current_team_bucket.clear();
            current_team_id++;
        }
    }

    std::vector<int> puntajes_equipos(equipos.size(), 0);

    write_log("Servidor", "Sistema", "INICIO_PARTIDA", "Partida iniciada con " + std::to_string(jugadores.size()) + " jugadores en " + std::to_string(equipos.size()) + " equipos.");

    // Broadcast team composition to every player
    for (size_t t = 0; t < equipos.size(); t++) {
        std::string team_info = "\n--- CONFIRMACIÓN DE EQUIPO ---\nPerteneces al Equipo " + std::to_string(t + 1) + "\nIntegrantes:\n";
        for (const auto& member : equipos[t]) {
            team_info += "- Jugador " + member.nombre + "\n";
        }
        team_info += "-------------------------------\n\n";

        for (const auto& member : equipos[t]) {
            send(member.socket, team_info.c_str(), team_info.length(), MSG_NOSIGNAL);
        }
    }


    total_jugadores = num;

    char buffer[256];
    int cycle = 0;
    std::string message, msg;

    message = "\nLlendo a ronda " + std::to_string(ronda) + " de " + std::to_string(rondas) + "\n.";
    for (Jugador &jug : jugadores) send(jug.socket, message.c_str(), message.length(), MSG_NOSIGNAL);

    size_t team_idx=0;
    size_t team_number;
    int team_points, lanzamiento;

    // SERVER LOOP
    while (true) {
        if (equipos.empty()) {
            std::cout << "[SERVER] Closing..." << '\n';
            write_log("Servidor", "Sistema", "TERMINO_PARTIDA", "Sin equipos restantes.");
            close(servidor);
            break;
        }

        team_number = team_idx+1;
        team_points=0;
        std::string equipo_nombre = "Equipo" + std::to_string(team_number);

        write_log("Servidor", equipo_nombre, "CAMBIO_TURNO", "Inicia turno del Equipo " + std::to_string(team_number));

        message = "TURNO DEL EQUIPO " + std::to_string(team_number) + " (Puntaje actual: " + std::to_string(puntajes_equipos[team_idx]) + "/" + std::to_string(tablero_meta) + ")\n";
        for (const auto& t_list : equipos) {
            for (const auto& member : t_list) {
                send(member.socket, message.c_str(), message.length(), MSG_NOSIGNAL);
            }
        }

        for (size_t p=0; p<equipos[team_idx].size(); p++) {
            Jugador &active_player = equipos[team_idx][p];
            for (size_t member=0; member < equipos[team_idx].size(); member++) {
                message = "\nTurno de jugador " + active_player.nombre + " (Equipo " + std::to_string(team_number) + " - Puntos: " + std::to_string(puntajes_equipos[team_idx]) 
                + "/" + std::to_string(tablero_meta) + ")\n";

                if (member==p) {
                    msg = message + "YOUR_TURN\n";
                } else {
                    msg = message + "WAIT_TURN\n";
                }
                send(equipos[team_idx][member].socket, msg.c_str(), msg.length(), MSG_NOSIGNAL);
            }
        
            // check in case of disconnect
            const ssize_t n = recv(active_player.socket, buffer, sizeof(buffer)-1, 0);
            if (n <= 0) {
                std::cout << "[SERVER] " << active_player.nombre << " disconnected during turn." << '\n';
                write_log(active_player.nombre, equipo_nombre, "DESCONECTA");
                close(active_player.socket);
                equipos[team_idx].erase(equipos[team_idx].begin() + p);
                p--;

                if (equipos[team_idx].empty()) {
                    std::cout << "[SERVER] Equipo " << team_number << " has no remaining players. Removing team...\n";
                    write_log("Servidor", equipo_nombre, "MODIFICACION_ESTADO", "Equipo " + std::to_string(team_number) + " eliminado por desconexión total.");
                    equipos.erase(equipos.begin() + team_idx);
                    puntajes_equipos.erase(puntajes_equipos.begin() + team_idx);
                    if (equipos.empty()) break;
                    if (team_idx >= equipos.size()) team_idx = 0;
                    break;
                }
                continue;
            }

            buffer[n] = '\0';
            lanzamiento = (rand() % caras_dado) + 1;
            team_points += lanzamiento;
            puntajes_equipos[team_idx] += lanzamiento;

            write_log(active_player.nombre, equipo_nombre, "LANZA_DADO", std::to_string(lanzamiento));
            write_log(active_player.nombre, equipo_nombre, "AVANZA", std::to_string(lanzamiento) + ";" + std::to_string(puntajes_equipos[team_idx]));

            std::string roll_notice = "¡Jugador " + active_player.nombre + " lanzó el dado y cayó en: " + std::to_string(lanzamiento) + "!\n";
            for (const auto& member : equipos[team_idx]) {
                send(member.socket, roll_notice.c_str(), roll_notice.length(), MSG_NOSIGNAL);
            }
        }

        if (equipos.empty()) break;


        // Mensaje global
        std::string team_summary = "[RESUMEN DE TURNO] El Equipo " + std::to_string(team_number) + 
                             " sumo " + std::to_string(team_points) + " puntos en este turno." +
                             "\nPuntaje Total del Equipo " + std::to_string(team_number) + ": " + 
                             std::to_string(puntajes_equipos[team_idx]) + "/" + std::to_string(tablero_meta) + "\n";

        write_log("Servidor", equipo_nombre, "MODIFICACION_ESTADO", "Suma " + std::to_string(team_points) + " puntos. Total: " + std::to_string(puntajes_equipos[team_idx]) + "/" + std::to_string(tablero_meta));
        for (const auto& t_list : equipos) {
            for (const auto& member : t_list) {
                send(member.socket, team_summary.c_str(), team_summary.length(), MSG_NOSIGNAL);
            }
        }


        // Winning condition
        if (puntajes_equipos[team_idx] >= tablero_meta) {
            for (const auto& member : equipos[team_idx])
                write_log(member.nombre, equipo_nombre, "GANA");

            ronda++;

            if (ronda > rondas) {
                message = "\n¡FIN DE RONDA! EL GANADOR ES EL EQUIPO " + std::to_string(team_number) + 
                          "\n¡FIN DEL JUEGO! Cerrando conexión...\n\n";

                write_log("Servidor", equipo_nombre, "TERMINO_PARTIDA", "Juego finalizado. Ganador definitivo: Equipo " + std::to_string(team_number));

                for (const auto& t_list : equipos) {
                    for (const auto& member : t_list) {
                        send(member.socket, message.c_str(), message.length(), MSG_NOSIGNAL);
                        close(member.socket);
                    }
                }
                close(servidor);
                break;              
            } else {
                fill(puntajes_equipos.begin(), puntajes_equipos.end(), 0);
                message = "\n¡FIN DE RONDA! EL GANADOR ES EL EQUIPO " + std::to_string(team_number) + 
                          "\nLlendo a ronda " + std::to_string(ronda) + " de " + std::to_string(rondas) + "\n\n";
                
                write_log("Servidor", "Sistema", "MODIFICACION_ESTADO", "Inicia ronda " + std::to_string(ronda));
                for (const auto& t_list : equipos) {
                    for (const auto& member : t_list) {
                        send(member.socket, message.c_str(), message.length(), MSG_NOSIGNAL);
                    }
                }
                team_idx = 0;
                continue;
            }
        }

        if (team_idx >= equipos.size()-1) team_idx=0;
        else team_idx++;
    }

}


void crearServidor(const std::string &playerName) {
    // game conditions
    int num_jugadores, num_por_equipo, tablero_meta, caras_dado, rondas, port;
    std::string IP; 

    std::string input_str;

    std::cout << "Ingrese cantidad de jugadores: ";
    std::getline(std::cin, input_str);
    input_str = get_trimmed_string(input_str);
    while (input_str.empty() || std::stoi(input_str) <= 0) {
        std::cout << "\nCantidad no válida de jugadores! (>0)";
        std::cout << "\nIngrese cantidad de jugadores: ";
        std::getline(std::cin, input_str);
        input_str = get_trimmed_string(input_str);
    }
    num_jugadores = std::stoi(input_str);


    std::cout << "\nIngrese cantidad de jugadores por equipo: ";
    std::getline(std::cin, input_str);
    input_str = get_trimmed_string(input_str);
    while (input_str.empty() || std::stoi(input_str) < 0) {
        std::cout << "\nCantidad no válida. (>=0)";
        std::cout << "\nIngrese cantidad de jugadores por equipo: ";
        std::getline(std::cin, input_str);
        input_str = get_trimmed_string(input_str);
    }
    num_por_equipo = std::stoi(input_str);


    std::cout << "Ingrese el tamaño del tablero (meta a alcanzar): ";
    std::getline(std::cin, input_str);
    input_str = get_trimmed_string(input_str);
    while (input_str.empty() || std::stoi(input_str) <= 0) {
        std::cout << "\nCantidad no válida de meta! (>0)";
        std::cout << "\nIngrese el tamaño del tablero (meta a alcanzar): ";
        std::getline(std::cin, input_str);
        input_str = get_trimmed_string(input_str);
    }
    tablero_meta = std::stoi(input_str);

    std::cout << "Ingrese el número de caras del dado: ";
    std::getline(std::cin, input_str);
    input_str = get_trimmed_string(input_str);
    while (input_str.empty() || std::stoi(input_str) <= 0) {
        std::cout << "\nCantidad no válida de caras! (>0)";
        std::cout << "\nIngrese el número de caras del dado: ";
        std::getline(std::cin, input_str);
        input_str = get_trimmed_string(input_str);
    }
    caras_dado = std::stoi(input_str);


    std::cout << "Ingrese el número de rondas: ";
    std::getline(std::cin, input_str);
    input_str = get_trimmed_string(input_str);
    while (input_str.empty() || std::stoi(input_str) <= 0) {
        std::cout << "\nCantidad no válida de rondas! (>0)";
        std::cout << "\nIngrese el número de rondas: ";
        std::getline(std::cin, input_str);
        input_str = get_trimmed_string(input_str);
    }
    rondas = std::stoi(input_str);


    std::cout << "Ingrese el puerto ('0' para puerto predeterminado): ";
    std::getline(std::cin, input_str);
    input_str = get_trimmed_string(input_str);
    while (input_str.empty() || std::stoi(input_str) < 0) {
        std::cout << "\nPuerto no válido!" 
                  << "\nIngrese el puerto ('0' para puerto predeterminado): ";
        std::getline(std::cin, input_str);
        input_str = get_trimmed_string(input_str);
    }
    port = std::stoi(input_str);


    std::cout << "Ingrese la IP: ";
    std::getline(std::cin, IP);
    IP = get_trimmed_string(IP);
    while (IP.empty()) {
        std::cout << "ERROR! Ingrese caracteres válidos: ";
        std::getline(std::cin, IP);
        IP = get_trimmed_string(IP);
    }


    std::thread serverThread(servidor, num_jugadores, num_por_equipo, tablero_meta, caras_dado, rondas, port);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    cliente(IP, port, playerName);

    if (serverThread.joinable()) serverThread.join();

}
