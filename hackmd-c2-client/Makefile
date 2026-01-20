# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -I./src -I./include -g
LDFLAGS = -lcurl

# Directories
SRC_DIR = src
EXAMPLE_DIR = example
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj
TEST_DIR = test

# Source files
SRC_FILES = $(SRC_DIR)/api.cpp $(SRC_DIR)/command.cpp $(SRC_DIR)/c2client.cpp
EXAMPLE_FILES = $(EXAMPLE_DIR)/simple_c2.cpp
TEST_FILES = $(TEST_DIR)/update_note.cpp

# Object files
OBJ_FILES = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/src_%.o,$(SRC_FILES))
EXAMPLE_OBJ = $(patsubst $(EXAMPLE_DIR)/%.cpp,$(OBJ_DIR)/example_%.o,$(EXAMPLE_FILES))
TEST_OBJ = $(patsubst $(TEST_DIR)/%.cpp,$(OBJ_DIR)/test_%.o,$(TEST_FILES))

# Target executables
TARGET = $(BUILD_DIR)/simple_c2
TEST_TARGET = $(BUILD_DIR)/test_update_note

# Default target
all: $(TARGET) $(TEST_TARGET)

# Link the main executable
$(TARGET): $(OBJ_FILES) $(EXAMPLE_OBJ)
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)
	@echo "Build complete: $(TARGET)"

# Link the test executable
$(TEST_TARGET): $(OBJ_FILES) $(TEST_OBJ)
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)
	@echo "Build complete: $(TEST_TARGET)"

# Compile source files
$(OBJ_DIR)/src_%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile example files
$(OBJ_DIR)/example_%.o: $(EXAMPLE_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile test files
$(OBJ_DIR)/test_%.o: $(TEST_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Run the program
run: $(TARGET)
	./$(TARGET)

# Run the test
test: $(TEST_TARGET)
	./$(TEST_TARGET)

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR)

# Rebuild
rebuild: clean all

.PHONY: all run test clean rebuild
