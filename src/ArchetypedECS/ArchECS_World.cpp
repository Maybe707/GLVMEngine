#include "ArchetypeECS/ArchECS_World.hpp"
#include <cmath>
#include <limits>


namespace GLVM::ecs::arch {


	World::World() {
		assert( spatialGrid.width > 0 && spatialGrid.height > 0 && spatialGrid.depth > 0 );
		
		const float chunkSize       = spatialGrid.grid[0][0][0].size;
		const float halfWorldWidth  = spatialGrid.width * chunkSize * 0.5f;
		const float halfWorldHeight = spatialGrid.height * chunkSize * 0.5f;
		const float halfWorldDepth  = spatialGrid.depth * chunkSize * 0.5f;
		const float halfChunkSize   = chunkSize * 0.5f;
		const vec3 pivot = vec3( -halfWorldWidth + halfChunkSize, -halfWorldHeight + halfChunkSize, -halfWorldDepth + halfChunkSize );
		for( u32 i0 = 0; i0 < spatialGrid.depth; ++i0 ) {
			for( u32 i1 = 0; i1 < spatialGrid.height; ++i1 ) {
				for( u32 i2 = 0; i2 < spatialGrid.width; ++i2 ) {
					spatialGrid.grid[i0][i1][i2].position = vec3( i2 * chunkSize, i1 * chunkSize, i0 * chunkSize ) + pivot;
				}
			}
		}
	}
	
	World::~World() {
		for( unsigned int i = 0; i < archetypes.GetSize(); ++i ) {
			delete archetypes[i];
			archetypes[i] = nullptr;
		}
	}
	
	void World::addEntityToArchetype(entity entity_, Archetype* arch) {
        if (!arch || std::find(archetypes.begin(), archetypes.end(), arch) == archetypes.end())
            throw std::invalid_argument("Archetype does not belong to this world");
        id id_ = getId(entity_);
        if (id_ == std::numeric_limits<id>::max()) throw std::out_of_range("Reserved entity ID");

        if (id_ >= entityLocations.GetSize())
            entityLocations.Resize(id_ + 1);

        EntityLocation& location = entityLocations[id_];

        if (location.arch != nullptr) {
            throw std::logic_error("Entity already assigned to archetype");
        }

        if (arch->entityCount >= arch->capacity) {
            auto* available = static_cast<Archetype*>(nullptr);
            for (auto* candidate : archetypes) {
                if (typeid(*candidate) == typeid(*arch) && candidate->entityCount < candidate->capacity) {
                    available = candidate;
                    break;
                }
            }
            if (!available) {
                auto chunk = arch->cloneEmpty();
                available = chunk.get();
                archetypes.Push(available);
                chunk.release();
            }
            arch = available;
        }

        uint32_t index = arch->addEntity(entity_);

        location.arch  = arch;
        location.index = index;
        location.isDirty = true;
    }

	void World::removeEntity(entity entity_) {
        id id_ = getId(entity_);
        if (id_ >= entityLocations.GetSize()) return;
        EntityLocation& location = entityLocations[id_];

        Archetype* arch = location.arch;
        uint32_t index  = location.index;
        if (!arch || index >= arch->entityCount || arch->entities[index] != entity_) return;
        detachSpatial(id_);

        entity moved = arch->removeEntity(index);

        if (moved != entity_) {
            id movedId = getId(moved);
            entityLocations[movedId].index = index;
			entityLocations[movedId].arch  = arch;
        }
        location = {};
        entities.removeEntity(entity_);
    }

    std::optional<SpatialGrid::Range> SpatialGrid::cellRange(vec3 minimum, vec3 maximum) {
        const float half = width * GridChunk::size * 0.5f;
        Range range{};
        u32* low[] = {&range.minX, &range.minY, &range.minZ};
        u32* high[] = {&range.maxX, &range.maxY, &range.maxZ};
        for (unsigned int axis = 0; axis < 3; ++axis) {
            if (!std::isfinite(minimum[axis]) || !std::isfinite(maximum[axis]) ||
                minimum[axis] > maximum[axis] || maximum[axis] < -half || minimum[axis] >= half)
                return std::nullopt;
            const auto index = [half](float position) {
                return static_cast<u32>(std::clamp(std::floor((position + half) / GridChunk::size), 0.0f, float(width - 1)));
            };
            *low[axis] = index(minimum[axis]);
            *high[axis] = index(maximum[axis]);
        }
        return range;
    }
    void World::detachSpatial(id value) {
        if (value >= entityLocations.GetSize()) return;
        auto& location = entityLocations[value];
        for (const auto& membership : location.gridCells) {
            auto& cell = spatialGrid.grid[membership.z][membership.y][membership.x];
            const auto last = cell.entities.GetSize() - 1;
            if (membership.slot != last) {
                const auto moved = cell.entities[last];
                const auto movedMembership = cell.membershipIndices[last];
                cell.entities[membership.slot] = moved;
                cell.membershipIndices[membership.slot] = movedMembership;
                entityLocations[moved].gridCells[movedMembership].slot = membership.slot;
            }
            cell.entities.Pop();
            cell.membershipIndices.Pop();
        }
        location.gridCells.clear();
        location.gridInitialized = false;
    }
    void World::attachSpatial(id value, const SpatialGrid::Range& range) {
        auto& location = entityLocations[value];
        const u32 count = (range.maxX - range.minX + 1) * (range.maxY - range.minY + 1) * (range.maxZ - range.minZ + 1);
        location.gridCells.Reserve(count);
        for (u32 z = range.minZ; z <= range.maxZ; ++z)
            for (u32 y = range.minY; y <= range.maxY; ++y)
                for (u32 x = range.minX; x <= range.maxX; ++x) {
                    auto& cell = spatialGrid.grid[z][y][x];
                    if (cell.entities.GetSize() == cell.entities.GetCapacity()) {
                        const auto capacity = std::max(4u, cell.entities.GetCapacity() * 2);
                        cell.entities.Reserve(capacity);
                        cell.membershipIndices.Reserve(capacity);
                    }
                    const u32 membership = location.gridCells.GetSize();
                    location.gridCells.Push({x, y, z, cell.entities.GetSize()});
                    cell.entities.Push(value);
                    cell.membershipIndices.Push(membership);
                }
    }
    core::vector<Archetype*> World::query(componentMask requiredMask) const {
        core::vector<Archetype*> result;
        uint32_t count = 0;
        searchCacheArchetypes(requiredMask, result, count);
        return result;
    }
    Archetype* World::findArchetype(componentMask requiredMask) const {
        Archetype* empty = nullptr;
        for (auto* chunk : archetypes) {
            if ((chunk->mask & requiredMask) != requiredMask) continue;
            if (chunk->entityCount) return chunk;
            if (!empty) empty = chunk;
        }
        return empty;
    }
    void World::searchCacheArchetypes(componentMask requiredMask, core::vector<Archetype*>& result, uint32_t& count) const {
        result.clear();
        for (auto* chunk : archetypes)
            if ((chunk->mask & requiredMask) == requiredMask) result.Push(chunk);
        count = result.GetSize();
    }
}; // namespace GLVM::ecs::arch
