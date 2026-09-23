// This file is part of Game Loop Versatile Modules (GLVM)
// Copyright © 2024 Maksim Manokhin a.k.a. Yuriorkis_Scream. Contacts: <fellfrostqtw@gmail.com>
// Author: Maksim Manokhin a.k.a. Yuriorkis_Scream
// License: http://opensource.org/licenses/MIT

#ifndef VECTOR_DEBUG_COMPONENT_HPP
#define VECTOR_DEBUG_COMPONENT_HPP

#include "VertexMath.hpp"
#include "Event.hpp"
#include "typenames.hpp"

namespace GLVM::ecs::components
{
	struct vectorDebug
	{
		vec3 origin{ 0.0f, 0.0f, 0.0f };
		vec3 end{0.0f, 0.0f, 0.0f};
	    u32 meshIndex;
	};
}

#endif
