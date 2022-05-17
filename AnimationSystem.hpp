#ifndef ANIMATION_SYSTEM
#define ANIMATION_SYSTEM

#include "ComponentManager.hpp"
#include "Event.hpp"
#include "VectorContainer.hpp"
#include "VertexData.hpp"
#include "Stack.hpp"
#include "ISystem.hpp"
#include "GLPointer.h"
#include "AnimationMoveComponent.hpp"
#include "VertexComponent.hpp"

#define ANIM_PER_AXIS_NUMBER 4

namespace GLVM::ECS
{

	class CAnimationSystem : public ECS::ISystem
	{
	public:
		Core::EEvents eSave_Event_;
		static const int anim_index_array = 3;
		int anim_count = 0;
//		Core::CStack& _Inputs;
		Core::EEvents eEvent_;
		double Animation_Delta;
		double Delta_Time;
		float* Vertex_Animation[ANIM_PER_AXIS_NUMBER][anim_index_array] =
		{
			{vertices, vertices2, vertices3},
			{vertices4, vertices5, vertices6},
			{vertices7, vertices8, vertices9},
			{vertices10, vertices11, vertices12}
		};

        ///< Write one side of cube(if vertex component had vertices for 3D cube).
		void ArrayCopy(float* _aArray_Source, float* _aArray_Destination, unsigned int _u_iRange)
		{
			for(unsigned int i = 0; i < _u_iRange; ++i)
				_aArray_Destination[i] = _aArray_Source[i];
		}
		
		void Update(CComponentManager& _Component_Manager, Core::CEvent& _Event) override
		{
            Core::TCVectorContainer<unsigned int>* pEntity_Container_refAnimationMove =
                ECS::GetInnerIDsContainer<ECS::SAnimationMoveComponent>(_Component_Manager);
            unsigned int uiVector_AnimationMove_Size = pEntity_Container_refAnimationMove->GetSize();

            
			for(int i = 0, iSize = uiVector_AnimationMove_Size; i < iSize; ++i)
			{
                unsigned int uiEntity_refAnimationMove = (*pEntity_Container_refAnimationMove)[i];
                                
				if(eEvent_ != eSave_Event_)
					Animation_Delta = 0.31f;
				eSave_Event_ = eEvent_;
				if(eEvent_ == Core::eMOVE_BACKWARD)
				{
					Animation_Delta += Delta_Time;
					if(Animation_Delta > 0.3f)
					{
						switch(anim_count)
						{
						case 0:
							ArrayCopy(Vertex_Animation[0][anim_count], _Component_Manager.GetComponent<ECS::SVertexComponent>(uiEntity_refAnimationMove).aVertex_, 30);
							break;
						case 1:
							ArrayCopy(Vertex_Animation[0][anim_count], _Component_Manager.GetComponent<ECS::SVertexComponent>(uiEntity_refAnimationMove).aVertex_, 30);
							break;
						case 2:
							ArrayCopy(Vertex_Animation[0][anim_count], _Component_Manager.GetComponent<ECS::SVertexComponent>(uiEntity_refAnimationMove).aVertex_, 30);
							break;
						default:
							break;
						}
						++anim_count;
						if(anim_count == 3)
							anim_count = 0;
						Animation_Delta = 0;
						}
					}
					else if(eEvent_ == Core::EEvents::eMOVE_LEFT)
					{
						Animation_Delta += Delta_Time;
						if(Animation_Delta > 0.3f)
						{
							switch(anim_count)
							{
							case 0:
								ArrayCopy(Vertex_Animation[1][anim_count], _Component_Manager.GetComponent<ECS::SVertexComponent>(uiEntity_refAnimationMove).aVertex_, 30);
								break;
							case 1:
								ArrayCopy(Vertex_Animation[1][anim_count], _Component_Manager.GetComponent<ECS::SVertexComponent>(uiEntity_refAnimationMove).aVertex_, 30);
								break;
							case 2:
								ArrayCopy(Vertex_Animation[1][anim_count], _Component_Manager.GetComponent<ECS::SVertexComponent>(uiEntity_refAnimationMove).aVertex_, 30);
								break;
							default:
								break;
							}
							++anim_count;
							if(anim_count == 3)
								anim_count = 0;
							Animation_Delta = 0;
						}
					}
					else if(eEvent_ == Core::EEvents::eMOVE_RIGHT)
					{
						Animation_Delta += Delta_Time;
						if(Animation_Delta > 0.3f)
						{
							switch(anim_count)
							{
							case 0:
								ArrayCopy(Vertex_Animation[2][anim_count], _Component_Manager.GetComponent<ECS::SVertexComponent>(uiEntity_refAnimationMove).aVertex_, 30);
								break;
							case 1:
								ArrayCopy(Vertex_Animation[2][anim_count], _Component_Manager.GetComponent<ECS::SVertexComponent>(uiEntity_refAnimationMove).aVertex_, 30);
								break;
							case 2:
								ArrayCopy(Vertex_Animation[2][anim_count], _Component_Manager.GetComponent<ECS::SVertexComponent>(uiEntity_refAnimationMove).aVertex_, 30);
								break;
							default:
								break;
							}
							++anim_count;
							if(anim_count == 3)
								anim_count = 0;
							Animation_Delta = 0;
						}
					}
					else if(eEvent_ == Core::EEvents::eMOVE_FORWARD)
					{
						Animation_Delta += Delta_Time;
						if(Animation_Delta > 0.3f)
						{
							switch(anim_count)
							{
							case 0:
								ArrayCopy(Vertex_Animation[3][anim_count], _Component_Manager.GetComponent<ECS::SVertexComponent>(uiEntity_refAnimationMove).aVertex_, 30);
								break;
							case 1:
								ArrayCopy(Vertex_Animation[3][anim_count], _Component_Manager.GetComponent<ECS::SVertexComponent>(uiEntity_refAnimationMove).aVertex_, 30);
								break;
							case 2:
								ArrayCopy(Vertex_Animation[3][anim_count], _Component_Manager.GetComponent<ECS::SVertexComponent>(uiEntity_refAnimationMove).aVertex_, 30);
								break;
							default:
								break;
							}
							++anim_count;
							if(anim_count == 3)
								anim_count = 0;
							Animation_Delta = 0;
						}
					}
				}
			}
		};

	}

#endif
