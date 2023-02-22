#include <iostream>
#include "Vector.hpp"
#include "ComponentManager.hpp"
#include "EntityManager.hpp"
#include <vector>

typedef unsigned int Entity;

struct Component0
{
	int value = 100;
};

struct Component1
{
	int value = 200;
};

struct Component2
{
	int value = 300;
	std::string str = "";
};

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[])
{
 	namespace ecs = GLVM::ecs;

	ecs::CEntityManager* entityManager       = ecs::CEntityManager::GetInstance();
	ecs::CComponentManager* componentManager = ecs::CComponentManager::GetInstance();

	Entity entity0 = entityManager->CreateEntity();
	componentManager->CreateComponent<Component0, Component1, Component2>(entity0);

	Entity entity1 = entityManager->CreateEntity();
	componentManager->CreateComponent<Component0, Component1, Component2>(entity1);

	Entity entity2 = entityManager->CreateEntity();
	componentManager->CreateComponent<Component0, Component1, Component2>(entity2);

	Entity entity3 = entityManager->CreateEntity();
	componentManager->CreateComponent<Component0, Component1, Component2>(entity3);

//	componentManager->RemoveComponent<Component1>(entity0);
	
	std::cout << componentManager->GetComponent<Component0>(entity0).value << std::endl;
	std::cout << componentManager->GetComponent<Component1>(entity0).value << std::endl;
	std::cout << componentManager->GetComponent<Component2>(entity0).value << std::endl;
	// std::cout << componentManager->GetComponent<Component2>(entity0).value << std::endl;
	// std::cout << componentManager->GetComponent<Component2>(entity0).value << std::endl;
	// std::cout << componentManager->GetComponent<Component2>(entity0).value << std::endl;

//	componentManager->RemoveComponent<Component1>(entity0);

//	componentManager->RemoveComponent<Component2>(entity2);


	
//	std::cout << componentManager->GetComponent<Component2>(entity0).str << std::endl;
//	std::cout << componentManager->GetComponent<Component2>(entity1).str << std::endl;
//	std::cout << componentManager->GetComponent<Component2>(entity2).str << std::endl;
//	std::cout << componentManager->GetComponent<Component2>(entity3).str << std::endl;

	// Component0 obj0;
	// Component0* ptr0 = &obj0;
	// new(ptr0) Component0();

	// std::cout << ptr0->value << std::endl;

	// Component0 obj1;
	// obj1.value = 10000;
	// ptr0->~Component0();
	// new(ptr0) Component0(obj1);
		
	// std::cout << ptr0->value << std::endl;
	
	return 0;
}
