# Programa que crea el índice invertido con threads (Entrega 3, problema 1)
IDX_DIR = programs/indice_invertido
IDX_SRC = $(IDX_DIR)/src
IDX_INC = $(IDX_DIR)/include
IDX_OUT = $(OUT_DIR)/indice_invertido
IDX_EXEC = $(BIN_DIR)/indice_invertido

# Archivos
IDX_MAIN = $(IDX_SRC)/main.cpp
IDX_OBJECTS = $(IDX_OUT)/config.o $(IDX_OUT)/libros.o $(IDX_OUT)/indice.o
IDX_HEADERS = $(wildcard $(IDX_INC)/*.hpp)

# -pthread es necesario para usar threads y semáforos.
$(IDX_EXEC): $(IDX_OBJECTS) $(OUT_DIR)/env_config.o $(OUT_DIR)/utils.o $(IDX_MAIN) $(IDX_HEADERS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -pthread $(IDX_OBJECTS) $(OUT_DIR)/env_config.o $(OUT_DIR)/utils.o $(IDX_MAIN) -o $@

# Compilar los objetos (se recompilan si cambia algún .hpp, por ejemplo indice.hpp)
$(IDX_OUT)/%.o: $(IDX_SRC)/%.cpp $(IDX_HEADERS) | $(IDX_OUT)
	$(CXX) $(CXXFLAGS) -pthread -c $< -o $@

# Crear directorio de objetos
$(IDX_OUT):
	mkdir -p $(IDX_OUT)
