#ifndef MOVEMENT_SYSTEM
#define MOVEMENT_SYSTEM

#include "Event.hpp"
#include "TransformComponent.hpp"
#include "VectorContainer.hpp"
#include "MoveComponent.hpp"
#include "ComponentManager.hpp"
#include "ISystem.hpp"
#include "VertexMath.hpp"

namespace GLVM::ECS
{
	class CMovementSystem : public ISystem
	{
	public:
		float _dOffset;
		Core::EEvents _Anim_Event;
		void Update(ECS::CComponentManager& _Component_Manager, Core::CEvent& _Event) override
		{
            Vector<float, 3> Front_Camera(0.0f, 0.0f, -1.0f);
            Vector<float, 3> Up_Camera(0.0f, 1.0f, 0.0f);
			Core::TCVectorContainer<STransformComponent>* _pTransform_Components_Container = ECS::GetInnerComponentContainer<ECS::STransformComponent>(_Component_Manager);
			Core::TCVectorContainer<unsigned int>* _pOrdered_Move_Container = ECS::GetInnerIDsContainer<ECS::SMoveComponent>(_Component_Manager);
			Core::TCVectorContainer<SMoveComponent>* _pMove_Components_Container = ECS::GetInnerComponentContainer<ECS::SMoveComponent>(_Component_Manager);
			for(int i = 0; i < _pOrdered_Move_Container->GetSize(); ++i)
			{
				switch(_Anim_Event)
				{
				case Core::EEvents::eMOVE_LEFT:
                    std::cout << "Privet" << std::endl;
					(*_pTransform_Components_Container)[(*_pOrdered_Move_Container)[i]].fPos_X -= Normalize(Cross(Front_Camera, Up_Camera)) *_dOffset;
					(*_pMove_Components_Container)[(*_pOrdered_Move_Container)[i]].eEvent_ = Core::EEvents::eMOVE_LEFT;
					break;
				case Core::EEvents::eMOVE_RIGHT:
                    std::cout << "Privet" << std::endl;
					(*_pTransform_Components_Container)[(*_pOrdered_Move_Container)[i]].fPos_X += _dOffset;
					(*_pMove_Components_Container)[(*_pOrdered_Move_Container)[i]].eEvent_ = Core::EEvents::eMOVE_RIGHT;
					break;
				case Core::EEvents::eMOVE_DOWN:
                    std::cout << "Privet" << std::endl;
					(*_pTransform_Components_Container)[(*_pOrdered_Move_Container)[i]].fPos_Z -= _dOffset;
					(*_pMove_Components_Container)[(*_pOrdered_Move_Container)[i]].eEvent_ = Core::EEvents::eMOVE_DOWN; 
					break;
				case Core::EEvents::eMOVE_UP:
                    std::cout << "Privet" << std::endl;
					(*_pTransform_Components_Container)[(*_pOrdered_Move_Container)[i]].fPos_Z += _dOffset;
					(*_pMove_Components_Container)[(*_pOrdered_Move_Container)[i]].eEvent_ = Core::EEvents::eMOVE_UP;
					break;
				default:
					break;
				}
				
			}
		}
	};
}

#endif
