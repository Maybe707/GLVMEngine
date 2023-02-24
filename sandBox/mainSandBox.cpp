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

template <typename T>
void func(GLVM::core::vector<std::string>& vector) {
	vector.Push(typeid(T).name());
}

class Interface
{
public:
	~Interface() {}

	virtual void func() = 0;
};

class Spawn : public Interface
{
public:
	void func() override;
};

void Spawn::func() {
	std::cout << "Privet" << std::endl;
}

class Spawn2 : public Interface
{
public:
	void func() override;
};

void Spawn2::func() {
	std::cout << "Medved" << std::endl;
}


int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[])
{
	Spawn obj;
	Interface* interface = &obj;
	
	((Spawn2*)interface)->func();



	
//  	namespace ecs = GLVM::ecs;

// 	ecs::CEntityManager* entityManager       = ecs::CEntityManager::GetInstance();
// 	ecs::CComponentManager* componentManager = ecs::CComponentManager::GetInstance();

// 	Entity entity0 = entityManager->CreateEntity();
// 	componentManager->CreateComponent<Component0, Component1, Component2>(entity0);

// 	Entity entity1 = entityManager->CreateEntity();
// 	componentManager->CreateComponent<Component0, Component1, Component2>(entity1);

// 	Entity entity2 = entityManager->CreateEntity();
// 	componentManager->CreateComponent<Component0, Component1, Component2>(entity2);

// 	Entity entity3 = entityManager->CreateEntity();
// 	componentManager->CreateComponent<Component0, Component1, Component2>(entity3);

// //	componentManager->RemoveComponent<Component1>(entity0);
	
// 	std::cout << componentManager->GetComponent<Component0>(entity0).value << std::endl;
// 	std::cout << componentManager->GetComponent<Component1>(entity0).value << std::endl;
// 	std::cout << componentManager->GetComponent<Component2>(entity0).value << std::endl;
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
	GLVM::core::vector<std::string> vector;
	func<int>(vector);

	char symbol = 'a';

	if ( vector[0] == typeid(int).name() )
		std::cout << (int)symbol << std::endl;
	

	
	return 0;
}
