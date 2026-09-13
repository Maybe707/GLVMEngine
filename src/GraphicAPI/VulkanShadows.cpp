// This file is part of Game Loop Versatile Modules (GLVM)
// Copyright © 2024 Maksim Manokhin a.k.a. Yuriorkis_Scream. Contacts: <fellfrostqtw@gmail.com>
// Author: Maksim Manokhin a.k.a. Yuriorkis_Scream
// License: http://opensource.org/licenses/MIT

#include "ComponentManager.hpp"
#include "GraphicAPI/Vulkan.hpp"
#include "GraphicAPI/UniqueShaderModule.hpp"
#include "Components/ActorComponent.hpp"
#include "Components/AnimationMoveComponent.hpp"
#include "Components/ColliderComponent.hpp"
#include "Components/ControllerComponent.hpp"
#include "Components/HealthComponent.hpp"
#include "Components/InventoryComponent.hpp"
#include "Components/InventorySlotComponent.hpp"
#include "Components/ItemComponent.hpp"
#include "Components/MaterialComponent.hpp"
#include "Components/InterfaceComponent.hpp"
#include "Components/PointLightComponent.hpp"
#include "Components/TransformComponent.hpp"
#include "Components/VertexComponent.hpp"
#include "Components/ViewComponent.hpp"
#include "Components/CrosshairComponent.hpp"
#include "EntityManager.hpp"
#include "GraphicAPI/RenderConfig.hpp"
#include "GraphicAPI/RenderData.hpp"
#include "PGA.hpp"
#include "ShaderStructs.hpp"
#include "Texture.hpp"
#include "ThreadPool.hpp"
#include "Vector.hpp"
#include "VertexMath.hpp"
#include "VkStructs.hpp"
#include "WavefrontObjParser.hpp"
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <string>
#include <thread>
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_wayland.h>

namespace GLVM::core {
    void CVulkanRenderer::transitionShadowMapImageLayout(VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout) {
        VkCommandBuffer commandBuffer = beginSingleTimeCommands(directionalLightCommandPool);

        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = image;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;

        VkPipelineStageFlags sourceStage;
        VkPipelineStageFlags destinationStage;

        if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        } else {
            throw std::invalid_argument("unsupported layout transition!");
        }

        vkCmdPipelineBarrier(
            commandBuffer,
            sourceStage, destinationStage,
            0,
            0, nullptr,
            0, nullptr,
            1, &barrier
            );

