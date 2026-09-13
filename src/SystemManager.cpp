#include "SystemManager.hpp"
#include <stdexcept>
namespace GLVM::ecs {
void CSystemManager::setEnabled(const ISystem& system, bool enabled) {
    for (auto& entry : systems_) {
        if (entry.system.get() == &system) {
            entry.enabled = enabled;
            return;
        }
    }
    throw std::invalid_argument("System does not belong to this scheduler");
}
void CSystemManager::Update() {
    for (const auto& entry : systems_)
        if (entry.enabled) entry.system->Update();
}
}
