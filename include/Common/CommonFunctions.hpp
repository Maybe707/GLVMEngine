// This file is part of Game Loop Versatile Modules (GLVM)
// Copyright © 2024 Maksim Manokhin a.k.a. Yuriorkis_Scream. Contacts: <fellfrostqtw@gmail.com>
// Author: Maksim Manokhin a.k.a. Yuriorkis_Scream
// License: http://opensource.org/licenses/MIT

#ifndef COMMON_FUNCTIONS_HPP
#define COMMON_FUNCTIONS_HPP

#include "VertexMath.hpp"
#include "Components/VertexComponent.hpp"
#include "Globals.hpp"

namespace GLVM::core {
	bool BoxCollider(
		const vec3 backtrackingPosition,
		const vec3 comparedPosition,
		const float backtrackingScale,
		const float comparedScale,
		const core::MeshAxisMaxAbsoluteValues& backtrackingMeshAxisMaxAbsoluteValues,
		const core::MeshAxisMaxAbsoluteValues& comparedMeshAxisMaxAbsoluteValues);
}; ///< namespace GLVM::core

#endif
