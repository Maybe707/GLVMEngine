// This file is part of Game Loop Versatile Modules (GLVM)
// Copyright © 2024 Maksim Manokhin a.k.a. Yuriorkis_Scream. Contacts: <fellfrostqtw@gmail.com>
// Author: Maksim Manokhin a.k.a. Yuriorkis_Scream
// License: http://opensource.org/licenses/MIT

#include "Rendering/ScenePresenter.hpp"
#include "Rendering/CameraOrbit.hpp"
#include "Globals.hpp"
#include "ArchetypeECS/ArchECS_Types.hpp"
#include "ArchetypeECS/ArchECS_Utils.hpp"
#include "ArchetypeECS/ArchetypeInterface.hpp"
#include "Archetypes/CrosshairArchetype.hpp"
#include "Archetypes/DirectionalLightArchetype.hpp"
#include "Archetypes/EnemyArchetype.hpp"
#include "Archetypes/InventoryArchetype.hpp"
#include "Archetypes/ItemArchetype.hpp"
#include "Archetypes/LevelChunkArchetype.hpp"
#include "Archetypes/PlayerArchetype.hpp"
#include "Archetypes/ProjectileArchetype.hpp"
#include "Archetypes/StaticMeshArchetype.hpp"
#include "Common/CommonFunctions.hpp"
#include "Components/HealthComponent.hpp"
#include "Components/ProjectileBundle.hpp"
#include "Components/AnimationComponent.hpp"
#include "Components/ColliderComponent.hpp"
#include "Components/DirectionalLightComponent.hpp"
#include "Components/InventoryComponent.hpp"
#include "Components/MaterialComponent.hpp"
#include "Components/PointLightComponent.hpp"
#include "Components/RotationComponent.hpp"
#include "Components/TransformComponent.hpp"
#include "Components/VertexComponent.hpp"
#include "Components/ViewComponent.hpp"
#include "Event.hpp"
#include "ShaderStructs.hpp"
#include "TagComponents/LevelChunkTagComponent.hpp"
#include "Texture.hpp"
#include "VertexMath.hpp"
#include <cstdint>
#include <limits>
#include <mutex>
#include <sys/types.h>
#include <thread>
#include <fstream>
#include <filesystem>
#include <iomanip>
namespace GLVM::core {
	void ScenePresenter::EnlargeFrameAccumulator(float value) {
		namespace cm   = GLVM::ecs::components;
		namespace arch = GLVM::ecs::arch;
		animationArchetypesNumber = 0;
		for( uint32_t m = 0; m < world_.archetypes.GetSize(); ++m ) {
			arch::Archetype* arch = world_.archetypes[m];
			arch::componentMask requiredMask = (1ul << arch::ComponentsIndices::MESH_COMPONENT) |
				(1ul << arch::ComponentsIndices::ANIMATION_COMPONENT);

			if( arch::matchesRequiredMask( arch->mask, requiredMask ) ) {
				cachedAnimationArchetypes[animationArchetypesNumber] = arch;
				++animationArchetypesNumber;
			}
		}

		for( uint32_t n = 0; n < animationArchetypesNumber; ++n ) {
			arch::Archetype* arch = cachedAnimationArchetypes[n];
			cm::animation* animationView  = nullptr;
			cm::mesh*      meshView       = nullptr;
			if( arch != nullptr ) {
				switch( arch->mask ) {
				case arch::enemyComponentMask:
					animationView   = static_cast<arch::EnemyArchetype*>( arch )->animations;
					meshView        = static_cast<arch::EnemyArchetype*>( arch )->meshes;
					break;
				case arch::playerComponentMask:
					animationView   = static_cast<arch::PlayerArchetype*>( arch )->animations;
					meshView        = static_cast<arch::PlayerArchetype*>( arch )->meshes;
					break;
				}

				for(unsigned int i = 0; i < cachedAnimationArchetypes[n]->entityCount; ++i) {
					if( &meshView[i] != nullptr && &animationView[i] != nullptr ) {
						unsigned int mesh_id = meshView[i].handle.id;
						if ( assets_.jointMatricesPerMesh.GetSize() > 0 && assets_.jointMatricesPerMesh[mesh_id].GetSize() > 0 )
							animationView[i].frameAccumulator += value;
					}
				}
			}
		}
	}

