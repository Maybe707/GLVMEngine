#include "Engine.hpp"
#include "DemoScene.hpp"
#include <exception>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#ifdef _WIN32
#include <windows.h>
#include <cstdio>
#endif

int main(int argc, char** argv) {
    try {
#ifdef _WIN32
        // Explorer and shortcuts may supply a different working directory.
        wchar_t executable[32768]{};
        const DWORD length = GetModuleFileNameW(nullptr, executable, 32768);
        if (!length || length >= 32768) throw std::runtime_error("Cannot locate the executable");
        std::filesystem::current_path(std::filesystem::path(executable).parent_path());
        std::freopen("GLVMEngine.log", "w", stdout);
        std::freopen("GLVMEngine-error.log", "w", stderr);
#endif
        if (argc != 1) {
            if (argc != 3 || std::strcmp(argv[1], "--frames"))
                throw std::invalid_argument("Usage: GLVMEngine [--frames N]");
#ifdef _WIN32
            _putenv_s("GLVM_MAX_FRAMES", argv[2]);
#else
            setenv("GLVM_MAX_FRAMES", argv[2], 1);
#endif
        }
        GLVM::core::Engine engine;
        GLVM::demo::populateScene(engine);
        engine.GameLoop();
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "GLVM: " << error.what() << '\n';
#ifdef _WIN32
        if (argc == 1 && !std::getenv("GLVM_MAX_FRAMES"))
            MessageBoxA(nullptr, error.what(), "GLVMEngine", MB_OK | MB_ICONERROR);
#endif
        return 1;
    }
}
