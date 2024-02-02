# Compiler and flags
CXX := g++ -std=c++14
CFLAGS := -fPIC
LDFLAGS := -lncurses -ldl -lsqlite3 -lGL -lX11 -L./LIB -lAPI
GLFW_FLAGS := -D_GLFW_X11 -lGL -ldl
#INCLUDES := -I./GUI/Dependencies/imgui-1.89.9 -I./GUI/Dependencies/glfw-3.3.8/include
#INCLUDES := -I./GUI/Dependencies/imgui-1.89.9 -I./GUI/Dependencies/glfw-3.3.8/include -I./GUI/Dependencies/nfd/include 
INCLUDES := -I./GUI/Dependencies/imgui-1.89.9 -I./GUI/Dependencies/glfw-3.3.8/include -I./GUI/Dependencies/ImGuiFileDialog
#GTKFLAGS := `pkg-config --cflags --libs gtk+-3.0`


# Directories
API_DIR := ./API
OBJ_DIR := ./OBJ
LIB_DIR := ./LIB
BIN_DIR := ./BIN
GUI_DEP_DIR := ./GUI/Dependencies
GLFW_DIR := $(GUI_DEP_DIR)/glfw*/src
GLFW_OBJ_DIR := $(OBJ_DIR)/GLFW
IMGUI_DIR := $(GUI_DEP_DIR)/imgui*
IMGUI_OBJ_DIR := $(OBJ_DIR)/IMGUI

# Source files
API_SRC := $(wildcard $(API_DIR)/*.cpp)
CONSOLE_SRC := ./Console/Console.cpp
MAIN_SRC := ./CC_Generator/main.cpp
SQLITE_SRC := ./API/Dependencies/sqlite/sqlite3.c
GLFW_SRC := $(shell find $(GLFW_DIR) -type f \( -name 'init.c' -o -name 'x11_init.c' -o -name 'x11_window.c' -o -name 'window.c' -o -name 'input.c' -o -name 'x11_monitor.c' -o -name 'monitor.c' -o -name 'vulkan.c' -o -name 'posix_thread.c' -o -name 'posix_time.c' -o -name 'context.c' -o -name 'linux_joystick.c' -o -name 'osmesa_context.c' -o -name 'egl_context.c' -o -name 'glx_context.c' -o -name 'xkb_unicode.c' \))
IMGUI_SRC := $(shell find $(IMGUI_DIR) -maxdepth 1 -name '*.cpp') $(IMGUI_DIR)/misc/cpp/imgui_stdlib.cpp

# Object files
API_OBJ := $(patsubst $(API_DIR)/%.cpp,$(OBJ_DIR)/API/%.o,$(API_SRC))
CONSOLE_OBJ := $(OBJ_DIR)/Console/Console.o
MAIN_OBJ := $(OBJ_DIR)/main.o
SQLITE_OBJ := $(OBJ_DIR)/sqlite3.o
GLFW_OBJ := $(addprefix ./OBJ/GLFW/,$(notdir $(patsubst %.c,%.o,$(GLFW_SRC))))
IMGUI_OBJ := $(addprefix $(IMGUI_OBJ_DIR)/,$(notdir $(patsubst %.cpp,%.o,$(IMGUI_SRC))))
GUI_OBJ := $(OBJ_DIR)/GUI/GUI.o $(OBJ_DIR)/GUI/LinuxFileDialog.o $(OBJ_DIR)/GUI/ImGuiFileDialog.o 

# Targets
CC_GENERATOR := $(BIN_DIR)/CC_Generator
CONSOLE_SO := $(BIN_DIR)/Console.so
GUI_SO := $(BIN_DIR)/GUI.so
LIB_API := $(LIB_DIR)/libAPI.a

.PHONY: all clean

all: $(CC_GENERATOR) $(GUI_SO) $(CONSOLE_SO)
# all:
# 	@echo $(IMGUI_OBJ)

clean:
	@rm -rf $(LIB_DIR) $(OBJ_DIR) $(BIN_DIR)

# Main
$(CC_GENERATOR): $(MAIN_OBJ) $(LIB_API)
	@mkdir -p $(@D)
	$(CXX) -o $@ $^ $(LDFLAGS) $(INCLUDES)

$(OBJ_DIR)/main.o: $(MAIN_SRC)
	@mkdir -p $(@D)
	$(CXX) -I$(API_DIR) -c -o $@ $<

# API
$(OBJ_DIR)/API/%.o: $(API_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CFLAGS) -I$(API_DIR) -c -o $@ $<

$(LIB_API): $(API_OBJ)
	@mkdir -p $(@D)
	ar rvs $@ $^

$(SQLITE_OBJ): $(SQLITE_SRC)
	@mkdir -p $(@D)
	gcc -c -o $@ $<

# Console
$(OBJ_DIR)/Console/%.o: $(CONSOLE_SRC) $(SQLITE_OBJ)
	@mkdir -p $(@D)
	$(CXX) $(CFLAGS) -I$(API_DIR) -c -o $@ $<

$(CONSOLE_SO): $(CONSOLE_OBJ)
	@mkdir -p $(@D)
	$(CXX) -shared -o $@ $^ $(LDFLAGS)

# GUI
$(GUI_SO): $(GUI_OBJ) $(IMGUI_OBJ) $(GLFW_OBJ)
	@mkdir -p $(@D)
	$(CXX) -shared -o $@ $^ $(LDFLAGS) $(GTKFLAGS) $(INCLUDES)
 
$(GLFW_OBJ_DIR)/%.o: $(GLFW_DIR)/%.c
	@mkdir -p $(@D)
	gcc $(CFLAGS) -c -o $@ $< $(GLFW_FLAGS)

$(IMGUI_OBJ_DIR)/%.o: $(IMGUI_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CFLAGS) -IAPI -c -o $@ $^ $(INCLUDES)

$(IMGUI_OBJ_DIR)/%.o: $(IMGUI_DIR)/misc/cpp/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CFLAGS) -IAPI -c -o $@ $^ $(INCLUDES)

#$(OBJ_DIR)/GUI/%.o: ./GUI/%.cpp $(SQLITE_OBJ)
#	@mkdir -p $(@D)
#	$(CXX) $(CFLAGS) -IAPI -c -o $@ $< $(INCLUDES)

$(OBJ_DIR)/GUI/%.o: ./GUI/%.cpp $(SQLITE_OBJ)
	@mkdir -p $(@D)
	$(CXX) $(CFLAGS) -IAPI -c -o $@ $< $(GTKFLAGS) $(INCLUDES)

$(OBJ_DIR)/GUI/%.o: ./GUI/Dependencies/ImGuiFileDialog/%.cpp $(SQLITE_OBJ)
	@mkdir -p $(@D)
	$(CXX) $(CFLAGS) -IAPI -c -o $@ $< $(GTKFLAGS) $(INCLUDES)