    void ScenePresenter::SetViewMatrix() {
        namespace cm = GLVM::ecs::components;
        namespace arch = GLVM::ecs::arch;
        playerArchetypesNumber = 0;
        world_.searchCacheArchetypes(playerRequiredMask, cachedPlayerArchetypes, playerArchetypesNumber);
        const auto& mouse = event_.mousePointerPosition;
        for (uint32_t n = 0; n < playerArchetypesNumber; ++n) {
            auto* archetype = cachedPlayerArchetypes[n];
            auto* views = static_cast<cm::beholder*>(archetype->components[arch::ComponentsIndices::VIEW_COMPONENT]);
            auto* transforms = static_cast<cm::transform*>(archetype->components[arch::ComponentsIndices::TRANSFORM_COMPONENT]);
            for (uint32_t i = 0; i < archetype->entityCount; ++i) {
                auto& camera = views[i];
                auto& player = transforms[i];
                rotateCameraOrbit(camera, mouse.offset_X, mouse.offset_Y);
                if ((mouse.offset_X || mouse.offset_Y) &&
                    player.frameMovement[0] == 0.0f && player.frameMovement[2] == 0.0f &&
                    previousFrameKeyEvents[0] == 0 && previousFrameKeyEvents[1] == 0 &&
                    previousFrameKeyEvents[2] == 0 && previousFrameKeyEvents[3] == 0) {
                    player.forward = camera.forward;
                    const auto previous = Normalize(vec3(player.previousFrameForward[0], 0.0f, player.previousFrameForward[2]));
                    const auto current = Normalize(vec3(camera.forward[0], 0.0f, camera.forward[2]));
                    const float turn = cross<float>(vec2(previous[0], previous[2]), vec2(current[0], current[2]));
                    player.pitch -= std::atan2(turn, Dot(previous, current));
                    player.previousFrameForward = current;
                }
                frame_.viewMatrix = LookAtMain(camera.Position + player.position,
                                               camera.Position + player.position + camera.forward,
                                               vec3(0.0f, -1.0f, 0.0f));
            }
        }
    }

	void ScenePresenter::SetProjectionMatrix()
	{
		mat4 tProjection_Matrix = Perspective(Radians(90.0f), frame_.aspectRate, 0.1f, 100.0f);
		frame_.projectionMatrix = tProjection_Matrix;
		frame_.projectionMatrix[1][1] *= 1.0f;
	}

