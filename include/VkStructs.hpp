#ifndef VK_STRUCTS
#define VK_STRUCTS

#include <math.h>

namespace GLVM::core
{    
	struct MeshAxisMaxAbsoluteValues {
		float absolute_x = 0.0f;
		float absolute_y = 0.0f;
		float absolute_z = 0.0f;
	};

	struct MeshAxisLimitingValues {
		float lowest_x = MAXFLOAT;
		float highest_x = -MAXFLOAT;
		float lowest_y = MAXFLOAT;
		float highest_y = -MAXFLOAT;
		float lowest_z = MAXFLOAT;
		float highest_z = -MAXFLOAT;

		void setToDefaultValues() {
			highest_x = -MAXFLOAT;
			lowest_x  = MAXFLOAT;
			highest_y = -MAXFLOAT;
			lowest_y  = MAXFLOAT;
			highest_z = -MAXFLOAT;
			lowest_z  = MAXFLOAT;
		}
	};
	
	enum class DescriptorsTypes {
		/// UBO - uniform buffer object
		DIRECTIONAL_LIGHT_SHADOW_MAP_MATRIX_UBO,
		SPOT_LIGHT_SHADOW_MAP_MATRIX_UBO,
		POINT_LIGHT_SHADOW_MAP_MATRIX_UBO,
		HUD_UBO,
		FONT_UBO,
		FONT_ATLAS_SAMPLER,
		HUD_SCREEN_UBO,
		UI_UBO,
		UI_SAMPLER,
		UI_ICONS_UBO,
		UI_ICONS_SAMPLER,
		MODEL_MATRIX_UBO,

		LIGHT_DATA,
		SPECULAR_SAMPLER,
		LIGHT_SAMPLERS,
	};
} 

#endif