        endSingleTimeCommands(directionalLightCommandPool, commandBuffer);
    }

    void CVulkanRenderer::updateDirectionalLightShadowMapMatrixUBO(uint32_t currentImage, uint32_t currentLight, unsigned int actor) {
		ShadowMapMatrixUBO modelMatrixUBO{};

		modelMatrixUBO.model = frame_.actors[actor].modelMatrix;
		modelMatrixUBO.lightSpaceMatrix = dirLightSpaceMatrix[currentLight];

		// for ( unsigned int j = 0; j < MAX_JOINTS_NUMBER; ++j ) {
		// 	modelMatrixUBO.jointMatrices[j] = frame_.actors[actor].jointMatrices[j];
		// }

		for ( unsigned int j = 0; j < frame_.actors[actor].jointMatrices.GetSize(); ++j ) {
			modelMatrixUBO.jointMatrices[j] = frame_.actors[actor].jointMatrices[j];
		}

        void* modelMatrixData = nullptr;
		unsigned int shadowMapDirectionalLightDescriptorBindingIndex = resources_.descriptorSetsConfig[DescriptorSetDataLink::SHADOW_MAP_DIRECTIONAL_LIGHT].descriptorsBindingsIDs[0];
        vkMapMemory(device, resources_.GPUDescriptors[resources_.descriptorBindingsConfig[shadowMapDirectionalLightDescriptorBindingIndex].globalDescriptorOffset].GPUBuffer->deviceMemory, currentImage * sizeof(modelMatrixUBO),
					sizeof(modelMatrixUBO), 0, &modelMatrixData);
        memcpy(modelMatrixData, &modelMatrixUBO, sizeof(modelMatrixUBO));
        vkUnmapMemory(device, resources_.GPUDescriptors[resources_.descriptorBindingsConfig[shadowMapDirectionalLightDescriptorBindingIndex].globalDescriptorOffset].GPUBuffer->deviceMemory);
    }

    void CVulkanRenderer::updateSpotLightShadowMapMatrixUBO(uint32_t currentImage, uint32_t currentLight, unsigned int actor) {
		ShadowMapMatrixUBO modelMatrixUBO{};

		modelMatrixUBO.model = frame_.actors[actor].modelMatrix;
		modelMatrixUBO.lightSpaceMatrix = spotLightSpaceMatrix[currentLight];

		// for ( unsigned int j = 0; j < MAX_JOINTS_NUMBER; ++j ) {
		// 	modelMatrixUBO.jointMatrices[j] = frame_.actors[actor].jointMatrices[j];
		// }

		for ( unsigned int j = 0; j < frame_.actors[actor].jointMatrices.GetSize(); ++j ) {
			modelMatrixUBO.jointMatrices[j] = frame_.actors[actor].jointMatrices[j];
		}

        void* modelMatrixData;
		unsigned int shadowMapSpotLightDescriptorBindingIndex = resources_.descriptorSetsConfig[DescriptorSetDataLink::SHADOW_MAP_SPOT_LIGHT].descriptorsBindingsIDs[0];
        vkMapMemory(device, resources_.GPUDescriptors[resources_.descriptorBindingsConfig[shadowMapSpotLightDescriptorBindingIndex].globalDescriptorOffset].GPUBuffer->deviceMemory, currentImage * sizeof(modelMatrixUBO),
					sizeof(modelMatrixUBO), 0, &modelMatrixData);
        memcpy(modelMatrixData, &modelMatrixUBO, sizeof(modelMatrixUBO));
        vkUnmapMemory(device, resources_.GPUDescriptors[resources_.descriptorBindingsConfig[shadowMapSpotLightDescriptorBindingIndex].globalDescriptorOffset].GPUBuffer->deviceMemory);
    }

    void CVulkanRenderer::updatePointLightShadowMapMatrixUBO([[maybe_unused]] uint32_t currentImage, uint32_t currentLight, uint32_t layer, unsigned int actor) {
		PointLightShadowMapMatrixUBO modelMatrixUBO{};

		modelMatrixUBO.model = frame_.actors[actor].modelMatrix;

//		projectionMatrixCubeShadowMap[1][1] *= -1;

		modelMatrixUBO.lightSpaceMatrix = frame_.pointLights[currentLight].pointLightSpaceMatrix[layer];
		modelMatrixUBO.farPlane = 100.0f;
		modelMatrixUBO.lightPosition = frame_.pointLights[currentLight].position;

		// for ( unsigned int j = 0; j < MAX_JOINTS_NUMBER; ++j ) {
		// 	modelMatrixUBO.jointMatrices[j] = frame_.actors[actor].jointMatrices[j];
		// }

		for ( unsigned int j = 0; j < frame_.actors[actor].jointMatrices.GetSize(); ++j ) {
			modelMatrixUBO.jointMatrices[j] = frame_.actors[actor].jointMatrices[j];
		}

        void* modelMatrixData;
		unsigned int shadowMapPointLightDescriptorBindingIndex = resources_.descriptorSetsConfig[DescriptorSetDataLink::SHADOW_MAP_POINT_LIGHT].descriptorsBindingsIDs[0];
        vkMapMemory(device, resources_.GPUDescriptors[resources_.descriptorBindingsConfig[shadowMapPointLightDescriptorBindingIndex].globalDescriptorOffset].GPUBuffer->deviceMemory, currentImage * sizeof(modelMatrixUBO),
					sizeof(modelMatrixUBO), 0, &modelMatrixData);
        memcpy(modelMatrixData, &modelMatrixUBO, sizeof(modelMatrixUBO));
        vkUnmapMemory(device, resources_.GPUDescriptors[resources_.descriptorBindingsConfig[shadowMapPointLightDescriptorBindingIndex].globalDescriptorOffset].GPUBuffer->deviceMemory);
    }

    void CVulkanRenderer::directionalLightShadowMapDrawFrame() {
		namespace cm = GLVM::ecs::components;
        vkWaitForFences(device, 1, &directionalLightShadowMapInFlightFences[directionalLightCurrentFrame], VK_TRUE, UINT64_MAX);

        [[maybe_unused]] uint32_t imageIndex = 0;

        vkResetFences(device, 1, &directionalLightShadowMapInFlightFences[directionalLightCurrentFrame]);
        vkResetCommandBuffer(directionalLightCommandBuffers[directionalLightCurrentFrame], /*VkCommandBufferResetFlagBits*/ 0);
//        directionalLightRecordCoomandBuffer(directionalLightCommandBuffers[directionalLightCurrentFrame], imageIndex);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &directionalLightCommandBuffers[directionalLightCurrentFrame];

        if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, directionalLightShadowMapInFlightFences[directionalLightCurrentFrame]) != VK_SUCCESS) {
            throw std::runtime_error("failed to submit draw command buffer!");
        }

        directionalLightCurrentFrame = (directionalLightCurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }

	void CVulkanRenderer::spotLightShadowMapDrawFrame() {
		namespace cm = GLVM::ecs::components;
        vkWaitForFences(device, 1, &spotLightShadowMapInFlightFences[spotLightCurrentFrame], VK_TRUE, UINT64_MAX);

        [[maybe_unused]] uint32_t imageIndex = 0;

        vkResetFences(device, 1, &spotLightShadowMapInFlightFences[spotLightCurrentFrame]);
        vkResetCommandBuffer(spotLightCommandBuffers[spotLightCurrentFrame], /*VkCommandBufferResetFlagBits*/ 0);
//        spotLightRecordCommandBuffer(spotLightCommandBuffers[spotLightCurrentFrame], imageIndex);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &spotLightCommandBuffers[spotLightCurrentFrame];

        if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, spotLightShadowMapInFlightFences[spotLightCurrentFrame]) != VK_SUCCESS) {
            throw std::runtime_error("failed to submit draw command buffer!");
        }

        spotLightCurrentFrame = (spotLightCurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }

	    void CVulkanRenderer::pointLightShadowMapDrawFrame() {
		namespace cm = GLVM::ecs::components;
        vkWaitForFences(device, 1, &pointLightShadowMapInFlightFences[pointLightCurrentFrame], VK_TRUE, UINT64_MAX);

        [[maybe_unused]] uint32_t imageIndex = 0;

        vkResetFences(device, 1, &pointLightShadowMapInFlightFences[pointLightCurrentFrame]);
        vkResetCommandBuffer(pointLightCommandBuffers[pointLightCurrentFrame], /*VkCommandBufferResetFlagBits*/ 0);
//        pointLightRecordCommandBuffer(pointLightCommandBuffers[pointLightCurrentFrame], imageIndex);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &pointLightCommandBuffers[pointLightCurrentFrame];

        if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, pointLightShadowMapInFlightFences[pointLightCurrentFrame]) != VK_SUCCESS) {
            throw std::runtime_error("failed to submit draw command buffer!");
        }

        pointLightCurrentFrame = (pointLightCurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }

		void CVulkanRenderer::directionalLightRecordCoomandBuffer(std::vector<VkCommandBuffer>& commandBuffers, [[maybe_unused]] uint32_t currentFrame) {
		for ( uint32_t directionalLightCounter = 0; directionalLightCounter < frame_.directionalLights.GetSize(); ++ directionalLightCounter ) {
			VkCommandBufferInheritanceInfo inheritanceInfo{};
			inheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
			inheritanceInfo.renderPass = resources_.renderPasses[SpecificPipeline::DIRECTIONAL_LIGHT_PIPELINE];
			inheritanceInfo.framebuffer = directionalLightShadowMapFrameBuffers[directionalLightCounter];
			inheritanceInfo.subpass = 0;

			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.flags = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT |
				VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
			beginInfo.pInheritanceInfo = &inheritanceInfo;

			VkCommandBuffer commandBuffer = commandBuffers[currentFrame * directionalLightNumber + directionalLightCounter];
			if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
				throw std::runtime_error("failed to begin recording command buffer!");
			}

			// VkClearValue shadowMapClearValues[1];
			// shadowMapClearValues[0].depthStencil.depth = 1.0f;
			// shadowMapClearValues[0].depthStencil.stencil = 0;

			// VkRenderPassBeginInfo shadowMapRenderPassInfo{};
			// shadowMapRenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			// shadowMapRenderPassInfo.pNext = NULL;
			// shadowMapRenderPassInfo.renderPass = resources_.renderPasses[SpecificPipeline::DIRECTIONAL_LIGHT_PIPELINE];
			// shadowMapRenderPassInfo.framebuffer = directionalLightShadowMapFrameBuffers[directionalLightCounter];
			// shadowMapRenderPassInfo.renderArea.offset.x = 0;
			// shadowMapRenderPassInfo.renderArea.offset.y = 0;
			// shadowMapRenderPassInfo.renderArea.extent.width = swapChainExtent.width;
			// shadowMapRenderPassInfo.renderArea.extent.height = swapChainExtent.height;
			// shadowMapRenderPassInfo.clearValueCount = 1;
			// shadowMapRenderPassInfo.pClearValues = shadowMapClearValues;

			// vkCmdBeginRenderPass(commandBuffer, &shadowMapRenderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

			VkViewport shadowMapViewPort;
			shadowMapViewPort.height = swapChainExtent.height;
			shadowMapViewPort.width = swapChainExtent.width;
			shadowMapViewPort.minDepth = 0.0f;
			shadowMapViewPort.maxDepth = 1.0f;
			shadowMapViewPort.x = 0;
			shadowMapViewPort.y = 0;
			vkCmdSetViewport(commandBuffer, 0, 1, &shadowMapViewPort);

			VkRect2D shadowMapScissor;
			shadowMapScissor.extent.width = swapChainExtent.width;
			shadowMapScissor.extent.height = swapChainExtent.height;
			shadowMapScissor.offset.x = 0;
			shadowMapScissor.offset.y = 0;
			vkCmdSetScissor(commandBuffer, 0, 1, &shadowMapScissor);

			vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, resources_.pipelineConfigs[SpecificPipeline::DIRECTIONAL_LIGHT_PIPELINE].pipeline);
			dirLightSpaceMatrix[directionalLightCounter] = frame_.directionalLights[directionalLightCounter].DirectionalLightSpaceMatrix;

			uint32_t actorsNumber = frame_.actors.GetSize();
			for ( unsigned int actorCounter = 0; actorCounter < actorsNumber; ++actorCounter ) {
				RenderActor actor = frame_.actors[actorCounter];
				unsigned int meshId = actor.meshID;

				unsigned int uboDirectionalLightIndex = (resources_.descriptorSetsConfig[SHADOW_MAP_DIRECTIONAL_LIGHT].hostDescriptorNumber / MAX_FRAMES_IN_FLIGHT) * currentFrame +
					actorsNumber * directionalLightCounter + actorCounter;

				updateDirectionalLightShadowMapMatrixUBO(uboDirectionalLightIndex, directionalLightCounter, actorCounter);
				const unsigned int linkedDescriptorSetID = resources_.pipelineConfigs[SpecificPipeline::DIRECTIONAL_LIGHT_PIPELINE].linkedDescriptorSetIDs[0];
				const DescriptorSet& currentDescriptorSet = resources_.descriptorSetsConfig[linkedDescriptorSetID];
				vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, resources_.pipelineConfigs[SpecificPipeline::DIRECTIONAL_LIGHT_PIPELINE].pipelineLayout, 0, 1,
										&(*(resources_.descriptorSetsChunks.GetVectorContainer() + currentDescriptorSet.descriptorSetOffset + uboDirectionalLightIndex)), 0, nullptr);

				VkBuffer vertexBuffers[] = {vertexBufferContainer[meshId]};
				VkDeviceSize offsets[] = {0};
				vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

				vkCmdBindIndexBuffer(commandBuffer, indexBufferContainer[meshId], 0, VK_INDEX_TYPE_UINT32);

				unsigned int indicesContainerSize = assets_.aIndices_[meshId].size();
				vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indicesContainerSize), 1, 0, 0, 0);
			}

			if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
				throw std::runtime_error("failed to record command buffer!");
			}
