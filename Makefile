C = clang
CC = clang++
LDFLAGS = -lGL -lX11
CXXFLAGS = -c -std=c++17 -g -Wall
CFLAGS = -c -g -Wall
INC = -I./include
BUILD1 = build
BUILD2 = build/Systems
BUILD3 = build/Components
BUILD4 = build/UnixApi
SOURCES1 = $(wildcard *.c)
SOURCES2 = $(wildcard src/*.cpp)
SOURCES3 = $(wildcard src/Systems/*.cpp)
SOURCES4 = $(wildcard src/Components/*.cpp)
SOURCES5 = $(wildcard src/UnixApi/*.cpp)
OBJECTS1 = $(SOURCES5:%.c=$(BUILD1)/%.o)
OBJECTS2 = $(SOURCES1:src/%.cpp=$(BUILD1)/%.o)
OBJECTS3 = $(SOURCES2:src/Systems/%.cpp=$(BUILD2)/%.o)
OBJECTS4 = $(SOURCES3:src/Components/%.cpp=$(BUILD3)/%.o)
OBJECTS5 = $(SOURCES4:src/UnixApi/%.cpp=$(BUILD4)/%.o)
EXECUTABLE = glvm

all: $(EXECUTABLE)

$(EXECUTABLE) : $(OBJECTS1) $(OBJECTS2) $(OBJECTS3) $(OBJECTS4) $(OBJECTS5)
	 $(CC) $(INC) $(OBJECTS1) $(OBJECTS2) $(OBJECTS3) $(OBJECTS4) $(OBJECTS5) -o $(BUILD1)/$@

# Make targets for C-files.
$(BUILD1)/%.o : ./src/%.c
	mkdir -p $(@D)
	$(C) $(INC) $(CFLAGS) $< -o $@

$(BUILD2)/%.o : ./src/Systems/%.c
	mkdir -p $(@D)
	$(C) $(INC) $(CFLAGS) $< -o $@

$(BUILD3)/%.o : ./src/Components/%.c
	mkdir -p $(@D)
	$(C) $(INC) $(CFLAGS) $< -o $@

$(BUILD4)/%.o : ./src/UnixApi/%.c
	mkdir -p $(@D)
	$(C) $(INC) $(CFLAGS) $< -o $@

$(BUILD1)/%.o : %.c
	$(C) $(INC) $(CFLAGS) $< -o $@

# Make targets for C++-files.

$(BUILD1)/%.o : ./src/%.cpp
	mkdir -p $(@D)
	$(CC) $(INC) $(CXXFLAGS) $< -o $@

$(BUILD2)/%.o : ./src/Systems/%.cpp
	mkdir -p $(@D)
	$(CC) $(INC) $(CXXFLAGS) $< -o $@

$(BUILD3)/%.o : ./src/Components/%.cpp
	mkdir -p $(@D)
	$(CC) $(INC) $(CXXFLAGS) $< -o $@

$(BUILD4)/%.o : ./src/UnixApi/%.cpp
	mkdir -p $(@D)
	$(CC) $(INC) $(CXXFLAGS) $< -o $@

clean:
	rm -r $(BUILD1)/*
	

