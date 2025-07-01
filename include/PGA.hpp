// This file is part of Game Loop Versatile Modules (GLVM)
// Copyright © 2024 Maksim Manokhin a.k.a. Yuriorkis_Scream. Contacts: <fellfrostqtw@gmail.com>
// Author: Maksim Manokhin a.k.a. Yuriorkis_Scream
// License: http://opensource.org/licenses/MIT

#ifndef PROGECTIVE_GEOMETRIC_ALGEBRA
#define PROGECTIVE_GEOMETRIC_ALGEBRA

#include<concepts>

namespace GLVM::core::pga
{
	struct plane {      ///< vector in 3D PGA
		float x;            ///< e1 basis vector
		float y;            ///< e2 basis vector
		float z;            ///< e3 basis vector
		float w;            ///< e0 progective plane in infinity
	};

	struct line {      ///< bivector 
		float rx;
		float ry;
		float rz;
		float ix;
		float iy;
		float iz;
	};

	struct point {     ///< trivector
		float x;
		float y;
		float z;
		float w;
	};

	template<typename T>
	concept primitivesPGA_3D = requires(T a)
	{
		{ a } -> std::same_as<plane>;
		{ a } -> std::same_as<line>;
		{ a } -> std::same_as<point>;
	};

	template <typename T>
	requires primitivesPGA_3D<T>
	point operator!(T primitive) {
		return point{ .x = primitive.x, .y = primitive.y, .z = primitive.z, .w = primitive.w };
	}
}; // namespace GLVM::core::gpa

#endif
