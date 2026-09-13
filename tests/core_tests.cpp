#include "Vector.hpp"
#include "ArchetypeECS/ArchetypeEntityManager.hpp"
#include "ThreadPool.hpp"
#include <cassert>
#include <chrono>
#include <cstring>
#include <string>

struct Tracked {
    static inline int alive = 0;
    int value = 0;
    Tracked() { ++alive; }
    Tracked(const Tracked& other) : value(other.value) { ++alive; }
    ~Tracked() { --alive; }
};

int main(int argc, char** argv) {
    const std::string test = argc > 1 ? argv[1] : "";
    if (test == "remove") {
        GLVM::core::vector<Tracked> values;
        values.Resize(3);
        values.Remove(1);
        assert(Tracked::alive == 2);
        values.Remove(99);
        assert(values.GetSize() == 2);
        values.RemoveFirstItem();
        values.RemoveFirstItem();
        values.RemoveFirstItem();
        assert(values.empty() && Tracked::alive == 0);
    } else if (test == "reuse") {
        GLVM::core::vector<int> values;
        values.Resize(4);
        values.clear();
        values.Resize(3);
        assert(values[2] == 0);
        values.Push(7);
        auto copy = values;
        copy[0] = 5;
        assert(values[0] == 0 && copy.GetHead() == 7);
    } else if (test == "iterator") {
        GLVM::core::vector<int> values;
        int wanted = 3;
        assert(!values.Find(wanted).ValidStatus());
        values.Push(1);
        assert(!values.Find(wanted).ValidStatus());
        values.Push(3);
        auto found = values.Find(wanted);
        assert(found.ValidStatus() && found.Current() == 3);
        assert(!found.Next());
    } else if (test == "string") {
        GLVM::core::vector<char> text;
        assert(!(text == "x"));
        assert(text == "");
        text.Push('a');
        assert(text == "a");
        assert(!(text == "ab"));
        assert(!(text == ""));
        text.Push('\0');
        assert(text == "a");
    } else if (test == "entity") {
        namespace arch = GLVM::ecs::arch;
        auto* manager = arch::ArchetypeEntityManager::getInstance();
        assert(!manager->isAlive(arch::makeEntity(1000000, 1)));
        auto first = manager->createEntity();
        assert(manager->isAlive(first));
        manager->removeEntity(first);
        assert(!manager->isAlive(first));
        manager->removeEntity(first);
        auto second = manager->createEntity();
        assert(arch::getId(first) == arch::getId(second));
        assert(first != second && manager->isAlive(second));
    } else if (test == "thread") {
        ThreadPool pool(0);
        auto result = pool.enqueue([] { return 42; });
        assert(result.wait_for(std::chrono::seconds(2)) == std::future_status::ready);
        assert(result.get() == 42);
        auto exception = pool.enqueue([]() -> int { throw std::runtime_error("test"); });
        try { exception.get(); assert(false); } catch (const std::runtime_error&) {}
    } else {
        return 2;
    }
}
