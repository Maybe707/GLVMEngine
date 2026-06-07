// This file is part of Game Loop Versatile Modules (GLVM)
// Copyright © 2024 Maksim Manokhin a.k.a. Yuriorkis_Scream. Contacts: <fellfrostqtw@gmail.com>
// Author: Maksim Manokhin a.k.a. Yuriorkis_Scream
// License: http://opensource.org/licenses/MIT

#include "Common/CommonFunctions.hpp"

namespace GLVM::core {
	bool BoxCollider(const vec3 backtrackingPosition,
					 const vec3 comparedPosition,
					 const float backtrackingScale,
					 const float comparedScale,
					 const core::MeshAxisMaxAbsoluteValues& backtrackingMeshAxisMaxAbsoluteValues,
					 const core::MeshAxisMaxAbsoluteValues& comparedMeshAxisMaxAbsoluteValues)
	{
        if(backtrackingPosition[0] + backtrackingMeshAxisMaxAbsoluteValues.origin_offset_x + backtrackingMeshAxisMaxAbsoluteValues.absolute_x * backtrackingScale  >
		   comparedPosition[0] + comparedMeshAxisMaxAbsoluteValues.origin_offset_x - comparedMeshAxisMaxAbsoluteValues.absolute_x * comparedScale &&
           backtrackingPosition[0] + backtrackingMeshAxisMaxAbsoluteValues.origin_offset_x - backtrackingMeshAxisMaxAbsoluteValues.absolute_x * backtrackingScale  <
		   comparedPosition[0] + comparedMeshAxisMaxAbsoluteValues.origin_offset_x + comparedMeshAxisMaxAbsoluteValues.absolute_x * comparedScale &&
           backtrackingPosition[1] + backtrackingMeshAxisMaxAbsoluteValues.origin_offset_y + backtrackingMeshAxisMaxAbsoluteValues.absolute_y * backtrackingScale  >
		   comparedPosition[1] + comparedMeshAxisMaxAbsoluteValues.origin_offset_y - comparedMeshAxisMaxAbsoluteValues.absolute_y * comparedScale &&
           backtrackingPosition[1] + backtrackingMeshAxisMaxAbsoluteValues.origin_offset_y - backtrackingMeshAxisMaxAbsoluteValues.absolute_y * backtrackingScale  <
		   comparedPosition[1] + comparedMeshAxisMaxAbsoluteValues.origin_offset_y + comparedMeshAxisMaxAbsoluteValues.absolute_y * comparedScale &&
           backtrackingPosition[2] + backtrackingMeshAxisMaxAbsoluteValues.origin_offset_z + backtrackingMeshAxisMaxAbsoluteValues.absolute_z * backtrackingScale  >
		   comparedPosition[2] + comparedMeshAxisMaxAbsoluteValues.origin_offset_z - comparedMeshAxisMaxAbsoluteValues.absolute_z * comparedScale &&
           backtrackingPosition[2] + backtrackingMeshAxisMaxAbsoluteValues.origin_offset_z - backtrackingMeshAxisMaxAbsoluteValues.absolute_z * backtrackingScale  <
		   comparedPosition[2] + comparedMeshAxisMaxAbsoluteValues.origin_offset_z + comparedMeshAxisMaxAbsoluteValues.absolute_z * comparedScale) {
				return true;
		}
        
		return false;
	}

	core::vector<vec3> computeBoxCornerBoundPoints(
		const core::MeshAxisMaxAbsoluteValues entityChunkBounds,
		vec3 entityPosition,
		const float scale ) {
		const float halfWidht  = entityChunkBounds.absolute_x * scale * 0.5f;
		const float halfHeight = entityChunkBounds.absolute_y * scale * 0.5f;
		const float halfDepth  = entityChunkBounds.absolute_z * scale * 0.5f;

		core::vector<vec3> result;
		result.Push( entityPosition + vec3( -halfWidht, -halfHeight, -halfDepth ) );  ///< left bottom back
		result.Push( entityPosition + vec3( halfWidht, -halfHeight, -halfDepth ) );   ///< right bottom back
		result.Push( entityPosition + vec3( halfWidht, halfHeight, -halfDepth ) );    ///< right upper back
		result.Push( entityPosition + vec3( -halfWidht, halfHeight, -halfDepth ) );   ///< left upper back
		result.Push( entityPosition + vec3( -halfWidht, halfHeight, halfDepth ) );    ///< left upper front
		result.Push( entityPosition + vec3( -halfWidht, -halfHeight, halfDepth ) );   ///< left bottom front
		result.Push( entityPosition + vec3( halfWidht, -halfHeight, halfDepth ) );    ///< right botoom front
		result.Push( entityPosition + vec3( halfWidht, halfHeight, halfDepth ) );     ///< right upper front

		return result;
	}
}; ///< namespace GLVM::core
