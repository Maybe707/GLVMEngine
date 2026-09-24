// This file is part of Game Loop Versatile Modules (GLVM)
// Copyright © 2024 Maksim Manokhin a.k.a. Yuriorkis_Scream. Contacts: <fellfrostqtw@gmail.com>
// Author: Maksim Manokhin a.k.a. Yuriorkis_Scream
// License: http://opensource.org/licenses/MIT

#ifndef MESH_GENERATION_COMPONENT_HPP
#define MESH_GENERATION_COMPONENT_HPP

#include "VertexMath.hpp"
#include "Vector.hpp"
#include "typenames.hpp"

namespace GLVM::ecs::components
{
    class meshGeneration
    {
    public:
		core::vector<vec3> vertices;
		u32 meshID;
    };
}


#endif
