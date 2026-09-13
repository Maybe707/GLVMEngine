#pragma once
#include "ISystem.hpp"
#include <memory>
#include <utility>
#include <vector>

namespace GLVM::ecs {
// Owns systems and executes them in registration order. Returned references
// remain valid until destruction, even after more systems are added.
class CSystemManager final : public ISystem {
public:
    template<class T, class... Args>
    T& add(Args&&... args) {
        auto system = std::make_unique<T>(std::forward<Args>(args)...);
        T& result = *system;
        systems_.push_back({std::move(system), true});
        return result;
    }
    void setEnabled(const ISystem& system, bool enabled);
    void Update() override;
private:
    struct Entry { std::unique_ptr<ISystem> system; bool enabled; };
    std::vector<Entry> systems_;
};
}
