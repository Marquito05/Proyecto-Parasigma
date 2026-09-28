# Directorios del programa multiplicador de matrices
MM_DIR = programs/matrix_multiplier
MM_SRC = $(MM_DIR)/src
MM_INC = $(MM_DIR)/include
MM_OUT = $(OUT_DIR)/matrix_multiplier
MM_EXEC = $(BIN_DIR)/multi

# Archivos
MM_MAIN = $(MM_SRC)/main.cpp
MM_OBJECTS = $(MM_OUT)/matrix.o

# Compilar el ejecutable bin/multi (se vuelve a compilar si cambia main.cpp o matrix.cpp)
$(MM_EXEC): $(MM_OBJECTS) $(MM_MAIN) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -I$(MM_INC) $(MM_OBJECTS) $(MM_MAIN) -o $@

# Compilar los objetos del multiplicador
$(MM_OUT)/%.o: $(MM_SRC)/%.cpp $(MM_INC)/matrix.hpp | $(MM_OUT)
	$(CXX) $(CXXFLAGS) -I$(MM_INC) -c $< -o $@

# Crear directorio de objetos
$(MM_OUT):
	mkdir -p $(MM_OUT)
