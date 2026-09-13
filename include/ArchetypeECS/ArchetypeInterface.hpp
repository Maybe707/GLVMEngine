#ifndef ARCHETYPE_INTERFACE_HPP
#define ARCHETYPE_INTERFACE_HPP

#include "ArchetypeECS/ArchECS_Types.hpp"
#include "Vector.hpp"
#include "typenames.hpp"
#include <array>
#include <memory>
#include <utility>
#include <algorithm>
#include <stdexcept>

namespace GLVM::ecs::arch {
	struct Archetype {
		Archetype() = default;
		Archetype(const Archetype&) = delete;
		Archetype& operator=(const Archetype&) = delete;
		virtual ~Archetype() = default;
		virtual std::unique_ptr<Archetype> cloneEmpty() const = 0;
		
		static constexpr uint32_t CAPACITY = 1024;
		uint32_t capacity = CAPACITY;
		
		entity entities[CAPACITY]{};
		uint32_t entityCount = 0;
		uint32_t componentIds[ComponentsIndices::COMPONENTS_COUNT]{};
		uint32_t componentCount = 0;
		void* components[ComponentsIndices::COMPONENTS_COUNT]{};
		componentMask mask = 0;

		uint32_t addEntity( entity entity_ );
		entity removeEntity( uint32_t index );
    protected:
        template<ComponentsIndices::Types Id, class T, size_t N>
        void registerComponent(T (&data)[N]) {
            static_assert(N > 0);
            static_assert(Id < ComponentsIndices::COMPONENTS_COUNT && Id < 64);
            if (components[Id]) throw std::logic_error("Duplicate archetype component");
            components[Id] = data;
            componentIds[componentCount++] = Id;
            mask |= componentMask{1} << Id;
            capacity = std::min<uint32_t>(capacity, N);
            move_[Id] = [](void* storage, uint32_t to, uint32_t from) {
                auto* values = static_cast<T*>(storage);
                values[to] = std::move(values[from]);
            };
            reset_[Id] = [](void* storage, uint32_t index) { static_cast<T*>(storage)[index] = T{}; };
        }
    private:
        using Move = void (*)(void*, uint32_t, uint32_t);
        using Reset = void (*)(void*, uint32_t);
        std::array<Move, ComponentsIndices::COMPONENTS_COUNT> move_{};
        std::array<Reset, ComponentsIndices::COMPONENTS_COUNT> reset_{};
	};

	struct EntityLocation {
		Archetype* arch = nullptr;
		uint32_t   index = 0;
        struct Cell { u32 x, y, z, slot; };
        core::vector<Cell> gridCells;
        vec3 gridPosition{};
        float gridScale = 0.0f;
        u32 gridMesh = 0;
        bool gridInitialized = false;
		bool isDirty = false; ///< Is entity has been moved or removed
	};
}; // namespace GLVM::ecs::arch

#endif
