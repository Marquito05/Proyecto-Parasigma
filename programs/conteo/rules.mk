# Directorios
CON_DIR = programs/conteo
CON_SRC = $(CON_DIR)/src
CON_INC = $(CON_DIR)/include
CON_OUT = $(OUT_DIR)/conteo
CON_EXEC = $(BIN_DIR)/conteo

# Archivos
CON_MAIN = $(CON_SRC)/main.cpp
CON_OBJECTS = $(CON_OUT)/conteo.o

# Compilar ejecutable de user_manager
$(CON_EXEC): $(CON_OBJECTS)  | $(BIN_DIR)
	$(CXX) $(CON_OBJECTS) -o $@ $(CON_MAIN) -I$(CON_INC) -I$(INC_DIR)

# Compilar objetos de user_manager
$(CON_OUT)/%.o: $(CON_SRC)/%.cpp | $(CON_OUT)
	$(CXX) $(CXXFLAGS) -I$(CON_INC) -c $< -o $@

# Crear directorio de objetos
$(CON_OUT):
	mkdir -p $(CON_OUT)