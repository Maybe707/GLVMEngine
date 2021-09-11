ifeq ($(OS),Windows_NT)
    uname_S := Windows
else
    uname_S := $(shell uname -s)
endif

ifeq ($(uname_S), Windows)
    rpg = rpgWin.exe
	CC = x86_64-w64-mingw32-g++
	LDFLAGS = -lgdi32 -lopengl32
	WINDOWO = WindowWin.o
	WINDOWS = WindowWin.cpp
endif
ifeq ($(uname_S), Linux)
    rpg = rpgLin
	CC = g++
	LDFLAGS = -lGL -lX11
	WINDOWO = WindowLin.o
	WINDOWS = WindowLin.cpp
endif

all: $(rpg)

EngineMain.o: EngineMain.cpp
	$(CC) -c -o EngineMain.o EngineMain.cpp
GLPointer.o: GLPointer.c
	$(CC) -c -o GLPointer.o GLPointer.c
WindowCreator.o: WindowCreator.cpp
	$(CC) -c -o WindowCreator.o WindowCreator.cpp
Sprite.o: Sprite.cpp
	$(CC) -c -o Sprite.o Sprite.cpp
Renderer.o: Renderer.cpp
	$(CC) -c -o Renderer.o Renderer.cpp
ShaderProgram.o: ShaderProgram.cpp
	$(CC) -c -o ShaderProgram.o ShaderProgram.cpp
Event.o: Event.cpp
	$(CC) -c -o Event.o Event.cpp
$(WINDOWO): $(WINDOWS)
	$(CC) -c -o $(WINDOWO) $(WINDOWS)
$(rpg): EngineMain.o GLPointer.o WindowCreator.o Sprite.o Renderer.o ShaderProgram.o Event.o $(WINDOWO)
	$(CC) -o $(rpg) EngineMain.o GLPointer.o WindowCreator.o Sprite.o Renderer.o ShaderProgram.o Event.o $(WINDOWO) $(LDFLAGS)

clean:
	rm -rf *.o $(rpg)
