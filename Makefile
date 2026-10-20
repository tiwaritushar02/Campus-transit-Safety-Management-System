# Makefile for Extended 4-Portal Smart College Bus System
# Standard: C++17

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude -pthread

SRC_DIR = src
INC_DIR = include
OBJ_DIR = obj

# List of source files
SRCS = $(SRC_DIR)/UserAccount.cpp \
       $(SRC_DIR)/AuthManager.cpp \
       $(SRC_DIR)/Student.cpp \
       $(SRC_DIR)/Driver.cpp \
       $(SRC_DIR)/Conductor.cpp \
       $(SRC_DIR)/Admin.cpp \
       $(SRC_DIR)/Parent.cpp \
       $(SRC_DIR)/Stop.cpp \
       $(SRC_DIR)/Route.cpp \
       $(SRC_DIR)/Bus.cpp \
       $(SRC_DIR)/ETAEngine.cpp \
       $(SRC_DIR)/RouteOptimizer.cpp \
       $(SRC_DIR)/ConnectingBusEngine.cpp \
       $(SRC_DIR)/AttendanceLogger.cpp \
       $(SRC_DIR)/QRPassSystem.cpp \
       $(SRC_DIR)/ComplaintEngine.cpp \
       $(SRC_DIR)/MaintenanceManager.cpp \
       $(SRC_DIR)/NotificationCenter.cpp \
       $(SRC_DIR)/FileHandler.cpp \
       $(SRC_DIR)/SystemManager.cpp \
       $(SRC_DIR)/main.cpp

OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))
TARGET = smart_bus_system

all: create_dirs $(TARGET)

create_dirs:
	@mkdir -p $(OBJ_DIR) data

$(TARGET): $(OBJS)
	@echo "🔨 Linking executable $(TARGET)..."
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET)
	@echo "✅ Build complete! Run with ./$(TARGET)"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@echo "⚙️  Compiling $<..."
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: all
	./$(TARGET)

test: all
	./$(TARGET) --test

clean:
	@echo "🧹 Cleaning build artifacts..."
	rm -rf $(OBJ_DIR) $(TARGET)

.PHONY: all run test clean create_dirs
