// This file is part of Game Loop Versatile Modules (GLVM)
// Copyright © 2024 Maksim Manokhin a.k.a. Yuriorkis_Scream. Contacts: <fellfrostqtw@gmail.com>
// Author: Maksim Manokhin a.k.a. Yuriorkis_Scream
// License: http://opensource.org/licenses/MIT

#ifndef PROGECTIVE_GEOMETRIC_ALGEBRA
#define PROGECTIVE_GEOMETRIC_ALGEBRA

#include<cmath>

namespace GLVM::core::pga
{
	struct scalar {
		float value;
	};
	
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

	struct pseudoScalar {
		float w;
	};

	/*================== DUAL OPERATOR ======================*/
	inline point operator!( const plane& plane ) { return point{ .x = plane.x, .y = plane.y, .z = plane.z, .w = plane.w }; }
	inline plane operator!( const point& point ) { return plane{ .x = point.x, .y = point.y, .z = point.z, .w = point.w }; }
	inline line operator!( const line& line ) { return { .rx = line.ix, .ry = line.iy, .rz = line.iz, .ix = line.rx, .iy = line.ry, .iz = line.rz }; }
	inline scalar operator!( const pseudoScalar& pseudoScalar ) { return scalar{ .value = pseudoScalar.w }; }
	inline pseudoScalar operator!( const scalar& scalar ) { return pseudoScalar{ .w = scalar.value }; }

	/*=================== NORMALIZE =========================*/ 
	inline plane normalize( const plane& plane ) {
		float length = std::sqrt( plane.x * plane.x + plane.y * plane.y + plane.z * plane.z );
		return { .x = plane.x / length, .y = plane.y / length, .z = plane.z / length, .w = plane.w / length };
	}
	inline point normalize( const point& point ) {
		return { .x = point.x / point.w, .y = point.y / point.w, .z = point.z / point.z, .w = 1.0f };
	}
	inline line normalize( const line& line ) {
		float length = std::sqrt( line.ix * line.ix + line.iy * line.iy + line.iz * line.iz );
		return { .rx = line.rx / length, .ry = line.ry / length, .rz = line.rz / length,
			.ix = line.ix / length, .iy = line.iy / length, .iz = line.iz / length };
	}

	/*===================== REVERSE =========================*/
	inline plane operator~( const plane& plane ) { return plane; }
	inline line operator~( const line& line ) {
		return { .rx = line.rx, .ry = line.ry, .rz = line.rz, .ix = line.ix, .iy = line.iy, .iz = line.iz };
	}
	inline point operator~( const point& point ) {
		return { .x = point.x, .y = point.y, .z = point.z, .w = point.w };
	}

	/*===================== INNER PRODUCT====================*/
	/// Scalar product of the plane normals
	inline float operator|( const plane& plane0, const plane& plane1 ) {
		return plane0.x * plane1.x + plane0.y * plane1.y + plane0.z * plane1.z;
	}
	/// This gives the oriented distance from the point to the plane (if normalized)
	inline float operator|( const plane& plane, const point& point ) {
		return plane.x * point.x + plane.y * point.y + plane.z * point.z - plane.w * point.w;
	}
	/// If the plane and the line intersect, w ≠ 0, otherwise the result is an infinite point
	inline point operator|( const plane& plane, const line& line ) {
		return {
			.x = line.ry * plane.z - line.rz * plane.y - line.iy * plane.w,
			.y = line.rz * plane.x - line.rx * plane.z - line.iz * plane.w,
			.z = line.rx * plane.y - line.ry * plane.x - line.ix * plane.w,
			.w = line.ix * plane.x + line.iy * plane.y + line.iz * plane.z
		};
	}
	/* Angular measure between directions. These are dot directions; if the lines are normalized,
	   this is the cos(θ) between them.
	*/
	inline float operator|( const line& line0, const line& line1 ) {
		return line0.ix * line1.ix + line0.iy * line1.iy + line0.iz * line1.iz;
	}
	/* A line through a point defines a plane. the form is similar to plane ⋅ line, but semantically
	   it is a plane containing l and pt
	*/
	inline plane operator|( const line& line, const point& point ) {
		return {
			.x = line.ry * point.z - line.rz * point.y - line.iy * point.w,
			.y = line.rz * point.x - line.rx * point.z - line.iz * point.w,
			.z = line.rx * point.y - line.ry * point.x - line.ix * point.w,
			.w = line.ix * point.x + line.iy * point.y + line.iz * point.z
		};
	}
	/// Points do not have an inner product: it is always zero (if strictly by definition).
	inline float operator|( [[maybe_unused]] const point& point0, [[maybe_unused]] const point& point1 ) {
		return 0.0f;
	}