//			vkCmdEndRenderPass(commandBuffer);
		}
	}

		void CVulkanRenderer::spotLightRecordCommandBuffer(std::vector<VkCommandBuffer>& commandBuffers, [[maybe_unused]] uint32_t currentFrame) {
		for ( uint32_t spotLightCounter = 0; spotLightCounter < frame_.spotLights.GetSize(); ++ spotLightCounter ) {
			VkCommandBufferInheritanceInfo inheritanceInfo{};
			inheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
			inheritanceInfo.renderPass = resources_.renderPasses[SpecificPipeline::SPOT_LIGHT_PIPELINE];
			inheritanceInfo.framebuffer = spotLightShadowMapFrameBuffers[spotLightCounter];
			inheritanceInfo.subpass = 0;

			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.flags = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT |
				VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
			beginInfo.pInheritanceInfo = &inheritanceInfo;

			VkCommandBuffer commandBuffer = commandBuffers[currentFrame * spotLightNumber + spotLightCounter];
			if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
				throw std::runtime_error("failed to begin recording command buffer!");
			}

			// VkClearValue spotLightShadowMapClearValues[1];
			// spotLightShadowMapClearValues[0].depthStencil.depth = 1.0f;
			// spotLightShadowMapClearValues[0].depthStencil.stencil = 0;

			// VkRenderPassBeginInfo spotLightShadowMapRenderPassInfo{};
			// spotLightShadowMapRenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			// spotLightShadowMapRenderPassInfo.pNext = NULL;
			// spotLightShadowMapRenderPassInfo.renderPass = resources_.renderPasses[SpecificPipeline::SPOT_LIGHT_PIPELINE];
			// spotLightShadowMapRenderPassInfo.framebuffer = spotLightShadowMapFrameBuffers[spotLightCounter];
			// spotLightShadowMapRenderPassInfo.renderArea.offset.x = 0;
			// spotLightShadowMapRenderPassInfo.renderArea.offset.y = 0;
			// spotLightShadowMapRenderPassInfo.renderArea.extent.width = swapChainExtent.width;
			// spotLightShadowMapRenderPassInfo.renderArea.extent.height = swapChainExtent.height;
			// spotLightShadowMapRenderPassInfo.clearValueCount = 1;
			// spotLightShadowMapRenderPassInfo.pClearValues = spotLightShadowMapClearValues;

			// vkCmdBeginRenderPass(commandBuffer, &spotLightShadowMapRenderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

			VkViewport spotLightShadowMapViewPort;
			spotLightShadowMapViewPort.height = swapChainExtent.height;
			spotLightShadowMapViewPort.width = swapChainExtent.width;
			spotLightShadowMapViewPort.minDepth = 0.0f;
			spotLightShadowMapViewPort.maxDepth = 1.0f;
			spotLightShadowMapViewPort.x = 0;
			spotLightShadowMapViewPort.y = 0;
			vkCmdSetViewport(commandBuffer, 0, 1, &spotLightShadowMapViewPort);

			VkRect2D spotLightShadowMapScissor;
			spotLightShadowMapScissor.extent.width = swapChainExtent.width;
			spotLightShadowMapScissor.extent.height = swapChainExtent.height;
			spotLightShadowMapScissor.offset.x = 0;
			spotLightShadowMapScissor.offset.y = 0;
			vkCmdSetScissor(commandBuffer, 0, 1, &spotLightShadowMapScissor);

			vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, resources_.pipelineConfigs[SpecificPipeline::SPOT_LIGHT_PIPELINE].pipeline);

			spotLightSpaceMatrix[spotLightCounter] = frame_.spotLights[spotLightCounter].SpotLigthSpaceMatrix;
			uint32_t actorsNumber = frame_.actors.GetSize();
			for ( unsigned int actorsCounter = 0; actorsCounter < actorsNumber; ++actorsCounter ) {
				RenderActor actor = frame_.actors[actorsCounter];
				unsigned int meshID = actor.meshID;
				unsigned int uboSpotLightIndex = (resources_.descriptorSetsConfig[SHADOW_MAP_SPOT_LIGHT].hostDescriptorNumber / MAX_FRAMES_IN_FLIGHT) * currentFrame +
					actorsNumber * spotLightCounter + actorsCounter;

				updateSpotLightShadowMapMatrixUBO(uboSpotLightIndex, spotLightCounter, actorsCounter);
				const unsigned int linkedDescriptorSetID = resources_.pipelineConfigs[SpecificPipeline::SPOT_LIGHT_PIPELINE].linkedDescriptorSetIDs[0];
				const DescriptorSet& currentDescriptorSet = resources_.descriptorSetsConfig[linkedDescriptorSetID];
				vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, resources_.pipelineConfigs[SpecificPipeline::SPOT_LIGHT_PIPELINE].pipelineLayout, 0, 1,
										&(*(resources_.descriptorSetsChunks.GetVectorContainer() + currentDescriptorSet.descriptorSetOffset + uboSpotLightIndex)), 0, nullptr);
				VkBuffer vertexBuffers[] = {vertexBufferContainer[meshID]};
				VkDeviceSize offsets[] = {0};
				vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

				vkCmdBindIndexBuffer(commandBuffer, indexBufferContainer[meshID], 0, VK_INDEX_TYPE_UINT32);

				unsigned int indicesContainerSize = assets_.aIndices_[meshID].size();
				vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indicesContainerSize), 1, 0, 0, 0);
			}

			if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
				throw std::runtime_error("failed to record command buffer!");
			}
