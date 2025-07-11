// This file is part of Game Loop Versatile Modules (GLVM)
// Copyright © 2024 Maksim Manokhin a.k.a. Yuriorkis_Scream. Contacts: <fellfrostqtw@gmail.com>
// Author: Maksim Manokhin a.k.a. Yuriorkis_Scream
// License: http://opensource.org/licenses/MIT

#ifndef PROGECTIVE_GEOMETRIC_ALGEBRA
#define PROGECTIVE_GEOMETRIC_ALGEBRA

#include<cmath>
#include<assert.h>
#include <ostream>
#include<iostream>

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

	struct rline {
		float rx;
		float ry;
		float rz;
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

	struct motor {
		float rx;
		float ry;
		float rz;
		float rw;         ///< Scalar
		float ix;
		float iy;
		float iz;
		float iw;         ///< Pseudoscalar
	};

	struct rotor {
		float rx;
		float ry;
		float rz;
		float rw;
	};

	struct translator {
		float ix;
		float iy;
		float iz;
		float iw;
	};

	inline std::ostream& operator<<( std::ostream& os, const line& line ) {
		os << "rx: " << line.rx << " ry: " << line.ry << " rz: " << line.rz <<
			" ix: " << line.ix << " iy: " << line.iy << " iz: " << line.iz;
		return os;
	}

	inline std::ostream& operator<<( std::ostream& os, const point& point ) {
		os << "x: " << point.x << " y: " << point.y << " z: " << point.z << " w: " << point.w;
		return os;
	}

	inline std::ostream& operator<<( std::ostream& os, const plane& plane ) {
		os << "x: " << plane.x << " y: " << plane.y << " z: " << plane.z << " w: " << plane.w;
		return os;
	}

	inline std::ostream& operator<<( std::ostream& os, const scalar& scalar ) {
		os << "value: " << scalar.value;
		return os;
	}
	
	inline line operator-( line line ) {
		return { .rx = -line.rx, .ry = -line.ry, .rz = -line.rz, .ix = -line.ix, .iy = -line.iy, .iz = -line.iz };
	}

	inline point operator-( point point ) {
		return { .x = -point.x, .y = -point.y, .z = -point.z, .w = -point.w };
	}
	
	/*================== DUAL OPERATOR ======================*/
	inline point operator!( const plane& plane ) { return point{ .x = plane.x, .y = plane.y, .z = plane.z, .w = plane.w }; }
	inline plane operator!( const point& point ) { return plane{ .x = point.x, .y = point.y, .z = point.z, .w = point.w }; }
	inline line operator!( const line& line ) { return { .rx = line.ix, .ry = line.iy, .rz = line.iz, .ix = line.rx, .iy = line.ry, .iz = line.rz }; }
	inline scalar operator!( const pseudoScalar& pseudoScalar ) { return scalar{ .value = pseudoScalar.w }; }
	inline pseudoScalar operator!( const scalar& scalar ) { return pseudoScalar{ .w = scalar.value }; }

	/*=================== NORMALIZE =========================*/ 
	inline plane normalize( const plane& plane ) {
		float length = std::sqrt( plane.x * plane.x + plane.y * plane.y + plane.z * plane.z );
		assert(length != 0);
		return { .x = plane.x / length, .y = plane.y / length, .z = plane.z / length, .w = plane.w / length };
	}
	// inline point normalize( const point& point ) {
	// 	return { .x = point.x / point.w, .y = point.y / point.w, .z = point.z / point.z, .w = 1.0f };
	// }
	inline line normalize( const line& line ) {
		float length = std::sqrt( line.ix * line.ix + line.iy * line.iy + line.iz * line.iz );
		assert(length != 0);
		return { .rx = line.rx / length, .ry = line.ry / length, .rz = line.rz / length,
			.ix = line.ix / length, .iy = line.iy / length, .iz = line.iz / length };
	}

	/*===================== REVERSE =========================*/
	inline plane operator~( const plane& plane ) { return plane; }
	inline line operator~( const line& line ) { return -line; }
	inline point operator~( const point& point ) { return -point; }

	/*===================== INNER PRODUCT====================*/
	/// Scalar product of the plane normals
	inline float operator|( const plane& plane0, const plane& plane1 ) {
		return plane0.x * plane1.x + plane0.y * plane1.y + plane0.z * plane1.z;
	}
	/// This gives the oriented distance from the point to the plane (if normalized)
	inline line operator|( const plane& plane, const point& point ) {
//		return plane.x * point.x + plane.y * point.y + plane.z * point.z - plane.w * point.w;
		// std::cout << "inner ix: " << plane.z * point.y - plane.y * point.z << std::endl;
		// std::cout << "inner iy: " << plane.x * point.z - plane.z * point.x << std::endl;
		// std::cout << "inner iz: " << plane.y * point.x - plane.x * point.y << std::endl;
		return {
			.rx = plane.x * point.w,                        ///< e2 ^ e3
			.ry = plane.y * point.w,                        ///< e3 ^ e1
			.rz = plane.z * point.w,                        ///< e1 ^ e2
			.ix = plane.z * point.y - plane.y * point.z,    ///< e0 ^ e1
			.iy = plane.x * point.z - plane.z * point.x,    ///< e0 ^ e2
			.iz = plane.y * point.x - plane.x * point.y     ///< e0 ^ e3
		};
	}
	/// If the plane and the line intersect, w ≠ 0, otherwise the result is an infinite point
	inline point operator|( const plane& plane, const line& line ) {
		// return {
		// 	.x = line.ry * plane.z - line.rz * plane.y - line.iy * plane.w,
		// 	.y = line.rz * plane.x - line.rx * plane.z - line.iz * plane.w,
		// 	.z = line.rx * plane.y - line.ry * plane.x - line.ix * plane.w,
		// 	.w = line.ix * plane.x + line.iy * plane.y + line.iz * plane.z
		// };
		return {
			.x = -plane.y * line.rz + plane.z * line.ry,                     ///< e1
			.y = plane.x * line.rz - plane.z * line.rx,                      ///< e2
			.z = -plane.x * line.ry + plane.y * line.rx,                     ///< e3
			.w = -plane.x * line.ix - plane.y * line.iy - plane.z * line.iz  ///< e0
		};
	}
	/* Angular measure between directions. These are dot directions; if the lines are normalized,
	   this is the cos(θ) between them.
	*/
	inline float operator|( const line& line0, const line& line1 ) {
		return -line0.rx * line1.rx - line0.ry * line1.ry - line0.rz * line1.rz;
	}
	/* A line through a point defines a plane. the form is similar to plane ⋅ line, but semantically
	   it is a plane containing l and pt
	*/
	inline plane operator|( const line& line, const point& point ) {
		// return {
		// 	.x = line.ry * point.z - line.rz * point.y - line.iy * point.w,
		// 	.y = line.rz * point.x - line.rx * point.z - line.iz * point.w,
		// 	.z = line.rx * point.y - line.ry * point.x - line.ix * point.w,
		// 	.w = line.ix * point.x + line.iy * point.y + line.iz * point.z
		// };
		return {
			.x = -line.rx * point.w,
			.y = -line.ry * point.w,
			.z = -line.rz * point.w,
			.w = line.rx * point.x + line.ry * point.y + line.rz * point.z
		};
	}
	/// Points do not have an inner product: it is always zero (if strictly by definition).
	inline scalar operator|( [[maybe_unused]] const point& point0, [[maybe_unused]] const point& point1 ) {
		return { .value = -point0.w * point1.w };
	}

	/*====================== OUTER PRODUCT ====================*/
	/// plane ^ plane -> line ( those intersection )
	inline line operator^( const plane& plane0, const plane& plane1 ) {
		// return {
		// 	/// real part (moment): e23, e31, e12
		// 	.rx = plane0.w * plane1.x - plane0.x * plane1.w,
		// 	.ry = plane0.w * plane1.y - plane0.y * plane1.w,
		// 	.rz = plane0.w * plane1.z - plane0.z * plane1.w,
		// 	/// ideal part (direction): e01, e02, e03
		// 	.ix = plane0.y * plane1.z - plane0.z * plane1.y,
		// 	.iy = plane0.z * plane1.x - plane0.x * plane1.z,
		// 	.iz = plane0.x * plane1.y - plane0.y * plane1.x,
		// };
		return {
			/// real part ( moment ): e23, e31, e12
			.rx = plane0.y * plane1.z - plane0.z * plane1.y,
			.ry = plane0.z * plane1.x - plane0.x * plane1.z,
			.rz = plane0.x * plane1.y - plane0.y * plane1.x,
			/// ideal part (direction): e01, e02, e03
			.ix = plane0.w * plane1.x - plane0.x * plane1.w,
			.iy = plane0.w * plane1.y - plane0.y * plane1.w,
			.iz = plane0.w * plane1.z - plane0.z * plane1.w
		};
	}
	/// plane ∧ point -> line passing through a point on a plane
	inline pseudoScalar operator^( const plane& plane, const point& point ) {
		// return {
		// 	.rx = plane.x * point.w - plane.w * point.x,
		// 	.ry = plane.y * point.w - plane.w * point.y,
		// 	.rz = plane.z * point.w - plane.w * point.z,
		// 	.ix = plane.y * point.z - plane.z * point.y,
		// 	.iy = plane.z * point.x - plane.x * point.z,
		// 	.iz = plane.x * point.y - plane.y * point.x
		// };
		return { .w = plane.x * point.x + plane.y * point.y + plane.z * point.z + plane.w * point.w };
	}
	/// line ^ point -> plane
	inline float operator^( [[maybe_unused]] const line& line, [[maybe_unused]] const point& point ) {
		// return {
		// 	.x = line.ry * point.z - line.rz * point.y - line.iy * point.w,
		// 	.y = line.rz * point.x - line.rx * point.z - line.iz * point.w,
		// 	.z = line.rx * point.y - line.ry * point.x - line.ix * point.w,
		// 	.w = line.ix * point.x + line.iy * point.y + line.iz * point.z
		// };
		return 0.0;
	}
	/// point ∧ point → line (through two points)
	inline float operator^( [[maybe_unused]] const point& point0, [[maybe_unused]] const point& point1 ) {
		// return {
		// 	.rx = point0.x * point1.w - point0.w * point1.x,        ///< e23
		// 	.ry = point0.y * point1.w - point0.w * point1.y,        ///< e31
		// 	.rz = point0.z * point1.w - point0.w * point1.z,        ///< e12
		// 	.ix = point0.y * point1.z - point0.z * point1.y,
		// 	.iy = point0.z * point1.x - point0.x * point1.z,
 		// 	.iz = point0.x * point1.y - point0.y * point1.x
		// };
		return 0.0;
	}
	/* line ∧ line → point (if intersecting). If w == 0, then the lines do
	   not intersect (the result is a point at infinity).
	*/
	inline pseudoScalar operator^( const line& line0, const line& line1 ) {
		// return {
		// 	.x = line0.ry * line1.iz - line0.rz * line1.iy + line0.iy * line1.rz - line0.iz * line1.ry,
		// 	.y = line0.rz * line1.ix - line0.rx * line1.iz + line0.iz * line1.rx - line0.ix * line1.rz,
		// 	.z = line0.rx * line1.iy - line0.ry * line1.ix + line0.ix * line1.ry - line0.iy * line1.rx,
		// 	.w = line0.ix * line1.ix + line0.iy * line1.iy + line0.iz * line1.iz
		// };
		/// e0 ^ (e1 ^ (e2 ^ e3))
		return { .w = line0.rx * line1.ix + line0.ry * line1.iy + line0.rz * line1.iz +
			line0.ix * line1.rx + line0.iy * line1.ry + line0.iz * line1.rz }; 
	}
	/* This is the wedge product between a plane and a line, the result is the intersection point
	   if they are not parallel. If they are parallel, the point will be at infinity (w = 0).
	   This is the same formula as inner(plane, line) - in PGA 3D the result of plane ∧ line and
	   plane | line have the same component form, but semantically they are different operations:
	   1. inner — orthogonal projection
	   2. outer — geometric "generating" subspace
	*/
	inline point operator^( const plane& plane, const line& line ) {
		// return {
		// 	.x = line.ry * plane.z - line.rz * plane.y - line.iy * plane.w,
		// 	.y = line.rz * plane.x - line.rx * plane.z - line.iz * plane.w,
		// 	.z = line.rx * plane.y - line.ry * plane.x - line.ix * plane.w,
		// 	.w = line.ix * plane.x + line.iy * plane.y + line.iz * plane.z
		// };
		return {
			.x = plane.y * line.iz - plane.z * line.iy - plane.w * line.rx,
			.y = -plane.x * line.iz + plane.z * line.ix - plane.w * line.ry,
			.z = plane.x * line.iy - plane.y * line.ix - plane.w * line.rz,
			.w = plane.x * line.rx + plane.y * line.ry + plane.z * line.rz
		};
	}

	/*================== REGRESSIVE PRODUCT ===================
	  Regressive product gives the intersection of objects.
	  In Projective Geometric Algebra (PGA), the regressive
	  product, denoted by ∨ (vee), is the dual operation to the
	  exterior product (∧). That is:
	  A ∨ B = (⟦A⟧ ∧ ⟦B⟧)*, where ⟦A⟧ is the dual of object A,
	  and * is the dual of the result
	 =========================================================*/
	inline float operator&( [[maybe_unused]] plane plane0, [[maybe_unused]] plane plane1 ) {
		return 0.0f;
	}

	inline pseudoScalar operator&( plane plane, point point ) {
		/// plane below link with dual point from outer product and point below link with dual plane from outer product
		return { .w = plane.x * point.x + plane.y * point.y + plane.z * point.z + plane.w * point.w };
	}

	inline plane operator&( point point, line line ) {
		/// point below link with dual plane from outer product and line below link with dual line from outer product
		return {
			.x = point.y * line.iz - point.z * line.iy - point.w * line.rx,
			.y = -point.x * line.iz + point.z * line.ix - point.w * line.ry,
			.z = point.x * line.iy - point.y * line.ix - point.w * line.rz,
			.w = point.x * line.rx + point.y * line.ry + point.z * line.rz
		};
	}

	inline line operator&( [[maybe_unused]] point point0, [[maybe_unused]] point point1 ) {
		/// point0 below link with dual plane0 from outer product and point1 below link with dual plane1 from outer product
		return {
			/// real part ( moment ): e23, e31, e12
			.rx = point0.y * point1.z - point0.z * point1.y,
			.ry = point0.z * point1.x - point0.x * point1.z,
			.rz = point0.x * point1.y - point0.y * point1.x,
			/// ideal part (direction): e01, e02, e03
			.ix = point0.w * point1.x - point0.x * point1.w,
			.iy = point0.w * point1.y - point0.y * point1.w,
			.iz = point0.w * point1.z - point0.z * point1.w
		};
	}

	inline scalar operator&( line line0, line line1 ) {
		return { .value = line0.rx * line1.ix + line0.ry * line1.iy + line0.rz * line1.iz +
			line0.ix * line1.rx + line0.iy * line1.ry + line0.iz * line1.rz };
	}

	inline float operator&( [[maybe_unused]] plane plane, [[maybe_unused]] line line ) {
		return 0.0f;
	}


	/*========================= GEOMETRIC PRODUCT =========================*/
	inline motor operator*( plane plane0, plane plane1 ) {
		return {
			/// real part ( moment ): e23, e31, e12
			.rx = plane0.y * plane1.z - plane0.z * plane1.y,
			.ry = plane0.z * plane1.x - plane0.x * plane1.z,
			.rz = plane0.x * plane1.y - plane0.y * plane1.x,
			.rw = plane0.x * plane1.x + plane0.y * plane1.y + plane0.z * plane1.z, ///< Scalar
			/// ideal part (direction): e01, e02, e03
			.ix = plane0.w * plane1.x - plane0.x * plane1.w,
			.iy = plane0.w * plane1.y - plane0.y * plane1.w,
			.iz = plane0.w * plane1.z - plane0.z * plane1.w,
			.iw = 0.0                                                              ///< Pseudoscalar
		};
	}

	inline motor operator*( line line0, line line1 ) {
		return {
			/// real part ( moment ): e23, e31, e12
			.rx = -line0.ry * line1.rz + line0.rz * line1.ry,
			.ry = line0.rx * line1.rz + line0.rz * line1.rx,
			.rz = -line0.rx * line1.ry + line0.ry * line1.rx,
			.rw = -line0.rx * line1.rx - line0.ry * line1.ry - line0.rz * line1.rz, ///< Scalar
			/// ideal part (direction): e01, e02, e03
			.ix = -line0.ry * line1.iz + line0.rz * line1.iy - line0.iy * line1.rz + line0.iz * line1.ry,
			.iy = line0.rx * line1.iz - line0.rz * line1.ix + line0.ix * line1.rz - line0.iz * line1.rx,
			.iz = -line0.rx * line1.iy + line0.ry * line1.ix - line0.ix * line1.ry + line0.iy * line1.rx,
			.iw = line0.rx * line1.ix + line0.ry * line1.iy + line0.rz * line1.iz +
			line0.ix * line1.rx + line0.iy * line1.ry + line0.iz * line1.rz        ///< Pseudoscalar
		};
	}

	inline rotor operator*( rline rline0, rline rline1 ) {
		return {
			/// real part ( moment ): e23, e31, e12
			.rx = -rline0.ry * rline1.rz + rline0.rz * rline1.ry,
			.ry = rline0.rx * rline1.rz + rline0.rz * rline1.rx,
			.rz = -rline0.rx * rline1.ry + rline0.ry * rline1.rx,
			.rw = -rline0.rx * rline1.rx - rline0.ry * rline1.ry - rline0.rz * rline1.rz, ///< Scalar
		};
	}

	inline translator operator*( point point0, point point1 ) {
		return {
			.ix = point0.x * point1.w - point0.w * point1.x,
			.iy = point0.y * point1.w - point0.w * point1.y,
			.iz = point0.z * point1.w - point0.w * point1.z,
			.iw = point0.w * point1.w
		};
	}
}; // namespace GLVM::core::pga

#endif