	/*====================== OUTER PRODUCT ====================*/
	/// plane ^ plane -> line ( those intersection )
	inline line operator^( const plane& plane0, const plane& plane1 ) {
		return {
			/// real part (moment): e23, e31, e12
			.rx = plane0.w * plane1.x - plane0.x * plane1.w,
			.ry = plane0.w * plane1.y - plane0.y * plane1.w,
			.rz = plane0.w * plane1.z - plane0.z * plane1.w,
			/// ideal part (direction): e01, e02, e03
			.ix = plane0.y * plane1.z - plane0.z * plane1.y,
			.iy = plane0.z * plane1.x - plane0.x * plane1.z,
			.iz = plane0.x * plane1.y - plane0.y * plane1.x,
		};
	}
	/// plane ∧ point -> line passing through a point on a plane
	inline line operator^( const plane& plane, const point& point ) {
		return {
			.rx = plane.x * point.w - plane.w * point.x,
			.ry = plane.y * point.w - plane.w * point.y,
			.rz = plane.z * point.w - plane.w * point.z,
			.ix = plane.y * point.z - plane.z * point.y,
			.iy = plane.z * point.x - plane.x * point.z,
			.iz = plane.x * point.y - plane.y * point.x
		};
	}
	/// line ^ point -> plane
	inline plane operator^( const line& line, const point& point ) {
		return {
			.x = line.ry * point.z - line.rz * point.y - line.iy * point.w,
			.y = line.rz * point.x - line.rx * point.z - line.iz * point.w,
			.z = line.rx * point.y - line.ry * point.x - line.ix * point.w,
			.w = line.ix * point.x + line.iy * point.y + line.iz * point.z
		};
	}
	/// point ∧ point → line (through two points)
	inline line operator^( const point& point0, const point& point1 ) {
		return {
			.rx = point0.x * point1.w - point0.w * point1.x,        ///< e23
			.ry = point0.y * point1.w - point0.w * point1.y,        ///< e31
			.rz = point0.z * point1.w - point0.w * point1.z,        ///< e12
			.ix = point0.y * point1.z - point0.z * point1.y,
			.iy = point0.z * point1.x - point0.x * point1.z,
			.iz = point0.x * point1.y - point0.y * point1.x
		};
	}
	/* line ∧ line → point (if intersecting). If w == 0, then the lines do
	   not intersect (the result is a point at infinity).
	*/
	inline point operator^( const line& line0, const line& line1 ) {
		return {
			.x = line0.ry * line1.iz - line0.rz * line1.iy + line0.iy * line1.rz - line0.iz * line1.ry,
			.y = line0.rz * line1.ix - line0.rx * line1.iz + line0.iz * line1.rx - line0.ix * line1.rz,
			.z = line0.rx * line1.iy - line0.ry * line1.ix + line0.ix * line1.ry - line0.iy * line1.rx,
			.w = line0.ix * line1.ix + line0.iy * line1.iy + line0.iz * line1.iz
		};
	}
	/* This is the wedge product between a plane and a line, the result is the intersection point
	   if they are not parallel. If they are parallel, the point will be at infinity (w = 0).
	   This is the same formula as inner(plane, line) - in PGA 3D the result of plane ∧ line and
	   plane | line have the same component form, but semantically they are different operations:
	   1. inner — orthogonal projection
	   2. outer — geometric "generating" subspace
	*/
	inline point operator^( const plane& plane, const line& line ) {
		return {
			.x = line.ry * plane.z - line.rz * plane.y - line.iy * plane.w,
			.y = line.rz * plane.x - line.rx * plane.z - line.iz * plane.w,
			.z = line.rx * plane.y - line.ry * plane.x - line.ix * plane.w,
			.w = line.ix * plane.x + line.iy * plane.y + line.iz * plane.z
		};
	}
}; // namespace GLVM::core::gpa

#endif