//			vkCmdEndRenderPass(commandBuffer);
		}
	}

		void CVulkanRenderer::pointLightRecordCommandBuffer(std::vector<VkCommandBuffer>& commandBuffers, [[maybe_unused]] uint32_t currentFrame) {

// 		if ( entityManager->isEntitiesCollectionChanged && componentManager->isComponentsCollectionChanged ) {

// 			core::vector<unsigned int> linkedEntities;
// 			for ( unsigned int i = 0; i < linkedEntitiesTemp.GetSize(); ++i ) {
// 				unsigned int entity = linkedEntitiesTemp[i];
// 				for ( unsigned int j = 0; j < pointLightEntities.GetSize(); ++j ) {
// 					if ( entity == pointLightEntities[j] ) {
// 						break;
// 					} else if ( entity != pointLightEntities[j] && j == pointLightEntities.GetSize() - 1 ) {
// 						linkedEntities.Push(entity);
// 					}
// 				}
// 			}
// //			std::cout << "number of frame_.actors: " << linkedEntities.GetSize() << std::endl;
// 			entitiesCollectionLinked__Trn_Mat_Mes_Act.clear();
// 			for ( unsigned int i = 0; i < linkedEntities.GetSize(); ++i )
// 				entitiesCollectionLinked__Trn_Mat_Mes_Act.Push(linkedEntities[i]);

// 			entitiesCollectionLinked__Trn_PoL_Mes_Act.clear();
// 			for ( unsigned int i = 0; i < pointLightEntities.GetSize(); ++i )
// 				entitiesCollectionLinked__Trn_PoL_Mes_Act.Push(pointLightEntities[i]);

// 			entityManager->isEntitiesCollectionChanged = false;
// 			componentManager->isComponentsCollectionChanged = false;
// 		}

		// VkDebugUtilsLabelEXT label;
		// label.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
		// label.color[0] = 0.1;
		// label.color[0] = 0.7;
		// label.color[0] = 0.2;
		// label.color[0] = 1.0;
		// label.pLabelName = "pointLightShadowMap";
		// label.pNext = NULL;

		// vkDebugUtils::CreateBeginDebugUtilsLabelEXT(instance, commandBuffers[0], &label);
		for ( uint32_t pointLightCounter = 0; pointLightCounter < frame_.pointLights.GetSize(); ++pointLightCounter ) {
			uint32_t maxCubeMapLayers = 6;
			for ( uint32_t cubeMapLayerCounter = 0; cubeMapLayerCounter < maxCubeMapLayers; ++cubeMapLayerCounter ) {                      ///< 6 is a number of cube map layers.
				VkCommandBufferInheritanceInfo inheritanceInfo{};
				inheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
				inheritanceInfo.renderPass = resources_.renderPasses[SpecificPipeline::POINT_LIGHT_PIPELINE];
				inheritanceInfo.framebuffer = pointLightShadowMapFrameBuffers[pointLightCounter][cubeMapLayerCounter];
				inheritanceInfo.subpass = 0;

				VkCommandBufferBeginInfo beginInfo{};
				beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
				beginInfo.flags = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT |
					VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
				beginInfo.pInheritanceInfo = &inheritanceInfo;

				VkCommandBuffer commandBuffer = commandBuffers[currentFrame * pointLightNumber * maxCubeMapLayers +
					pointLightCounter * maxCubeMapLayers + cubeMapLayerCounter];
				if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
					throw std::runtime_error("failed to begin recording command buffer!");
				}

				// VkClearValue pointLightShadowMapClearValues[2];
				// pointLightShadowMapClearValues[0].depthStencil.depth = 1.0f;
				// pointLightShadowMapClearValues[0].depthStencil.stencil = 0;
				// pointLightShadowMapClearValues[1].color = {{0.5f, 0.5f, 0.5f, 1.0f}};

				// VkRenderPassBeginInfo pointLightShadowMapRenderPassInfo{};
				// pointLightShadowMapRenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
				// pointLightShadowMapRenderPassInfo.pNext = NULL;
				// pointLightShadowMapRenderPassInfo.renderPass = resources_.renderPasses[SpecificPipeline::POINT_LIGHT_PIPELINE];
				// pointLightShadowMapRenderPassInfo.framebuffer = pointLightShadowMapFrameBuffers[pointLightCounter][cubeMapLayerCounter];
				// pointLightShadowMapRenderPassInfo.renderArea.offset.x = 0;
				// pointLightShadowMapRenderPassInfo.renderArea.offset.y = 0;
				// pointLightShadowMapRenderPassInfo.renderArea.extent.width = SHADOW_MAP_SIZE;
				// pointLightShadowMapRenderPassInfo.renderArea.extent.height = SHADOW_MAP_SIZE;
				// pointLightShadowMapRenderPassInfo.clearValueCount = 2;
				// pointLightShadowMapRenderPassInfo.pClearValues = pointLightShadowMapClearValues;

				// vkCmdBeginRenderPass(commandBuffer, &pointLightShadowMapRenderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

				VkViewport pointLightShadowMapViewPort;
				pointLightShadowMapViewPort.height = SHADOW_MAP_SIZE;
				pointLightShadowMapViewPort.width = SHADOW_MAP_SIZE;
				pointLightShadowMapViewPort.minDepth = 0.0f;
				pointLightShadowMapViewPort.maxDepth = 1.0f;
				pointLightShadowMapViewPort.x = 0;
				pointLightShadowMapViewPort.y = 0;
				vkCmdSetViewport(commandBuffer, 0, 1, &pointLightShadowMapViewPort);

				VkRect2D pointLightShadowMapScissor;
				pointLightShadowMapScissor.extent.width = SHADOW_MAP_SIZE;
				pointLightShadowMapScissor.extent.height = SHADOW_MAP_SIZE;
				pointLightShadowMapScissor.offset.x = 0;
				pointLightShadowMapScissor.offset.y = 0;
				vkCmdSetScissor(commandBuffer, 0, 1, &pointLightShadowMapScissor);

				vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, resources_.pipelineConfigs[SpecificPipeline::POINT_LIGHT_PIPELINE].pipeline);

//				unsigned int pointLightEntity = entitiesCollectionLinked__Trn_PoL_Mes_Act[pointLightCounter];

				uint32_t actorsNumber = frame_.actors.GetSize();
				for ( unsigned int actorCounter = 0; actorCounter < actorsNumber; ++actorCounter ) {
//					unsigned int meshOwnerEntity = entitiesCollectionLinked__Trn_Mat_Mes_Act[actorCounter];
					RenderActor actor = frame_.actors[actorCounter];
					unsigned int meshID = actor.meshID;

					unsigned int uboIndex =
                        (resources_.descriptorSetsConfig[SHADOW_MAP_POINT_LIGHT].hostDescriptorNumber / MAX_FRAMES_IN_FLIGHT) * currentFrame +                           ///< Choose frame (first 168 or second 168)
						actorsNumber * maxCubeMapLayers * pointLightCounter +                      ///< Choose point light (i)
						maxCubeMapLayers * actorCounter + cubeMapLayerCounter;                     ///< Choose actor (m) and layer (j)

					updatePointLightShadowMapMatrixUBO(uboIndex, pointLightCounter, cubeMapLayerCounter, actorCounter);
					const unsigned int linkedDescriptorSetID = resources_.pipelineConfigs[SpecificPipeline::POINT_LIGHT_PIPELINE].linkedDescriptorSetIDs[0];
					const DescriptorSet& currentDescriptorSet = resources_.descriptorSetsConfig[linkedDescriptorSetID];
					vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, resources_.pipelineConfigs[SpecificPipeline::POINT_LIGHT_PIPELINE].pipelineLayout, 0, 1,
											&(*(resources_.descriptorSetsChunks.GetVectorContainer() + currentDescriptorSet.descriptorSetOffset + uboIndex)), 0, nullptr);

					VkBuffer vertexBuffers[] = {vertexBufferContainer[meshID]};
					VkDeviceSize offsets[] = {0};
					vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

					vkCmdBindIndexBuffer(commandBuffer, indexBufferContainer[meshID], 0, VK_INDEX_TYPE_UINT32);

					unsigned int indicesContainerSize = assets_.aIndices_[meshID].size();
					vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indicesContainerSize), 1, 0, 0, 0);
				}

				if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
					throw std::runtime_error("failed to record command buffer!");
				}
//				vkCmdEndRenderPass(commandBuffer);
			}
		}
	}

}
