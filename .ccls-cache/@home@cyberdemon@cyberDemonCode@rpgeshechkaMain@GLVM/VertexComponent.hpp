#ifndef VERTEX_COMPONENT
#define VERTEX_COMPONENT

namespace GLVM::ECS
{
	struct SVertexComponent
	{
		float aVertex_[30] =
		{
			// координаты        // текстурные координаты
			0.5f,  0.5f, 0.0f,   1.0f/3, 1.0f, // верхняя правая вершина
			0.5f, -0.5f, 0.0f,   1.0f/3, 0.75f, // нижняя правая вершина
			-0.5f, -0.5f, 0.0f,   0.0f, 0.75f, // нижняя левая вершина
			-0.5f,  0.5f, 0.0f,   0.0f, 1.0f,  // верхняя левая вершина
			0.5f,  0.5f, 0.0f,   1.0f/3, 1.0f,
			-0.5f, -0.5f, 0.0f,   0.0f, 0.75f
		};
	};
}

#endif
