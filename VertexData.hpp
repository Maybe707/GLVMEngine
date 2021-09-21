
const int kVertex_Size = 9;
const int kVertex_Size2 = 30;
float aVertices[kVertex_Size] =
{
    -0.5f, -0.5f, 0.5f, ///< Left vertex
    -0.5f,  0.5f, 0.0f, ///< Right vertex
     0.0f,  0.0f, 0.0f  ///< Upper vertex   
};

float aVertices2[kVertex_Size] = {
    0.5f, -0.5f, -0.5f, ///< Left vertex
    0.5f, 0.5f,  0.5f,  ///< Right vertex
    0.0f, 0.0f,  -1.0f  ///< Upper vertex
};

float vertices[kVertex_Size2] =
{
	// координаты        // текстурные координаты
	0.5f,  0.5f, 0.0f,   0.33f, 1.0f, // верхняя правая вершина
	0.5f, -0.5f, 0.0f,   0.33f, 0.75f, // нижняя правая вершина
	-0.5f, -0.5f, 0.0f,   0.0f, 0.75f, // нижняя левая вершина
	-0.5f,  0.5f, 0.0f,   0.0f, 1.0f,  // верхняя левая вершина
	0.5f,  0.5f, 0.0f,   0.33f, 1.0f,
	-0.5f, -0.5f, 0.0f,   0.0f, 0.75f
};

float vertices2[kVertex_Size2] =
{
	// координаты        // текстурные координаты
	0.5f,  0.5f, 0.0f,   0.67f, 1.0f, // верхняя правая вершина
	0.5f, -0.5f, 0.0f,   0.67f, 0.75f, // нижняя правая вершина
	-0.5f, -0.5f, 0.0f,   0.33f, 0.75f, // нижняя левая вершина
	-0.5f,  0.5f, 0.0f,   0.33f, 1.0f,  // верхняя левая вершина
	0.5f,  0.5f, 0.0f,   0.67f, 1.0f,
	-0.5f, -0.5f, 0.0f,   0.33f, 0.75f
};

    
