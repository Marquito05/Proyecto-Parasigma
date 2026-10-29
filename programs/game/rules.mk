G_DIR = programs/game
G_SRC = $(G_DIR)/src
G_INC = $(G_DIR)/include
G_OUT = $(OUT_DIR)/game
G_EXEC = $(BIN_DIR)/game

# Archivos
G_MAIN = $(G_SRC)/main.cpp
G_OBJECTS = $(G_OUT)/cliente.o $(G_OUT)/servidor.o

$(G_EXEC): $(G_OBJECTS) $(OUT_DIR)/utils.o | $(BIN_DIR)
	$(CXX) $(G_OBJECTS) -o $@ $(G_MAIN) -I$(G_INC) -I$(INC_DIR) $(OUT_DIR)/utils.o 

$(G_OUT)/%.o: $(G_SRC)/%.cpp  | $(G_OUT)
	$(CXX) $(CXXFLAGS) -I$(G_INC) -c $< -o $@

$(G_OUT):
	mkdir -p $(G_OUT)
