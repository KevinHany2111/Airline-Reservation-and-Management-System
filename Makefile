# ==============================
# Airline Reservation System Makefile
# ==============================

# Compiler and flags
CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra

# Project directories
SRC_DIR := src
INCLUDE_DIR := include
BUILD_DIR := build
BIN_DIR := bin
TARGET := $(BIN_DIR)/AirlineReservationSystem

# Automatically find all include subdirectories
INC_DIRS := $(shell find $(INCLUDE_DIR) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

# Add include flags to compiler flags
CXXFLAGS += $(INC_FLAGS)

# Find all source files recursively
SRCS := $(shell find $(SRC_DIR) -name '*.cpp')
OBJS := $(SRCS:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

# Create directories automatically
$(shell mkdir -p $(BIN_DIR))
$(shell mkdir -p $(BUILD_DIR))

# Default target
all: $(TARGET)

# Link object files
$(TARGET): $(OBJS)
	@echo "🔗 Linking..."
	$(CXX) $(OBJS) -o $(TARGET)
	@echo "✅ Build complete: $(TARGET)"

# Compile source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@echo "🧩 Compiling $<..."
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Run the program
run: all
	@echo
	@echo "🚀 Running Airline Reservation System..."
	@./$(TARGET)

# Clean build and binary files
clean:
	@echo "🧹 Cleaning..."
	rm -rf $(BUILD_DIR) $(BIN_DIR)
	@echo "🧼 Clean done."

# Rebuild everything
rebuild: clean all

.PHONY: all run clean rebuild
