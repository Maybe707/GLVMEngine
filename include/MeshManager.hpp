#ifndef MESH_MANAGER
#define MESH_MANAGER

#include <vector>
#include <mutex>
#include "Vector.hpp"
#include "Components/VertexComponent.hpp"

typedef unsigned int Mesh_ID;

namespace GLVM::core
{
    class MeshManager
    {
        static MeshManager* pInstance_;
        static std::mutex  Mutex_;

        MeshManager();
        ~MeshManager();
        
    public:
        std::vector<const char*> pathsArray_;
		core::vector<const char*> pathsGLTF_;

        static MeshManager* GetInstance();                          ///< It possibly to get only one instance of this class whith this method.
        void SetMesh(const char* _pathToMesh);
		void SetMeshGLTF(const char* pathToMesh);
    };
}

#endif
