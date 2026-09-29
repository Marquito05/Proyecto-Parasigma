# Directorios del programa de conteo sobre texto
CO_DIR = programs/conteo
CO_SRC = $(CO_DIR)/src
CO_INC = $(CO_DIR)/include
CO_EXEC = $(BIN_DIR)/conteo

# Archivos (todo el conteo esta en conteo.hpp, asi que solo se compila main.cpp)
CO_MAIN = $(CO_SRC)/main.cpp

# Compilar el ejecutable bin/conteo (se vuelve a compilar si cambia main.cpp o conteo.hpp)
$(CO_EXEC): $(CO_MAIN) $(CO_INC)/conteo.hpp | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -I$(CO_INC) $(CO_MAIN) -o $@
