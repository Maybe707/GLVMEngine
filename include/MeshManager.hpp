#ifndef MESH_MANAGER
#define MESH_MANAGER

#include <vector>
#include "GraphicAPI/Vulkan.hpp"
#include "Components/VertexComponent.hpp"

typedef unsigned int Mesh_ID;

namespace GLVM::core
{
    class CMeshManager
    {
        static CMeshManager* pInstance_;
        static std::mutex  Mutex_;

        CMeshManager();
        ~CMeshManager();
        
    public:
        std::vector<const char*> pathsArray_;

        static CMeshManager* GetInstance();                          ///< It possibly to get only one instance of this class whith this method.
        void SetMesh(const char* _pathToMesh);
    };
}

#endif
