# Phony targets
.PHONY: all clean run run-user-admin

# Definir compilador y flags
CXX = g++
CXXFLAGS = -std=c++23 -Ofast -Wall -Wextra -Wconversion -Wdouble-promotion -Wduplicated-cond -Wfatal-errors -Wfloat-equal -Wformat=2 -Wlogical-op -Wpedantic -Wshadow -Wundef -Wno-unused-parameter -I$(INC_DIR)

# Directorios
SRC_DIR = src
INC_DIR = include
OUT_DIR = out
BIN_DIR = bin
EXEC = main

# Archivos fuente de programa principal
MAIN = $(SRC_DIR)/main.cpp
SOURCES = $(SRC_DIR)/env_config.cpp $(SRC_DIR)/utils.cpp $(SRC_DIR)/menu_repository.cpp $(SRC_DIR)/program_runner.cpp $(SRC_DIR)/args.cpp

# Archivos objeto programa principal
OBJECTS = $(OUT_DIR)/env_config.o $(OUT_DIR)/utils.o $(OUT_DIR)/menu_repository.o $(OUT_DIR)/program_runner.o $(OUT_DIR)/args.o

# Ejecutable
EXECUTABLE = $(BIN_DIR)/$(EXEC)

# Regla por defecto: compila el menú principal y los programas que llama con system()
all: $(EXECUTABLE) $(BIN_DIR)/user_admin $(BIN_DIR)/multi $(BIN_DIR)/palindromo $(BIN_DIR)/funcion_fx $(BIN_DIR)/conteo

# Incluir submodulos
include programs/user_manager/rules.mk
include programs/matrix_multiplier/rules.mk
include programs/palindromo/rules.mk
include programs/function/rules.mk
include programs/conteo/rules.mk

# Compilar el ejecutable principal enlazando los objetos y main.cpp directamente
# (main.cpp es prerrequisito para que se recompile cuando cambie)
$(EXECUTABLE): $(OBJECTS) $(MAIN) | $(BIN_DIR)
	$(CXX) $(OBJECTS) -o $@ $(MAIN) -I$(INC_DIR)

# Regla para compilar cada archivo .cpp en su correspondiente .o
$(OUT_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OUT_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Crear directorios si no existen
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(OUT_DIR):
	mkdir -p $(OUT_DIR)

# Ejecucion
run: $(EXECUTABLE)
	./$(EXECUTABLE) $(ARGS)

# Limpiar los archivos generados
clean:
	rm -rf $(OUT_DIR) $(BIN_DIR)