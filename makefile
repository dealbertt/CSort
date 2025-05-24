CXX = g++
CXXFLAGS = -Wall -Werror -std=c++17 -Iinclude $(shell pkg-config --cflags sdl2 SDL2_ttf)
LDFLAGS = $(shell pkg-config --libs sdl2 SDL2_ttf)

SRC_DIR = src
SRC = $(SRC_DIR)/main.cpp $(SRC_DIR)/array.cpp $(SRC_DIR)/sorting.cpp $(SRC_DIR)/sortView.cpp $(SRC_DIR)/config.cpp

OBJ_DIR = obj
OBJ = $(SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

TARGET = CSort

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJ) $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(TARGET)

