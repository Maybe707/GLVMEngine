#include "GraphicAPI/Vulkan.hpp"
#include <cassert>

// Linker wrapping exercises the real demo without test hooks in the game loop.
extern "C" void __real__ZN4GLVM4core15CVulkanRenderer4drawEv(GLVM::core::CVulkanRenderer*);
extern "C" void __wrap__ZN4GLVM4core15CVulkanRenderer4drawEv(GLVM::core::CVulkanRenderer* renderer) {
    static unsigned int frame = 0;
    ++frame;
    if (frame == 3 || frame == 9) {
        renderer->window().width = frame == 3 ? 960 : 1280;
        renderer->window().height = frame == 3 ? 540 : 720;
    }
    if (frame == 4 || frame == 7) Input_Stack_.Push(GLVM::core::eINVENTORY);
    if (frame == 11 || frame == 14) Input_Stack_.Push(GLVM::core::eDEBUG_COLLISIONS_ACTIVE);
    if (frame == 5 || frame == 8) {
        assert(renderer->frame().isInventoryOpened == (frame == 5));
        std::cout << "PASS inventory event " << frame << '\n';
    }
    if (frame == 12 || frame == 15)
        assert(renderer->frame().isDebugCollisitionsActive == (frame == 12));
    __real__ZN4GLVM4core15CVulkanRenderer4drawEv(renderer);
    if (frame == 3 || frame == 9) {
        assert(renderer->extent().width == renderer->window().width);
        assert(renderer->extent().height == renderer->window().height);
        std::cout << "PASS resize " << renderer->extent().width << "x" << renderer->extent().height << '\n';
    }
}
