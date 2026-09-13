#pragma once
#include "Rendering/RenderTypes.hpp"
namespace GLVM::core {
// CPU snapshot consumed synchronously by the renderer and its joined workers.
struct RenderFrame {
		bool isInventoryOpened = false;
		bool isDebugCollisitionsActive = false;
		vec3 forward{};
		float hud_screen_x = 0.0f;
		float hud_screen_y{};
		core::vector<RenderActor> actors{};
		core::vector<RenderDirectionalLight> directionalLights{};
		core::vector<RenderSpotLight> spotLights{};
		core::vector<RenderPointLight> pointLights{};
		core::vector<RenderHealth> healthBars{};
		core::vector<RenderFont> fonts{};
		core::vector<RenderInventory> inventories{};
		core::vector<RenderItem> items{};
		core::vector<RenderCrosshair> crosshairs{};
		core::vector<RenderPlayer> players{};
		core::vector<RenderCollisionWireframe> collisionsWireframes{};
		RenderPlayer player{};
		RenderSpacialGrid renderSpacialGrid{};
		float aspectRate = 16.0f / 9.0f;
		int   dragedItemEntity = -1;
		mat4 viewMatrix{};
		mat4 projectionMatrix{};
};
}