	[[nodiscard]] core::vector<mat4> ScenePresenter::updateAnimationFrames([[maybe_unused]] ecs::components::animation* animationComponent, [[maybe_unused]] unsigned int meshID) {
		if ( assets_.jointMatricesPerMesh.GetSize() > 0 && assets_.jointMatricesPerMesh[meshID].GetSize() > 0 &&
			 animationComponent->frameAccumulator >= assets_.frames[meshID][animationComponent->currentAnimationFrame] * 1.0f ) {
			++animationComponent->currentAnimationFrame;
			if ( assets_.jointMatricesPerMesh[meshID].GetSize() > 0 && animationComponent->currentAnimationFrame == assets_.frames[meshID].GetSize() ) {
				animationComponent->currentAnimationFrame = 0;
				animationComponent->frameAccumulator = 0.0f;
			}
		}

		unsigned int joinMatricesDataSize{};
		if ( assets_.jointMatricesPerMesh.GetSize() > 0 )
			joinMatricesDataSize = assets_.jointMatricesPerMesh[meshID].GetSize();

		core::vector<mat4> jointMatrices;
		if ( joinMatricesDataSize == 0 ) {
			jointMatrices.Resize(MAX_JOINTS_NUMBER);
			for ( unsigned int i = 0; i < MAX_JOINTS_NUMBER; ++i ) {
				mat4 unitMatrix(1.0f);
				jointMatrices[i] = unitMatrix;
			}

		} else {
			jointMatrices.Resize(MAX_JOINTS_NUMBER);
			for ( unsigned int i = 0; i < joinMatricesDataSize; ++i ) {
				if( meshID >= assets_.jointMatricesPerMesh.GetSize() ) {
					std::cout << "OUTER ARRAY OVERFLOW" << std::endl;
					throw("sdfsdf");
				} else if( i >= assets_.jointMatricesPerMesh[meshID].GetSize() ) {
					std::cout << "MIDDLE ARRAY OVERFLOW" << std::endl;
					throw("sdfsdf");
				} else if( animationComponent->currentAnimationFrame >= assets_.jointMatricesPerMesh[meshID][i].GetSize() ) {
					std::cout << "frame: " << animationComponent->currentAnimationFrame << std::endl;
					std::cout << "array size: " << assets_.jointMatricesPerMesh[meshID][i].GetSize() << std::endl;
					std::cout << "frames number: " << assets_.frames[meshID].GetSize() << std::endl;
					std::cout << "INNER ARRAY OVERFLOW" << std::endl;
					throw("sdfsdf");
				}

				jointMatrices[i] = assets_.jointMatricesPerMesh[meshID][i][animationComponent->currentAnimationFrame];
			}

			for ( u32 j = joinMatricesDataSize; j < MAX_JOINTS_NUMBER; ++j ) {
				mat4 unitMatrix(1.0f);
				jointMatrices[j] = unitMatrix;
			}
		}
		// core::vector<mat4> jointMatrices;
		// jointMatrices.Resize( MAX_JOINTS_NUMBER );
		// for ( unsigned int i = 0; i < MAX_JOINTS_NUMBER; ++i ) {
		// 	mat4 unitMatrix(1.0f);
		// 	jointMatrices[i] = unitMatrix;
		// }

		return jointMatrices;
	}

	mat4 ScenePresenter::updateDirectionalLightSpaceMatrixShadowMapUBO( ecs::components::directionalLight* directionalLightComponent ) {
		float nearPlaneFlatShadowMap = 5.5f;
		float farPlaneFlatShadowMap = 100.0f;
		mat4 directionalProjectionMatrixLight = ortho(-50.0f, 50.0f, -50.0f, 50.0f,
													  nearPlaneFlatShadowMap, farPlaneFlatShadowMap);

		vec3 positionVectorLight = directionalLightComponent->position;
		vec3 directionVectorLight = directionalLightComponent->direction;

		mat4 viewMatrixLight = LookAtMain(positionVectorLight,
										  directionVectorLight,
										  { 0.0f, -1.0f, 0.0f });

//		directionalProjectionMatrixLight[1][1] *= -1;
		return viewMatrixLight * directionalProjectionMatrixLight;
	}

	mat4 ScenePresenter::updateSpotLightSpaceMatrixShadowMapUBO( ecs::components::spotLight* spotLightComponent ) {
		float nearPlaneFlatShadowMap = 5.5f;
		float farPlaneFlatShadowMap = 100.0f;
		mat4 spotProjectionMatrixLight = Perspective(Radians(90.0f), (float)SHADOW_MAP_SIZE / (float)SHADOW_MAP_SIZE,
														 nearPlaneFlatShadowMap, farPlaneFlatShadowMap);

		vec3 positionVectorLight  = spotLightComponent->position;
		vec3 directionVectorLight = spotLightComponent->direction;
		mat4 viewMatrixLight = LookAtMain(positionVectorLight,
										  directionVectorLight,
										  { 0.0f, -1.0f, 0.0f });

//		spotProjectionMatrixLight[1][1] *= -1;
		return viewMatrixLight * spotProjectionMatrixLight;
	}

