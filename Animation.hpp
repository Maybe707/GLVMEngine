#ifndef ANIMATION
#define ANIMATION

#include "Event.hpp"
#include "Player.hpp"
#include "VertexData.hpp"
#include "Stack.hpp"

#define ANIM_PER_AXIS_NUMBER 4

namespace GLVM::Core
{

	class CAnimation
	{
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
		void Walk(CStack& _Inputs, double& Animation_Delta, double& Delta_Time, CPlayer& _Player)
		{
//			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			std::cout << _Inputs.Pop() << std::endl;
			_Player.SetVertex(aVertices_);
			if(_Inputs.Pop() == EEvents::eDEFAULT && bFlag_)
			{
				_Player.SetVertex(vertices);
			}
			if(_Inputs.Pop() == EEvents::eMOVE_DOWN)
			{
				if(bFlag_)
					pGLBuffer_Data(GL_ARRAY_BUFFER, sizeof(vertices), Vertex_Animation[0][0], GL_STATIC_DRAW);
				Animation_Delta += Delta_Time;
				if(Animation_Delta > 30.0f)
				{
					bFlag_ = false;
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
			else if(_Inputs.Pop() == EEvents::eMOVE_LEFT)
			{
				if(bFlag_)
					pGLBuffer_Data(GL_ARRAY_BUFFER, sizeof(vertices), Vertex_Animation[1][0], GL_STATIC_DRAW);
				Animation_Delta += Delta_Time;
				if(Animation_Delta > 30.0f)
				{
					bFlag_ = false;
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
			else if(_Inputs.Pop() == EEvents::eMOVE_RIGHT)
			{
				if(bFlag_)
					pGLBuffer_Data(GL_ARRAY_BUFFER, sizeof(vertices), Vertex_Animation[2][0], GL_STATIC_DRAW);
				Animation_Delta += Delta_Time;
				if(Animation_Delta > 30.0f)
				{
					bFlag_ = false;
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
			else if(_Inputs.Pop() == EEvents::eMOVE_UP)
			{
				if(bFlag_)
					pGLBuffer_Data(GL_ARRAY_BUFFER, sizeof(vertices), Vertex_Animation[3][0], GL_STATIC_DRAW);
				Animation_Delta += Delta_Time;
				if(Animation_Delta > 30.0f)
				{
					bFlag_ = false;
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
