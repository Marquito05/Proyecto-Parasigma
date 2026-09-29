# Directorios
P_DIR = programs/palindromo
P_SRC = $(P_DIR)/src
P_INC = $(P_DIR)/include
P_OUT = $(OUT_DIR)/palindromo
P_EXEC = $(BIN_DIR)/palindromo

# Archivos
P_MAIN = $(P_SRC)/main.cpp
P_OBJECTS = $(P_OUT)/palindromo.o

# Compilar ejecutable de user_manager
$(P_EXEC): $(P_OBJECTS)  | $(BIN_DIR)
	$(CXX) $(P_OBJECTS) -o $@ $(P_MAIN) -I$(P_INC) -I$(INC_DIR)

# Compilar objetos de user_manager
$(P_OUT)/%.o: $(P_SRC)/%.cpp | $(P_OUT)
	$(CXX) $(CXXFLAGS) -I$(P_INC) -c $< -o $@

# Crear directorio de objetos
$(P_OUT):
	mkdir -p $(P_OUT)