	mat4 ScenePresenter::updatePointLightSpaceMatrixShadowMapUBO( ecs::components::pointLight* pointLightComponent, uint32_t layer ) {
		vec3 positionVectorLight  = pointLightComponent->position;
		vec3 directionalVectorLight = vec3(0.0f, 0.0f, 0.0f);
		vec3 upVector = { 0.0, 0.0, 0.0 };

		switch(layer) {
		case 0:
			/// Positive X
			directionalVectorLight = positionVectorLight + vec3( 1.0f,  0.0f, 0.0f);
			upVector = vec3(0.0f, -1.0f,  0.0f);
			break;
		case 1:
			/// Negative X
			directionalVectorLight = positionVectorLight + vec3( -1.0f,  0.0f,  0.0f);
			upVector = vec3(0.0f, -1.0f,  0.0f);
			break;
		case 2:
			/// Positive Y
			directionalVectorLight = positionVectorLight + vec3( 0.0f,  1.0f,  0.0f);
			upVector = vec3(0.0f, 0.0f,  1.0f);
			break;
		case 3:
			/// Negative Y
			directionalVectorLight = positionVectorLight + vec3( 0.0f,  -1.0f,  0.0f);
			upVector = vec3(0.0f, 0.0f,  -1.0f);
			break;
		case 4:
			/// Positive Z
			directionalVectorLight = positionVectorLight + vec3( 0.0f,  0.0f,  1.0f);
			upVector = vec3(0.0f, -1.0f,  0.0f);
			break;
			/// Negative Z
		case 5:
			directionalVectorLight = positionVectorLight + vec3( 0.0f,  0.0f,  -1.0f);
			upVector = vec3(0.0f, -1.0f,  0.0f);
			break;
		default:
			break;
		}

		mat4 projectionMatrixCubeShadowMap = Perspective(Radians(90.0f), (float)SHADOW_MAP_SIZE / (float)SHADOW_MAP_SIZE, 0.3f, 100.0f);

		mat4 viewMatrixLight = LookAtMain(positionVectorLight,
										  directionalVectorLight,
										  upVector);

		return viewMatrixLight * projectionMatrixCubeShadowMap;
	}

	[[nodiscard]] SlotData ScenePresenter::updateDataUBO_UI(const unsigned int currentInventoryRow, const unsigned int currentInventoryColumn,
													ecs::components::inventory* inventoryComponent,
													ecs::components::transform* slotTransfromComponent,
													ecs::components::mesh*      meshComponent) {
		SlotData hudUBO{};
		mat4 model(1.0);
		const float fullSlotScale     = meshComponent->gltf ? inventoryComponent->slotScale * 2.0f : inventoryComponent->slotScale;
		const float x = slotTransfromComponent->position[0] + currentInventoryColumn * fullSlotScale;
		const float y_scaleMultilayer = frame_.aspectRate * fullSlotScale;
		const float y = slotTransfromComponent->position[1] + currentInventoryRow * y_scaleMultilayer;
		const float inventorySlotScale = inventoryComponent->slotScale;
		model[0][0] = inventorySlotScale;
		model[1][1] = inventorySlotScale;
		model[2][2] = inventorySlotScale;
		model[3][0] = x;
		model[3][1] = y;
		model[3][2] = 0.1f;

		hudUBO.model = model;

		bool highLightedSlot = false;
		for ( unsigned int i = 0; i < inventoryComponent->highlightedSlots.GetSize(); ++i ) {
			if ( inventoryComponent->highlightedSlots[i] == currentInventoryRow * inventoryComponent->col + currentInventoryColumn ) {
				highLightedSlot = true;
				break;
			} else
				continue;
		}

		if ( inventoryComponent->highlightedSlots.GetSize() > 0 ) {
			if ( highLightedSlot ) {
				if ( inventoryComponent->isAvailableHighlightedSlots )
					hudUBO.color = { 0.0, 0.3, 0.0 };
				else
					hudUBO.color = { 0.3, 0.0, 0.0 };
			}
		} else {
			hudUBO.color = { 0.0, 0.0, 0.0 };
		}

		return hudUBO;
	}

