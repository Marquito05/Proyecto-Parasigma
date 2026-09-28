# Directorios
UM_DIR = programs/user_manager
UM_SRC = $(UM_DIR)/src
UM_INC = $(UM_DIR)/include
UM_OUT = $(OUT_DIR)/user_manager
UM_EXEC = $(BIN_DIR)/user_admin

# Archivos
UM_MAIN = $(UM_SRC)/main.cpp
UM_OBJECTS = $(UM_OUT)/profile_repository.o $(UM_OUT)/user_repository.o

# Compilar ejecutable de user_manager
$(UM_EXEC): $(UM_OBJECTS) $(OUT_DIR)/utils.o | $(BIN_DIR)
	$(CXX) $(UM_OBJECTS) $(OUT_DIR)/utils.o -o $@ $(UM_MAIN) -I$(UM_INC) -I$(INC_DIR)

# Compilar objetos de user_manager
$(UM_OUT)/%.o: $(UM_SRC)/%.cpp | $(UM_OUT)
	$(CXX) $(CXXFLAGS) -I$(UM_INC) -c $< -o $@

# Crear directorio de objetos
$(UM_OUT):
	mkdir -p $(UM_OUT)