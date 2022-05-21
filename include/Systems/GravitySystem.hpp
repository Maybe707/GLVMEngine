#ifndef GRAVITY_SYSTEM
#define GRAVITY_SYSTEM

#include "Components/ColliderComponent.hpp"
#include "../ComponentManager.hpp"
#include "../Event.hpp"
#include "Components/EventComponent.hpp"
#include "Components/GravityComponent.hpp"
#include "ISystem.hpp"
#include "Components/TransformComponent.hpp"
#include "VectorContainer.hpp"

namespace GLVM::ECS
{

    class CGravitySystem : public ECS::ISystem
    {
    public:
        float fAcceleration_of_Gravity_;

        ///< Check for collision with ground.
        
        bool BoxCollider(STransformComponent& _transform_Component1,
                         STransformComponent& _transform_Component2)
        {
            bool bCollision_Flag = false;
            if(_transform_Component1.tVertex[0] + _transform_Component1.fScale / 2 > _transform_Component2.tVertex[0] - _transform_Component2.fScale / 2 &&
               _transform_Component1.tVertex[0] - _transform_Component1.fScale / 2 < _transform_Component2.tVertex[0] + _transform_Component2.fScale / 2 &&
               _transform_Component1.tVertex[1] + _transform_Component1.fScale / 2 > _transform_Component2.tVertex[1] - _transform_Component2.fScale / 2 &&
               _transform_Component1.tVertex[1] - _transform_Component1.fScale / 2 < _transform_Component2.tVertex[1] + _transform_Component2.fScale / 2 &&
               _transform_Component1.tVertex[2] + _transform_Component1.fScale / 2 > _transform_Component2.tVertex[2] - _transform_Component2.fScale / 2 &&
               _transform_Component1.tVertex[2] - _transform_Component1.fScale / 2 < _transform_Component2.tVertex[2] + _transform_Component2.fScale / 2)
            {
                bCollision_Flag = true;
            }
            return bCollision_Flag;
        }

        ///< Set Y-axis of transform component of backtracking entity to upper Y-axis of ground entity.
        
        void Gravity(STransformComponent& _transform_Component1, STransformComponent& _transform_Component2)
        {
            _transform_Component1.tVertex[1] = _transform_Component2.tVertex[1] + _transform_Component2.fScale / 2 + _transform_Component1.fScale / 2;
            fAcceleration_of_Gravity_ = 0.0f;
        }

        /*! This update searching for refering to colliders entities and check their
         *  transform components for collision, and if collision detected check if
         *  backtracking entity had gravity component for call Gravity function.
         */
         
        void Update(ECS::CComponentManager& _Component_Manager, Core::CEvent& _Event) override
        {
            Core::TCVectorContainer<unsigned int>* pEntity_Container_refCollider =
                ECS::GetInnerIDsContainer<ECS::CColliderComponent>(_Component_Manager);
            unsigned int uiVector_Collider_Size = pEntity_Container_refCollider->GetSize();
            Core::TCVectorContainer<unsigned int>* pEntity_Container_refGravity =
                ECS::GetInnerIDsContainer<ECS::CColliderComponent>(_Component_Manager);
            unsigned int uiVector_Gravity_Size = pEntity_Container_refGravity->GetSize();
            
            for(int i = 0, iSize_External = uiVector_Collider_Size; i < iSize_External; ++i)
            {
                for(int j = 0,iSize_Iner = uiVector_Collider_Size; j < iSize_Iner; ++j)
                {
                    // if(i == 1)                ///< DELETE GOVNO!!
                    //     break;
                    unsigned int iBacktracking_Entity_refCollider = (*pEntity_Container_refCollider)[i];
                    unsigned int iCompared_Entity_refCollider = (*pEntity_Container_refCollider)[j];
                    
                    if(iBacktracking_Entity_refCollider == iCompared_Entity_refCollider)
                        continue;
                    if(BoxCollider(_Component_Manager.GetComponent<ECS::STransformComponent>(iBacktracking_Entity_refCollider),
                                   _Component_Manager.GetComponent<ECS::STransformComponent>(iCompared_Entity_refCollider)))
                        for(int x = 0, iSize_Grav = uiVector_Gravity_Size; x < iSize_Grav; ++x)
                        {
                            unsigned int uiBacktracking_Entity_refGravity = (*pEntity_Container_refGravity)[x];
                            if(iBacktracking_Entity_refCollider == uiBacktracking_Entity_refGravity)
                            {
                                Gravity(_Component_Manager.GetComponent<ECS::STransformComponent>(uiBacktracking_Entity_refGravity),
                                        _Component_Manager.GetComponent<ECS::STransformComponent>(iCompared_Entity_refCollider));
                            }
                        }
                }
            }

            ///< Forcing all entities that had gravity component falling down.
            
            for(int n = 0; n < ECS::GetInnerIDsContainer<ECS::CGravityComponent>(_Component_Manager)->GetSize(); ++n)
            {
                int iEntity_refGravity = (*ECS::GetInnerIDsContainer<ECS::CGravityComponent>(_Component_Manager))[n];
                _Component_Manager.GetComponent<ECS::STransformComponent>(iEntity_refGravity).tVertex[1] -= fAcceleration_of_Gravity_;
            }
        }
    };
}

#endif