	mat4 ScenePresenter::updateDataUBO_IconsUI(ecs::components::transform* itemTransfromComponent,
									   [[maybe_unused]] ecs::components::collider* itemColliderComponent,
									   ecs::components::item* itemComponent,
									   const unsigned int rowInventory,
									   const unsigned int columnInventory,
									   ecs::components::transform* inventoryTransformComponent,
									   ecs::components::mesh* itemMesh,
									   int itemEntity) {
		float x_result_offset = 0.0f;
		float y_result_offset = 0.0f;
		if ( itemComponent->occupiedSlots.GetSize() == 0 ) {
		} else {
			const unsigned int inventorySlotEntity_0 = itemComponent->occupiedSlots[0];
			const unsigned int inventorySlotEntity_3 = itemComponent->occupiedSlots.GetHead();
			const unsigned int rowIndexFirstSlot = inventorySlotEntity_0 / rowInventory;
			const unsigned int colIndexFirstSlot = inventorySlotEntity_0 % columnInventory;
			const unsigned int rowIndexSecondSlot = inventorySlotEntity_3 / rowInventory;
			const unsigned int colIndexSecondSlot = inventorySlotEntity_3 % columnInventory;

			const float itemScale             = itemTransfromComponent->scale;
			const float fullSlotScale         = itemMesh->gltf ? itemScale * 2.0f : itemScale;
			constexpr float centreMultiplayer = 0.5f;                                                                   ///< Eather division by 2.0f using multiply on 0.5f
			x_result_offset = inventoryTransformComponent->position[0] + (colIndexFirstSlot * fullSlotScale + colIndexSecondSlot * fullSlotScale) * centreMultiplayer;
			y_result_offset = inventoryTransformComponent->position[1] + (rowIndexFirstSlot * fullSlotScale + rowIndexSecondSlot * fullSlotScale) * centreMultiplayer * frame_.aspectRate;
		}
		float itemScale = itemTransfromComponent->scale;

		if ( frame_.dragedItemEntity != itemEntity ) {
			itemTransfromComponent->position = vec3(x_result_offset, y_result_offset, 0.1f);
		} else {
//			std::cout << "item entity: " << itemEntity << std::endl;
			itemScale *= 1.1f;
//			itemColliderComponent->itemDrag = false;
			itemTransfromComponent->position[2] = 0.0f;
		}

//		std::cout << itemTransfromComponent->position << std::endl;

		mat4 model(1.0);
		model[0][0] = itemScale * itemComponent->itemSlotType.width;
		model[1][1] = itemScale * itemComponent->itemSlotType.height;
		model[2][2] = 0.0f;
		model[3][0] = itemTransfromComponent->position[0];
		model[3][1] = itemTransfromComponent->position[1];
		model[3][2] = itemTransfromComponent->position[2];

		return model;
	}

	mat4 ScenePresenter::updateDataHudScreenUBO( ecs::components::transform* cursorTransform ) {
		mat4 model;
		vec3 defaultPosition = vec3(0.0, 0.0, 0.0);

#ifndef __linux__
//		frame_.hud_screen_x = -frame_.hud_screen_x;
#endif

		cursorTransform->position[0] = frame_.hud_screen_x;
		cursorTransform->position[1] = -frame_.hud_screen_y;
//		std::cout << "cursor scale: " << cursorTransform->fScale << std::endl;

//		std::cout << "x: " << cursorTransform->tPosition[0] << " y: " << cursorTransform->tPosition << std::endl;

		if ( !frame_.isInventoryOpened ) {
			model[3][0] = defaultPosition[0];
			model[3][1] = defaultPosition[1];
			model[3][2] = defaultPosition[2];
			model[0][0] = cursorTransform->scale;
			model[1][1] = cursorTransform->scale;
			model[2][2] = cursorTransform->scale;
			model[3][3] = 1.0f;
		} else {
			defaultPosition[0] = frame_.hud_screen_x;
#ifdef _WIN32
			defaultPosition[0] = -defaultPosition[0];
#endif
			defaultPosition[1] = -frame_.hud_screen_y;

			model[3][0] = defaultPosition[0];
			model[3][1] = defaultPosition[1];
			model[3][2] = defaultPosition[2];
			model[0][0] = cursorTransform->scale;
			model[1][1] = cursorTransform->scale;
			model[2][2] = cursorTransform->scale;
			model[3][3] = 1.0f;
		}

		return model;
	}

