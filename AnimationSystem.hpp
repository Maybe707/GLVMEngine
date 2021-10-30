#ifndef ANIMATION_SYSTEM
#define ANIMATION_SYSTEM

#include "Event.hpp"
#include "VertexData.hpp"
#include "Stack.hpp"
#include "ISystem.hpp"
#include "Player.hpp"

#define ANIM_PER_AXIS_NUMBER 4

namespace GLVM::ECS
{

	class CAnimation : public ECS::ISystem
	{
		Core::EEvents eSave_Event_;
		bool bFlag_ = true;
		float aVertices_[30];
		static const int anim_index_array = 3;
		int anim_count = 0;
		float* Vertex_Animation[ANIM_PER_AXIS_NUMBER][anim_index_array] =
		{
			{vertices, vertices2, vertices3},
			{vertices4, vertices5, vertices6},
			{vertices7, vertices8, vertices9},
			{vertices10, vertices11, vertices12}
		};
	public:
		void Walk(Core::CStack& _Inputs, double& Animation_Delta, double& Delta_Time, Core::CPlayer& _Player)
		{
//			glBindBuffer(GL_ARRAY_BUFFER, VBO);
//			_Player.SetVertex(aVertices_);
			if(_Inputs.Pop() != Core::eDEFAULT)
				bFlag_ = false;
			if(_Inputs.Pop() == Core::eDEFAULT && bFlag_)
//				_Player.Core::SetVertex(vertices);
			if(_Inputs.Pop() != eSave_Event_)
				Animation_Delta = 31.0f;
			eSave_Event_ = _Inputs.Pop();
			if(_Inputs.Pop() == Core::eMOVE_DOWN)
			{
				Animation_Delta += Delta_Time;
				if(Animation_Delta > 30.0f)
				{
					// if(bFlag_ != false)
					// 	bFlag_ = false;
					switch(anim_count)
					{
					case 0:
						pGLBuffer_Data(GL_ARRAY_BUFFER, sizeof(vertices), Vertex_Animation[0][anim_count], GL_STATIC_DRAW);
						break;
					case 1:
						pGLBuffer_Data(GL_ARRAY_BUFFER, sizeof(vertices), Vertex_Animation[0][anim_count], GL_STATIC_DRAW);
						break;
					case 2:
						pGLBuffer_Data(GL_ARRAY_BUFFER, sizeof(vertices), Vertex_Animation[0][anim_count], GL_STATIC_DRAW);
						break;
					default:
						anim_count = 0;
						break;
					}
					for(int i = 0; i < 30; ++i)
						aVertices_[i] = Vertex_Animation[0][anim_count][i];
					++anim_count;
					Animation_Delta = 0;
//					return;
				}
			}
			else if(_Inputs.Pop() == Core::EEvents::eMOVE_LEFT)
			{
				Animation_Delta += Delta_Time;
				if(Animation_Delta > 30.0f)
				{
					// if(bFlag_ != false)
					// 	bFlag_ = false;
					switch(anim_count)
					{
					case 0:
						pGLBuffer_Data(GL_ARRAY_BUFFER, sizeof(vertices), Vertex_Animation[1][anim_count], GL_STATIC_DRAW);
						break;
					case 1:
						pGLBuffer_Data(GL_ARRAY_BUFFER, sizeof(vertices), Vertex_Animation[1][anim_count], GL_STATIC_DRAW);
						break;
					case 2:
						pGLBuffer_Data(GL_ARRAY_BUFFER, sizeof(vertices), Vertex_Animation[1][anim_count], GL_STATIC_DRAW);
						break;
					default:
						anim_count = 0;
						break;
					}
					for(int i = 0; i < 30; ++i)
						aVertices_[i] = Vertex_Animation[1][anim_count][i];
					++anim_count;
					Animation_Delta = 0;
//					return;
				}
			}
			else if(_Inputs.Pop() == Core::EEvents::eMOVE_RIGHT)
			{
				Animation_Delta += Delta_Time;
				if(Animation_Delta > 30.0f)
				{
					// if(bFlag_ != false)
					// 	bFlag_ = false;
					switch(anim_count)
					{
					case 0:
						pGLBuffer_Data(GL_ARRAY_BUFFER, sizeof(vertices), Vertex_Animation[2][anim_count], GL_STATIC_DRAW);
						break;
					case 1:
						pGLBuffer_Data(GL_ARRAY_BUFFER, sizeof(vertices), Vertex_Animation[2][anim_count], GL_STATIC_DRAW);
						break;
					case 2:
						pGLBuffer_Data(GL_ARRAY_BUFFER, sizeof(vertices), Vertex_Animation[2][anim_count], GL_STATIC_DRAW);
						break;
					default:
						anim_count = 0;
						break;
					}
					for(int i = 0; i < 30; ++i)
						aVertices_[i] = Vertex_Animation[2][anim_count][i];
					++anim_count;
					Animation_Delta = 0;
//					return;
				}
			}
			else if(_Inputs.Pop() == Core::EEvents::eMOVE_UP)
			{
				Animation_Delta += Delta_Time;
				if(Animation_Delta > 30.0f)
				{
					// if(bFlag_ != false)
					// 	bFlag_ = false;
					switch(anim_count)
					{
					case 0:
						pGLBuffer_Data(GL_ARRAY_BUFFER, sizeof(vertices), Vertex_Animation[3][anim_count], GL_STATIC_DRAW);
						break;
					case 1:
						pGLBuffer_Data(GL_ARRAY_BUFFER, sizeof(vertices), Vertex_Animation[3][anim_count], GL_STATIC_DRAW);
						break;
					case 2:
						pGLBuffer_Data(GL_ARRAY_BUFFER, sizeof(vertices), Vertex_Animation[3][anim_count], GL_STATIC_DRAW);
						break;
					default:
						anim_count = 0;
						break;
					}
					for(int i = 0; i < 30; ++i)
					{
						aVertices_[i] = Vertex_Animation[3][anim_count][i];
					}
					++anim_count;
					Animation_Delta = 0;
//					return;
				}
			}

		}
	};

}

#endif
