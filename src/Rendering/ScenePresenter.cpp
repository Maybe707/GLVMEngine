// This file is part of Game Loop Versatile Modules (GLVM)
// Copyright © 2024 Maksim Manokhin a.k.a. Yuriorkis_Scream. Contacts: <fellfrostqtw@gmail.com>
// Author: Maksim Manokhin a.k.a. Yuriorkis_Scream
// License: http://opensource.org/licenses/MIT

#include "Rendering/ScenePresenter.hpp"
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
ScenePresenter::ScenePresenter(ecs::arch::World& world, const RenderAssets& assets,
                               RenderFrame& frame, CEvent& event, CStack& input)
    : world_(world), assets_(assets), frame_(frame), event_(event), input_(input) {
    world_.searchCacheArchetypes(animatedActorsRequiredMask, cachedAnimationActorsArchetypes, animationActorsArchetypesNumber);
    world_.searchCacheArchetypes(staticActorsRequiredMask, cachedStaticActorsArchetypes, staticActorsArchetypesNumber);
    const auto& grid = world_.spatialGrid;
    frame_.renderSpacialGrid = {grid.grid[0][0][0].size, static_cast<float>(grid.width),
                              static_cast<float>(grid.height), static_cast<float>(grid.depth)};
}
void ScenePresenter::advanceAnimations(float seconds) { EnlargeFrameAccumulator(seconds); }
void ScenePresenter::updatePointer() { computeHudScreeenCoordinates(); }
void ScenePresenter::prepareFrame() {
    setFrameData();
    if (!frame_.isInventoryOpened) {
        SetViewMatrix();
        SetProjectionMatrix();
    }
}
	void ScenePresenter::setFrameData() {
		namespace cm = GLVM::ecs::components;
		namespace arch = GLVM::ecs::arch;

		frame_.directionalLights.clear();
		directionalLightArchetypesNumber = 0;
		world_.searchCacheArchetypes( directionalLightRequiredMask, cachedDirectionalLigthArchetypes, directionalLightArchetypesNumber );

		uint32_t directionalLightCounter = 0;
		for( uint32_t x = 0; x < directionalLightArchetypesNumber; ++x ) {
			arch::Archetype* arch = cachedDirectionalLigthArchetypes[x];
			cm::directionalLight*  directionalLights = (ecs::components::directionalLight*)arch->
				components[arch::ComponentsIndices::DIRECTIONAL_LIGHT_COMPONENT];

			for( uint32_t x1 = 0; x1 < arch->entityCount; ++x1 ) {
				if( directionalLights ) {
					frame_.directionalLights.Push({});
					cm::directionalLight* directionalLightComponent = &directionalLights[x1];
					frame_.directionalLights[directionalLightCounter].DirectionalLightSpaceMatrix =
						updateDirectionalLightSpaceMatrixShadowMapUBO( directionalLightComponent );
					frame_.directionalLights[directionalLightCounter].position = vec4(directionalLightComponent->position[0],
																							   directionalLightComponent->position[1],
																							   directionalLightComponent->position[2], 0.0);
					frame_.directionalLights[directionalLightCounter].direction = vec4(directionalLightComponent->direction[0],
																								directionalLightComponent->direction[1],
																								directionalLightComponent->direction[2], 0.0);
					frame_.directionalLights[directionalLightCounter].ambient = vec4(directionalLightComponent->ambient[0],
																							  directionalLightComponent->ambient[1],
																							  directionalLightComponent->ambient[2], 0.0);
					frame_.directionalLights[directionalLightCounter].diffuse = vec4(directionalLightComponent->diffuse[0],
																							  directionalLightComponent->diffuse[1],
																							  directionalLightComponent->diffuse[2], 0.0);
					frame_.directionalLights[directionalLightCounter].specular = vec4(directionalLightComponent->specular[0],
																							   directionalLightComponent->specular[1],
																							   directionalLightComponent->specular[2], 0.0);
					++directionalLightCounter;
				}
			}
		}

		frame_.spotLights.clear();
		spotLightArchetypesNumber = 0;
		world_.searchCacheArchetypes( spotLightRequiredMask, cachedSpotLigthArchetypes, spotLightArchetypesNumber );

		uint32_t spotLightCounter = 0;
		for( uint32_t x = 0; x < spotLightArchetypesNumber; ++x ) {
			arch::Archetype* arch = cachedSpotLigthArchetypes[x];
			cm::spotLight*  spotLights = (ecs::components::spotLight*)arch->
				components[arch::ComponentsIndices::SPOT_LIGHT_COMPONENT];

			for( uint32_t x1 = 0; x1 < arch->entityCount; ++x1 ) {
				if( spotLights ) {
					frame_.spotLights.Push({});
					cm::spotLight* spotLightComponent = &spotLights[x1];
					frame_.spotLights[spotLightCounter].SpotLigthSpaceMatrix =
						updateSpotLightSpaceMatrixShadowMapUBO( spotLightComponent );
					frame_.spotLights[spotLightCounter].position    = spotLightComponent->position;
					frame_.spotLights[spotLightCounter].direction   = spotLightComponent->direction;
					frame_.spotLights[spotLightCounter].cutOff      = spotLightComponent->cutOff;
					frame_.spotLights[spotLightCounter].outerCutOff = spotLightComponent->outerCutOff;
					frame_.spotLights[spotLightCounter].ambient     = spotLightComponent->ambient;
					frame_.spotLights[spotLightCounter].diffuse     = spotLightComponent->diffuse;
					frame_.spotLights[spotLightCounter].specular    = spotLightComponent->specular;
					frame_.spotLights[spotLightCounter].constant    = spotLightComponent->constant;
					frame_.spotLights[spotLightCounter].linear      = spotLightComponent->linear;
					frame_.spotLights[spotLightCounter].quadratic   = spotLightComponent->quadratic;
					++spotLightCounter;
				}
			}
		}

		frame_.pointLights.clear();
		pointLightArchetypesNumber = 0;
		world_.searchCacheArchetypes( pointLightRequiredMask, cachedPointLigthArchetypes, pointLightArchetypesNumber );

		uint32_t pointLightCounter = 0;
		for( uint32_t x = 0; x < pointLightArchetypesNumber; ++x ) {
			arch::Archetype* arch = cachedPointLigthArchetypes[x];
			cm::pointLight* pointLights = (ecs::components::pointLight*)arch->
				components[arch::ComponentsIndices::POINT_LIGHT_COMPONENT];

			for( uint32_t x1 = 0; x1 < arch->entityCount; ++x1 ) {
				if( pointLights ) {
					frame_.pointLights.Push({});
					cm::pointLight* pointLightComponent = &pointLights[x1];
					uint32_t maxCubeMapLayers = 6;
					for ( uint32_t cubeMapLayerCounter = 0; cubeMapLayerCounter < maxCubeMapLayers; ++cubeMapLayerCounter ) {                      ///< 6 is a number of cube map layers.
						frame_.pointLights[pointLightCounter].pointLightSpaceMatrix[cubeMapLayerCounter] =
							updatePointLightSpaceMatrixShadowMapUBO( pointLightComponent, cubeMapLayerCounter );
					}
					frame_.pointLights[pointLightCounter].position  = pointLightComponent->position;
					frame_.pointLights[pointLightCounter].ambient   = pointLightComponent->ambient;
					frame_.pointLights[pointLightCounter].diffuse   = pointLightComponent->diffuse;
					frame_.pointLights[pointLightCounter].specular  = pointLightComponent->specular;
					frame_.pointLights[pointLightCounter].constant  = pointLightComponent->constant;
					frame_.pointLights[pointLightCounter].linear    = pointLightComponent->linear;
					frame_.pointLights[pointLightCounter].quadratic = pointLightComponent->quadratic;
					++pointLightCounter;
				}
			}
		}

		frame_.healthBars.clear();
		healthBarsArchetypesNumber = 0;
		world_.searchCacheArchetypes( healthBarsRequiredMask, cachedHealthBarsArchetypes, healthBarsArchetypesNumber );

		uint32_t healthBarCounter = 0;
		for( uint32_t x = 0; x < healthBarsArchetypesNumber; ++x ) {
			arch::Archetype* arch = cachedHealthBarsArchetypes[x];
			cm::transform* healthBarTransforms = (ecs::components::transform*)arch->
				components[arch::ComponentsIndices::TRANSFORM_COMPONENT];
			cm::mesh*      healthBarMeshes     = (ecs::components::mesh*)arch->
				components[arch::ComponentsIndices::MESH_COMPONENT];
			cm::health*    healthBars          = (ecs::components::health*)arch->
				components[arch::ComponentsIndices::HEALTH_COMPONENT];

			unsigned int uiVertexId = 0;
			if( ecs::arch::matchesRequiredMask( arch->mask, arch::playerComponentMask ) ) {
				uiVertexId = healthBarMeshes[0].handle.id;
			}

			uiVertexId = 0;               ///< TODO: Need to consider another solution

			for ( unsigned int i = 0; i < arch->entityCount; ++i ) {
				if( !healthBars->randarable ) {
					continue;
				}

				frame_.healthBars.Push({});
//				unsigned int uiVertexId           = healthBarMeshes[i].handle.id;
				cm::transform* transformComponent = &healthBarTransforms[i];
				cm::health* healthComponent       = &healthBars[i];
				frame_.healthBars[healthBarCounter].meshID        = uiVertexId;
				frame_.healthBars[healthBarCounter].position      = transformComponent->position;
				frame_.healthBars[healthBarCounter].maxHealth     = healthComponent->maxHealth;
				frame_.healthBars[healthBarCounter].currentHealth = healthComponent->currentHealth;
				++healthBarCounter;
			}
		}

		frame_.fonts.clear();
		fontsArchetypesNumber = 0;
		world_.searchCacheArchetypes( fontRequiredMask, cachedFontsArchetypes, fontsArchetypesNumber );

		uint32_t fontCounter = 0;
		for( uint32_t x = 0; x < fontsArchetypesNumber; ++x ) {
			arch::Archetype* arch = cachedFontsArchetypes[x];
			cm::transform* fontTransforms = (ecs::components::transform*)arch->
				components[arch::ComponentsIndices::TRANSFORM_COMPONENT];
			cm::font*      fonts          = (ecs::components::font*)arch->
				components[arch::ComponentsIndices::FONT_COMPONENT];

			for ( unsigned int i = 0; i < arch->entityCount; ++i ) {
				frame_.fonts.Push({});
				cm::font* fontComponent           = &fonts[i];
				cm::transform* transformComponent = &fontTransforms[i];
				frame_.fonts[fontCounter].position    = transformComponent->position;
				frame_.fonts[fontCounter].font_string = fontComponent->font_string;
				frame_.fonts[fontCounter].lifeTime    = fontComponent->lifeTime;
				++fontCounter;
			}
		}

		if ( frame_.isInventoryOpened ) {
			frame_.inventories.clear();
			uint32_t inventoryCounter = 0;
			inventoryArchetypesNumber = 0;
			world_.searchCacheArchetypes( inventoryRequiredMask, cachedInventoryArchetypes, inventoryArchetypesNumber );

			for( uint32_t x = 0; x < inventoryArchetypesNumber; ++x ) {
				arch::Archetype* arch = cachedInventoryArchetypes[x];
				cm::transform* inventoryTransforms = (ecs::components::transform*)arch->
					components[arch::ComponentsIndices::TRANSFORM_COMPONENT];
				cm::inventory* inventory          = (ecs::components::inventory*)arch->
					components[arch::ComponentsIndices::INVENTORY_COMPONENT];
				cm::material*  inventoryMaterials = (ecs::components::material*)arch->
					components[arch::ComponentsIndices::MATERIAL_COMPONENT];
				cm::mesh*      inventoryMeshes    = (ecs::components::mesh*)arch->
					components[arch::ComponentsIndices::MESH_COMPONENT];

				if( inventoryTransforms && inventoryMaterials && inventory && inventoryMeshes ) {

					for ( unsigned int i = 0; i < arch->entityCount; ++i ) {
						frame_.inventories.Push({});
						cm::inventory* inventoryComponent = &inventory[i];
						unsigned int inventoryTextureID   = inventoryMaterials[i].diffuseTextureID_.id;
						unsigned int meshID           = inventoryComponent->slotMeshID.id;
						frame_.inventories[inventoryCounter].inventoryTextureID = inventoryTextureID;
						frame_.inventories[inventoryCounter].meshID             = meshID;
						frame_.inventories[inventoryCounter].row                = inventoryComponent->row;
						frame_.inventories[inventoryCounter].col                = inventoryComponent->col;
						frame_.inventories[inventoryCounter].slotData.clear();
						for ( unsigned int j = 0; j < inventoryComponent->row; ++j ) {
							for ( unsigned int m = 0; m < inventoryComponent->col; ++m ) {
								cm::transform* slotTransformComponent     = &inventoryTransforms[i];
								frame_.inventories[inventoryCounter].slotData.Push({});
								frame_.inventories[inventoryCounter].slotData[j * inventoryComponent->col + m] =
									updateDataUBO_UI( j, m, inventoryComponent, slotTransformComponent, &inventoryMeshes[i] );
							}
						}
						++inventoryCounter;
					}

					for ( unsigned int i = 0; i < arch->entityCount; ++i ) {
						cm::inventory* inventoryComponent          = &inventory[i];
						cm::transform* inventoryTransformComponent = &inventoryTransforms[i];

						frame_.items.clear();
						uint32_t itemCounter = 0;
						itemArchetypesNumber = 0;
						world_.searchCacheArchetypes( itemRequiredMask, cachedItemArchetypes, itemArchetypesNumber );

						for( uint32_t c = 0; c < itemArchetypesNumber; ++c ) {
							arch::Archetype* arch = cachedItemArchetypes[c];
							cm::transform* itemTransforms = (ecs::components::transform*)arch->
								components[arch::ComponentsIndices::TRANSFORM_COMPONENT];
							cm::item*      items          = (ecs::components::item*)arch->
								components[arch::ComponentsIndices::ITEM_COMPONENT];
							cm::material*  itemMaterials  = (ecs::components::material*)arch->
								components[arch::ComponentsIndices::MATERIAL_COMPONENT];
							cm::mesh*      itemMeshes     = (ecs::components::mesh*)arch->
								components[arch::ComponentsIndices::MESH_COMPONENT];
							cm::collider*  itemColliders  = (ecs::components::collider*)arch->
								components[arch::ComponentsIndices::COLLIDER_COMPONENT];

							if( itemTransforms && itemMaterials && itemMeshes &&
								itemColliders && items) {
								for ( unsigned int a = 0; a < arch->entityCount; ++a ) {
									cm::item* itemComponent = &items[a];
									if( !itemComponent->isActor ) {
										frame_.items.Push({});
										unsigned int meshID = itemMeshes[a].handle.id;
										unsigned int diffuseTexureID = itemMaterials[a].diffuseTextureID_.id;
										frame_.items[itemCounter].meshID          = meshID;
										frame_.items[itemCounter].diffuseTexureID = diffuseTexureID;
										cm::transform* itemTransformComponent    = &itemTransforms[a];
										cm::collider* itemColliderComponent      = &itemColliders[a];

										if ( itemTransformComponent == nullptr )
											std::cout << "NULL POINTER" << std::endl;

										uint32_t itemEntity = arch->entities[a];
										frame_.items[itemCounter].model = updateDataUBO_IconsUI(itemTransformComponent,
																							   itemColliderComponent,
																							   itemComponent,
																							   inventoryComponent->row,
																							   inventoryComponent->col,
																							   inventoryTransformComponent,
																							   &itemMeshes[a],
																							   itemEntity);
										++itemCounter;
									}
								}
							}
						}
					}
				}
			}
		}

		frame_.crosshairs.clear();
		crosshairActorsArchetypesNumber = 0;
		world_.searchCacheArchetypes( crosshairRequiredMask, cachedCrosshairActorsArchetypes, crosshairActorsArchetypesNumber );

		for( uint32_t x = 0; x < crosshairActorsArchetypesNumber; ++x ) {
			arch::Archetype* arch = cachedCrosshairActorsArchetypes[x];
			cm::transform* crosshairTransforms = (ecs::components::transform*)arch->
				components[arch::ComponentsIndices::TRANSFORM_COMPONENT];
			cm::mesh*      crosshairMeshes     = (ecs::components::mesh*)arch->
				components[arch::ComponentsIndices::MESH_COMPONENT];

			for ( unsigned int i = 0; i < arch->entityCount; ++i ) {
				frame_.crosshairs.Push({});
				cm::transform* cursorTransform = &crosshairTransforms[i];
				unsigned int meshID            = crosshairMeshes[i].handle.id;
				frame_.crosshairs[i].meshID = meshID;
				frame_.crosshairs[i].model  = updateDataHudScreenUBO( cursorTransform );
			}
		}



		frame_.actors.clear();
		frame_.collisionsWireframes.clear();
		uint32_t collisionsWireframesCounter = 0;
		levelChunkActorsArchetypesNumber = 0;
		world_.searchCacheArchetypes( levelChunkRequiredMask, cachedLevelChunkActorsArchetypes, levelChunkActorsArchetypesNumber );

		uint32_t levelChunkActorsCounter = 0;
		for( uint32_t x = 0; x < levelChunkActorsArchetypesNumber; ++x ) {
			arch::Archetype* arch = cachedLevelChunkActorsArchetypes[x];
			cm::transform* levelChunkTransforms = (ecs::components::transform*)arch->
				components[arch::ComponentsIndices::TRANSFORM_COMPONENT];
			cm::mesh*      levelChunkMeshes     = (ecs::components::mesh*)arch->
				components[arch::ComponentsIndices::MESH_COMPONENT];
			cm::material*  levelChunkMaterials  = (ecs::components::material*)arch->
				components[arch::ComponentsIndices::MATERIAL_COMPONENT];
			cm::rotation*  levelChunkRotations  = (ecs::components::rotation*)arch->
				components[arch::ComponentsIndices::ROTATION_COMPONENT];
			ecs::tagComponents::levelChunkTagComponent* levelChunks = (ecs::tagComponents::levelChunkTagComponent*)arch->
				components[arch::ComponentsIndices::LEVEL_CHUNK_TAG_COMPONENT];

			core::vector<mat4> jointMatrices;
			jointMatrices.Resize(MAX_JOINTS_NUMBER);
			for ( unsigned int i = 0; i < MAX_JOINTS_NUMBER; ++i ) {
				mat4 unitMatrix(1.0f);
				jointMatrices[i] = unitMatrix;
			}

			for( uint32_t n = 0; n < arch->entityCount; ++n ) {
				frame_.actors.Push({});
				frame_.collisionsWireframes.Push({});
				cm::transform* transformComponent = &levelChunkTransforms[n];
				cm::material*  materialComponent  = &levelChunkMaterials[n];
				cm::rotation*  rotationComponent  = &levelChunkRotations[n];
				if( levelChunkTransforms && levelChunkMaterials && levelChunks &&
					levelChunkRotations && levelChunkMeshes ) {
					const unsigned int meshID = levelChunkMeshes[n].handle.id;
//					cm::transform playerTransform = *transformComponent;
//					playerTransform.position += vec3(0.0f, 2.0f, -3.0f);
//					cm::rotation  playerRotation  = *rotationComponent;
					const mat4 model = computeModelMatrix(transformComponent, rotationComponent);
					frame_.collisionsWireframes[collisionsWireframesCounter].model    = model;
					frame_.collisionsWireframes[collisionsWireframesCounter].position = transformComponent->position;
					frame_.collisionsWireframes[collisionsWireframesCounter].scale    = transformComponent->scale;
					frame_.collisionsWireframes[collisionsWireframesCounter].meshAxisMaxAbsoluteValues = assets_.meshBounds[meshID];
					++collisionsWireframesCounter;

					frame_.actors[levelChunkActorsCounter].modelMatrix   = model;
					frame_.actors[levelChunkActorsCounter].jointMatrices = jointMatrices;
					frame_.actors[levelChunkActorsCounter].meshID        = meshID;
					frame_.actors[levelChunkActorsCounter].diffuseTextureIndex  = materialComponent->diffuseTextureID_.id;
					frame_.actors[levelChunkActorsCounter].specularTextureIndex = materialComponent->specularTextureID_.id;
					frame_.actors[levelChunkActorsCounter].ambient   = materialComponent->ambient;
					frame_.actors[levelChunkActorsCounter].shininess = materialComponent->shininess;
					++levelChunkActorsCounter;
				}
			}
		}

		animationActorsArchetypesNumber = 0;
		world_.searchCacheArchetypes( animationRequiredMask, cachedAnimationArchetypes, animationActorsArchetypesNumber );

		uint32_t animationActorsCounter = levelChunkActorsCounter;
		for( uint32_t x = 0; x < animationActorsArchetypesNumber; ++x ) {
			arch::Archetype* arch = cachedAnimationActorsArchetypes[x];
			cm::transform* actorTransforms = (ecs::components::transform*)arch->
				components[arch::ComponentsIndices::TRANSFORM_COMPONENT];
			cm::mesh*      actorMeshes     = (ecs::components::mesh*)arch->
				components[arch::ComponentsIndices::MESH_COMPONENT];
			cm::material*  actorMaterials  = (ecs::components::material*)arch->
				components[arch::ComponentsIndices::MATERIAL_COMPONENT];
			cm::rotation*  actorRotations  = (ecs::components::rotation*)arch->
				components[arch::ComponentsIndices::ROTATION_COMPONENT];
			cm::animation* actorAnimations = (ecs::components::animation*)arch->
				components[arch::ComponentsIndices::ANIMATION_COMPONENT];

			for( uint32_t n = 0; n < arch->entityCount; ++n ) {
				[[maybe_unused]] const u32 entity = arch->entities[n];
				frame_.actors.Push({});
				frame_.collisionsWireframes.Push({});
				cm::transform* transformComponent = &actorTransforms[n];
				cm::material*  materialComponent  = &actorMaterials[n];
				cm::mesh*      meshComponent      = &actorMeshes[n];
				[[maybe_unused]] cm::animation* animationComponent = &actorAnimations[n];
				cm::rotation*  rotationComponent  = &actorRotations[n];
				if( actorTransforms && actorMaterials &&
					actorAnimations && actorRotations ) {
					unsigned int meshID               = meshComponent->handle.id;
					const mat4 model = computeModelMatrix(transformComponent, rotationComponent);
					frame_.collisionsWireframes[collisionsWireframesCounter].model    = model;
					frame_.collisionsWireframes[collisionsWireframesCounter].position = transformComponent->position;
					frame_.collisionsWireframes[collisionsWireframesCounter].scale    = transformComponent->scale;
					frame_.collisionsWireframes[collisionsWireframesCounter].meshAxisMaxAbsoluteValues = assets_.meshBounds[meshID];
					++collisionsWireframesCounter;

//					std::cout << "frame movement: " << transformComponent->frameMovement << std::endl;
					if( std::abs(transformComponent->frameMovement[0]) > 0.0 ||
						std::abs(transformComponent->frameMovement[2]) > 0.0 ) {
//						std::cout << "frame move" << transformComponent->frameMovement << std::endl;
//						std::cout << "dot: " << clamp(-1.0f, Dot(Normalize(vec3(transformComponent->frameMovement[0], 0.0,
//																						 transformComponent->frameMovement[2])),
//																		  Normalize(vec3(transformComponent->forward[0], 0.0,
//																						 transformComponent->forward[2]))), 1.0f) << std::endl;



						const vec2 tempFrameMovement = vec2(transformComponent->frameMovement[0], transformComponent->frameMovement[2]);

//						std::cout << "forward: " << transformComponent->forward << std::endl;
//						std::cout << "movement x: " << tempFrameMovement[0] << " y: " << tempFrameMovement[1] << std::endl;

						float sign = cross<float>(vec2(tempFrameMovement[0], tempFrameMovement[1]),
										   vec2(transformComponent->forward[0], transformComponent->forward[2]));

//						std::cout << "sign: " << sign << std::endl;

						const float rotationAngle = acos(clamp(-1.0f, Dot(Normalize(vec3(transformComponent->frameMovement[0], 0.0,
																						 transformComponent->frameMovement[2])),
																		  Normalize(vec3(transformComponent->forward[0], 0.0,
																						 transformComponent->forward[2]))), 1.0f));



//						input_.PrintStack();

						int currentFrameEvents[4];

						if((input_.SearchElement(EEvents::eMOVE_FORWARD)) == EEvents::eMOVE_FORWARD) {
							currentFrameEvents[0] = eMOVE_FORWARD;
						} else {
							currentFrameEvents[0] = 0;
						}
						if((input_.SearchElement(EEvents::eMOVE_BACKWARD)) == EEvents::eMOVE_BACKWARD) {
							currentFrameEvents[1] = eMOVE_BACKWARD;
						} else {
							currentFrameEvents[1] = 0;
						}
						if((input_.SearchElement(EEvents::eMOVE_LEFT)) == EEvents::eMOVE_LEFT) {
							currentFrameEvents[2] = eMOVE_LEFT;
						} else {
							currentFrameEvents[2] = 0;
						}
						if((input_.SearchElement(EEvents::eMOVE_RIGHT)) == EEvents::eMOVE_RIGHT) {
							currentFrameEvents[3] = eMOVE_RIGHT;
						} else {
							currentFrameEvents[3] = 0;
						}

						// for( int i = 0; i < 4; ++i ) {
						// 	std::cout << "shtuka " << i << " :" << previousFrameKeyEvents[i] << std::endl;
						// }

						if( currentFrameEvents[0] == previousFrameKeyEvents[0] &&
							currentFrameEvents[1] == previousFrameKeyEvents[1] &&
							currentFrameEvents[2] == previousFrameKeyEvents[2] &&
							currentFrameEvents[3] == previousFrameKeyEvents[3] ) {
//							std::cout << "Default case " << std::endl;
//							std::this_thread::sleep_for(std::chrono::milliseconds(200));
						} else if ( currentFrameEvents[0] == 0 &&
									currentFrameEvents[1] == 0 &&
									currentFrameEvents[2] == 0 &&
									currentFrameEvents[3] == 0 ) {
//							std::cout << "Second default case " << std::endl;
//							std::this_thread::sleep_for(std::chrono::milliseconds(200));
						} else {
							previousFrameKeyEvents[0] = currentFrameEvents[0];
							previousFrameKeyEvents[1] = currentFrameEvents[1];
							previousFrameKeyEvents[2] = currentFrameEvents[2];
							previousFrameKeyEvents[3] = currentFrameEvents[3];
//							std::cout << "Rewrite key events " << std::endl;
//							std::this_thread::sleep_for(std::chrono::milliseconds(200));
//						sleep(1);
//						if( abs(rotationAngle) > 0.1 ) {
//							std::cout << "rot angle: " << rotationAngle << std::endl;
//							if

							if( sign > 0.0f ) {
								sign = 1.0f;
							} else if( sign < 0.0f ) {
								sign = -1.0f;
							} else {
								sign = 1.0f;
							}


							transformComponent->pitch += rotationAngle * sign;
//						}

							transformComponent->forward = transformComponent->frameMovement;
							transformComponent->previousFrameForward = transformComponent->forward;
//							transformComponent->frameMovement = 0;
						}
					} else {
						previousFrameKeyEvents[0] = 0.0f;
						previousFrameKeyEvents[1] = 0.0f;
						previousFrameKeyEvents[2] = 0.0f;
						previousFrameKeyEvents[3] = 0.0f;
					}
					// if( ecs::arch::getId( entity ) == 0 ) {
					// 	std::cout << "forward" << transformComponent->forward << std::endl;
					// }

					frame_.actors[animationActorsCounter].modelMatrix   = computeModelMatrix(transformComponent, rotationComponent);
					if( animationComponent->isAnimatedOnFrame ) {
						frame_.actors[animationActorsCounter].jointMatrices = updateAnimationFrames(animationComponent, meshID);
						animationComponent->jointMatrices = frame_.actors[animationActorsCounter].jointMatrices;
						animationComponent->isAnimatedOnFrame = false;
					} else {
						frame_.actors[animationActorsCounter].jointMatrices = animationComponent->jointMatrices;
					}
					frame_.actors[animationActorsCounter].meshID        = meshID;
					frame_.actors[animationActorsCounter].diffuseTextureIndex  = materialComponent->diffuseTextureID_.id;
					frame_.actors[animationActorsCounter].specularTextureIndex = materialComponent->specularTextureID_.id;
					frame_.actors[animationActorsCounter].ambient   = materialComponent->ambient;
					frame_.actors[animationActorsCounter].shininess = materialComponent->shininess;
					++animationActorsCounter;
				}
			}
		}

		staticActorsArchetypesNumber = 0;
		world_.searchCacheArchetypes( staticActorsRequiredMask, cachedStaticActorsArchetypes, staticActorsArchetypesNumber );

		uint32_t staticActorsCounter = animationActorsCounter;
		for( uint32_t x = 0; x < staticActorsArchetypesNumber; ++x ) {
			arch::Archetype* arch = cachedStaticActorsArchetypes[x];
			cm::transform* staticActorTransforms = (ecs::components::transform*)arch->
				components[arch::ComponentsIndices::TRANSFORM_COMPONENT];
			cm::mesh*      staticActorMeshes     = (ecs::components::mesh*)arch->
				components[arch::ComponentsIndices::MESH_COMPONENT];
			cm::material*  staticActorMaterials  = (ecs::components::material*)arch->
				components[arch::ComponentsIndices::MATERIAL_COMPONENT];
			cm::rotation*  staticActorRotations  = (ecs::components::rotation*)arch->
				components[arch::ComponentsIndices::ROTATION_COMPONENT];

			core::vector<mat4> jointMatrices;
			jointMatrices.Resize(MAX_JOINTS_NUMBER);
			for ( unsigned int i = 0; i < MAX_JOINTS_NUMBER; ++i ) {
				mat4 unitMatrix(1.0f);
				jointMatrices[i] = unitMatrix;
			}

			for( uint32_t n = 0; n < arch->entityCount; ++n ) {
				frame_.actors.Push({});
				frame_.collisionsWireframes.Push({});
				cm::transform* transformComponent = &staticActorTransforms[n];
				cm::material*  materialComponent  = &staticActorMaterials[n];
				cm::rotation*  rotationComponent  = &staticActorRotations[n];
				if( staticActorTransforms && staticActorMaterials &&
					staticActorRotations && staticActorMeshes ) {
					unsigned int meshID = staticActorMeshes[n].handle.id;
					const mat4 model = computeModelMatrix(transformComponent, rotationComponent);
					frame_.collisionsWireframes[collisionsWireframesCounter].model    = model;
					frame_.collisionsWireframes[collisionsWireframesCounter].position = transformComponent->position;
					frame_.collisionsWireframes[collisionsWireframesCounter].scale    = transformComponent->scale;
					frame_.collisionsWireframes[collisionsWireframesCounter].meshAxisMaxAbsoluteValues = assets_.meshBounds[meshID];
					++collisionsWireframesCounter;

					frame_.actors[staticActorsCounter].modelMatrix   = computeModelMatrix(transformComponent, rotationComponent);
					frame_.actors[staticActorsCounter].jointMatrices = jointMatrices;
					frame_.actors[staticActorsCounter].meshID        = meshID;
					frame_.actors[staticActorsCounter].diffuseTextureIndex  = materialComponent->diffuseTextureID_.id;
					frame_.actors[staticActorsCounter].specularTextureIndex = materialComponent->specularTextureID_.id;
					frame_.actors[staticActorsCounter].ambient   = materialComponent->ambient;
					frame_.actors[staticActorsCounter].shininess = materialComponent->shininess;
					++staticActorsCounter;
				}
			}
		}

		projectileActorsArchetypesNumber = 0;
		world_.searchCacheArchetypes( projectileRequiredMask, cachedProjectileActorsArchetypes, projectileActorsArchetypesNumber );

		uint32_t projectileActorsCounter = staticActorsCounter;
		for( uint32_t x = 0; x < projectileActorsArchetypesNumber; ++x ) {
			arch::Archetype* arch = cachedProjectileActorsArchetypes[x];
			cm::transform*           actorTransforms         = (ecs::components::transform*)arch->
				components[arch::ComponentsIndices::TRANSFORM_COMPONENT];
			cm::mesh*                actorMeshes             = (ecs::components::mesh*)arch->
				components[arch::ComponentsIndices::MESH_COMPONENT];
			arch::ProjectileBundle*  actorProjectileBundles  = (arch::ProjectileBundle*)arch->
				components[arch::ComponentsIndices::PROJECTILE_BUNDLE_COMPONENT];
			cm::rotation*            actorRotations          = (ecs::components::rotation*)arch->
				components[arch::ComponentsIndices::ROTATION_COMPONENT];

			core::vector<mat4> jointMatrices;
			jointMatrices.Resize(MAX_JOINTS_NUMBER);
			for ( unsigned int i = 0; i < MAX_JOINTS_NUMBER; ++i ) {
				mat4 unitMatrix(1.0f);
				jointMatrices[i] = unitMatrix;
			}

			for( uint32_t n = 0; n < arch->entityCount; ++n ) {
				frame_.actors.Push({});
				frame_.collisionsWireframes.Push({});
				cm::transform* transformComponent = &actorTransforms[n];
				cm::material*  materialComponent  = &actorProjectileBundles[n].material;
				cm::rotation*  rotationComponent  = &actorRotations[n];
				if( actorTransforms && actorProjectileBundles &&
					actorRotations && actorMeshes ) {
					unsigned int meshID = actorMeshes[n].handle.id;
					const mat4 model = computeModelMatrix(transformComponent, rotationComponent);
					frame_.collisionsWireframes[collisionsWireframesCounter].model    = model;
					frame_.collisionsWireframes[collisionsWireframesCounter].position = transformComponent->position;
					frame_.collisionsWireframes[collisionsWireframesCounter].scale    = transformComponent->scale;
					frame_.collisionsWireframes[collisionsWireframesCounter].meshAxisMaxAbsoluteValues = assets_.meshBounds[meshID];
					++collisionsWireframesCounter;

					frame_.actors[projectileActorsCounter].modelMatrix   = computeModelMatrix(transformComponent, rotationComponent);
					frame_.actors[projectileActorsCounter].jointMatrices = jointMatrices;
					frame_.actors[projectileActorsCounter].meshID        = meshID;
					frame_.actors[projectileActorsCounter].diffuseTextureIndex  = materialComponent->diffuseTextureID_.id;
					frame_.actors[projectileActorsCounter].specularTextureIndex = materialComponent->specularTextureID_.id;
					frame_.actors[projectileActorsCounter].ambient   = materialComponent->ambient;
					frame_.actors[projectileActorsCounter].shininess = materialComponent->shininess;
					++projectileActorsCounter;
				}
			}
		}


		/*
		 =====================================
		 Item actors renders in the game world
		 =====================================
		 */

		itemActorsArchetypesNumber = 0;
		world_.searchCacheArchetypes( rotationItemRequiredMask, cachedItemActorsArchetypes, itemActorsArchetypesNumber );

		uint32_t itemActorsCounter = projectileActorsCounter;
		for( uint32_t x = 0; x < itemActorsArchetypesNumber; ++x ) {
			arch::Archetype* arch = cachedItemActorsArchetypes[x];
			cm::transform*           itemTransforms         = (ecs::components::transform*)arch->
				components[arch::ComponentsIndices::TRANSFORM_COMPONENT];
			cm::mesh*                itemMeshes             = (ecs::components::mesh*)arch->
				components[arch::ComponentsIndices::MESH_COMPONENT];
			cm::material*            itemMaterials          = (cm::material*)arch->
				components[arch::ComponentsIndices::MATERIAL_COMPONENT];
			cm::rotation*            itemRotations          = (ecs::components::rotation*)arch->
				components[arch::ComponentsIndices::ROTATION_COMPONENT];
			cm::item*                items                  = (ecs::components::item*)arch->
				components[arch::ComponentsIndices::ITEM_COMPONENT];


			core::vector<mat4> jointMatrices;
			jointMatrices.Resize(MAX_JOINTS_NUMBER);
			for ( unsigned int i = 0; i < MAX_JOINTS_NUMBER; ++i ) {
				mat4 unitMatrix(1.0f);
				jointMatrices[i] = unitMatrix;
			}

			for( uint32_t n = 0; n < arch->entityCount; ++n ) {
				if( items[n].isActor ) {
					frame_.actors.Push({});
					frame_.collisionsWireframes.Push({});
					cm::transform* transformComponent = &itemTransforms[n];
					cm::material*  materialComponent  = &itemMaterials[n];
					cm::rotation*  rotationComponent  = &itemRotations[n];
					if( itemTransforms && itemMaterials &&
						itemRotations && itemMeshes ) {
						unsigned int meshID = itemMeshes[n].handle.id;
						const mat4 model = computeModelMatrix(transformComponent, rotationComponent);
						frame_.collisionsWireframes[collisionsWireframesCounter].model    = model;
						frame_.collisionsWireframes[collisionsWireframesCounter].position = transformComponent->position;
						frame_.collisionsWireframes[collisionsWireframesCounter].scale    = transformComponent->scale;
						frame_.collisionsWireframes[collisionsWireframesCounter].meshAxisMaxAbsoluteValues = assets_.meshBounds[meshID];
						++collisionsWireframesCounter;

						frame_.actors[itemActorsCounter].modelMatrix   = computeModelMatrix(transformComponent, rotationComponent);
						frame_.actors[itemActorsCounter].jointMatrices = jointMatrices;
						frame_.actors[itemActorsCounter].meshID        = meshID;
						frame_.actors[itemActorsCounter].diffuseTextureIndex  = materialComponent->diffuseTextureID_.id;
						frame_.actors[itemActorsCounter].specularTextureIndex = materialComponent->specularTextureID_.id;
						frame_.actors[itemActorsCounter].ambient   = materialComponent->ambient;
						frame_.actors[itemActorsCounter].shininess = materialComponent->shininess;
						++itemActorsCounter;
					}
				}
			}
		}


		frame_.players.clear();
		playerArchetypesNumber = 0;
		world_.searchCacheArchetypes( playerRequiredMask, cachedPlayerArchetypes, playerArchetypesNumber );

		uint32_t playerEntityCount = 0;
		for( uint32_t x = 0; x < playerArchetypesNumber; ++x ) {
			arch::Archetype* arch = cachedPlayerArchetypes[x];
			cm::transform*   playerTransforms = (ecs::components::transform*)arch->
				components[arch::ComponentsIndices::TRANSFORM_COMPONENT];
			cm::rotation*    playerRotations = (ecs::components::rotation*)arch->
				components[arch::ComponentsIndices::ROTATION_COMPONENT];
			cm::mesh*        playerMeshes    = (ecs::components::mesh*)arch->
				components[arch::ComponentsIndices::MESH_COMPONENT];

			for( unsigned int n = 0; n < arch->entityCount; ++n ) {
				frame_.players.Push({});
				frame_.collisionsWireframes.Push({});
				cm::transform* playerTransformComponent = &playerTransforms[n];
				cm::rotation*  playerRotationComponent  = &playerRotations[n];
				if( &playerTransforms[n] != nullptr ) {
					const unsigned int meshID = playerMeshes[n].handle.id;
					const mat4 model = computeModelMatrix(playerTransformComponent, playerRotationComponent);
					frame_.collisionsWireframes[collisionsWireframesCounter].model    = model;
					frame_.collisionsWireframes[collisionsWireframesCounter].position = playerTransformComponent->position;
					frame_.collisionsWireframes[collisionsWireframesCounter].scale    = playerTransformComponent->scale;
					frame_.collisionsWireframes[collisionsWireframesCounter].meshAxisMaxAbsoluteValues = assets_.meshBounds[meshID];
					++collisionsWireframesCounter;

					frame_.players[playerEntityCount].position = playerTransformComponent->position;
					frame_.players[playerEntityCount].forward  = playerTransformComponent->forward;
				}
                ++playerEntityCount;
			}
		}
	}

}
