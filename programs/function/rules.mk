FX_DIR = programs/funcion_fx
FX_SRC = $(FX_DIR)/src
FX_INC = $(FX_DIR)/include
FX_OUT = $(OUT_DIR)/funcion_fx
FX_EXEC = $(BIN_DIR)/funcion_fx

# Archivos del módulo
FX_MAIN = $(FX_SRC)/main.cpp
FX_OBJECTS = $(FX_OUT)/funcion_fx.o

# Regla para compilar el ejecutable de la función
$(FX_EXEC): $(FX_OBJECTS) | $(BIN_DIR)
	$(CXX) $(FX_OBJECTS) -o $@ $(FX_MAIN) -I$(FX_INC) -I$(INC_DIR)

# Regla para compilar los objetos de la carpeta src
$(FX_OUT)/%.o: $(FX_SRC)/%.cpp | $(FX_OUT)
	$(CXX) $(CXXFLAGS) -I$(FX_INC) -c $< -o $@

# Crear la carpeta out específica si no existe
$(FX_OUT):
	mkdir -p $(FX_OUT)