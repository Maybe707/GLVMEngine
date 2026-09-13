#pragma once
#include "Components/VertexComponent.hpp"
#include "Vector.hpp"
#include <climits>
#include <vector>

namespace GLVM::ecs::components {
struct inventory {
    inventory(unsigned int rows = 8, unsigned int columns = 8)
        : row(rows), col(columns), slots(rows, std::vector<unsigned int>(columns, UINT_MAX)) {}
    unsigned int row, col;
    std::vector<std::vector<unsigned int>> slots;
    unsigned int entityOwner = UINT_MAX;
    core::vector<unsigned int> highlightedSlots;
    bool isAvailableHighlightedSlots = false;
    MeshHandle slotMeshID{};
    float slotScale = 0;
};
}