	mat4 ScenePresenter::computeModelMatrix(ecs::components::transform* _transformComponent, ecs::components::rotation* rotation) {
		mat4 rotationMatrix(1.0f);
        mat4 scalingMatrix(1.0f);
        mat4 translationMatrix(1.0f);

		scalingMatrix[0][0] = _transformComponent->scale;
		scalingMatrix[1][1] = _transformComponent->scale;
		scalingMatrix[2][2] = _transformComponent->scale;

		translationMatrix[3][0] = _transformComponent->position[0];
		translationMatrix[3][1] = _transformComponent->position[1];
		translationMatrix[3][2] = _transformComponent->position[2];
		translationMatrix[3][3] = 1.0f;

		float sinPitch = std::sin(Radians(-rotation->pitch / 2));
		float cosPitch = std::cos(Radians(-rotation->pitch / 2));
		float sinYaw = std::sin(Radians((rotation->yaw)  / 2));
		float cosYaw = std::cos(Radians((rotation->yaw)  / 2));

		Quaternion pitchQuat;
		Quaternion yawQuat;
		pitchQuat.w = cosPitch;
		pitchQuat.x = sinPitch;
		pitchQuat.y = 0.0f;
		pitchQuat.z = 0.0f;

		yawQuat.w = cosYaw;
		yawQuat.x = 0.0f;
		yawQuat.y = sinYaw;
		yawQuat.z = 0.0f;

//		Quaternion result;
		// result = multiplyQuaternion(pitchQuat, yawQuat);

//		std::cout << "pitch: " << _transformComponent->pitch << std::endl;
//		const float rotationAngleScaler = 1.0f / 180.0f;
		rotationMatrix = Rotate<float, 4, 3>(vec3(0.0, 1.0, 0.0), _transformComponent->pitch);

		// glm::quat rotation = glm::quat(cos(glm::radians(fPitch/2)),(glm::radians(fPitch/2))*1, 0,0);
		// glm::mat4 rotationMat = glm::mat4_cast(rotation);
		// // result = { rotation.w, rotation.x, rotation.y, rotation.z };
		// // rotationMatrix = rotateQuaternion<float, 4>(result);
		// for ( aunsigned int i = 0; i < 4; ++i )
		// 	for ( unsigned int j = 0; j < 4; ++j )
		// 		rotationMatrix[i][j] = rotationMat[i][j];

        return rotationMatrix * scalingMatrix * translationMatrix;
	}

	void ScenePresenter::computeHudScreeenCoordinates() {
        frame_.hud_screen_y -= event_.mousePointerPosition.offset_Y / 1080.0f;
        frame_.hud_screen_x += event_.mousePointerPosition.offset_X / 1920.0f;

		if ( frame_.hud_screen_x > 1.0f )
			frame_.hud_screen_x = 1.0f;
		else if ( frame_.hud_screen_x < -1.0f )
			frame_.hud_screen_x = -1.0f;

		if ( frame_.hud_screen_y > 1.0f )
			frame_.hud_screen_y = 1.0f;
		else if ( frame_.hud_screen_y < -1.0f )
			frame_.hud_screen_y = -1.0f;
	}

}
