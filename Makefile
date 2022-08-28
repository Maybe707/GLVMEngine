FLAGS = -std=c++17 -g -Wall -I/home/cyber-demon/.wine/drive_c/VulkanSDK/1.3.224.1/Include -L/home/cyber-demon/.wine/drive_c/VulkanSDK/1.3.224.1/Lib
INC = -I./include
BUILD = build

Vulkan:
	x86_64-w64-mingw32-gcc -c GLPointer.c
	x86_64-w64-mingw32-g++ $(INC) $(FLAGS) -lm -mwindows ./src/Engine.cpp ./src/WinApi/winVk.cpp ./src/EngineMain.cpp GLPointer.o ./src/WindowCreator.cpp ./src/Systems/RenderSystem.cpp ./src/ShaderProgram.cpp ./src/Event.cpp ./src/WinApi/WindowWin.cpp ./src/WinApi/ChronoWin.cpp ./src/TimerCreator.cpp ./src/VertexData.cpp ./src/Systems/CollisionSystem.cpp ./src/Systems/AnimationSystem.cpp ./src/Systems/GUISystem.cpp ./src/Systems/PhysicsSystem.cpp ./src/ComponentManager.cpp ./src/EntityManager.cpp ./src/Systems/MovementSystem.cpp ./src/SystemManager.cpp ./src/chelik.cpp ./src/Systems/ProjectileSystem.cpp ./src/Systems/CameraSystem.cpp ./src/WinApi/SoundEngineWaveform.cpp ./src/SoundEngineFactory.cpp ./src/GraphicAPI/Opengl.cpp ./src/GraphicAPI/Vulkan.cpp -o out.exe -lvulkan-1 -lopengl32 -lwinmm

clean:
	rm -r $(BUILD)/* 
