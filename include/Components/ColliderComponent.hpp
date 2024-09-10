// This file is part of Game Loop Versatile Modules (GLVM)
// Copyright © 2024 Maksim Manokhin a.k.a. Yuriorkis_Scream. Contacts: <fellfrostqtw@gmail.com>
// Author: Maksim Manokhin a.k.a. Yuriorkis_Scream
// License: http://opensource.org/licenses/MIT

#ifndef COLLIDER_COMPONENT
#define COLLIDER_COMPONENT

#include "Vector.hpp"
#include <vector>

namespace GLVM::ecs::components
{
	class collider
	{
    public:
        bool groundCollision = false;
		bool roofCollision = false;
        bool wallCollision = false;
		bool itemDrag = false;
		core::vector<unsigned int> colliders;
	};
}

#endif
