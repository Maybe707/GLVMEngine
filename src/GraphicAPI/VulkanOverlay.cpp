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
	void CVulkanRenderer::updateHudUBO(uint32_t offset, bool isHudExists, float highestY, uint32_t healthCounter) {
		HUD_UBO hudUBO{};

		hudUBO.view = frame_.viewMatrix;
		hudUBO.proj = frame_.projectionMatrix;

		hudUBO.isHudExists = isHudExists;
		hudUBO.currentHP   = frame_.healthBars[healthCounter].currentHealth;
		hudUBO.maxHP       = frame_.healthBars[healthCounter].maxHealth;
		hudUBO.entityPosition = frame_.healthBars[healthCounter].position;
		hudUBO.highestY    = highestY;

		void* hudMatrixData;
		unsigned int hudUboDescriptorBindingIndex = resources_.descriptorSetsConfig[DescriptorSetDataLink::HUD].descriptorsBindingsIDs[0];
        vkMapMemory(device, resources_.GPUDescriptors[resources_.descriptorBindingsConfig[hudUboDescriptorBindingIndex].globalDescriptorOffset].GPUBuffer->deviceMemory, sizeof(HUD_UBO) * offset,
					sizeof(HUD_UBO), 0, &hudMatrixData);
        memcpy(hudMatrixData, &hudUBO, sizeof(HUD_UBO));
        vkUnmapMemory(device, resources_.GPUDescriptors[resources_.descriptorBindingsConfig[hudUboDescriptorBindingIndex].globalDescriptorOffset].GPUBuffer->deviceMemory);
	}

	void CVulkanRenderer::updateHudScreenUBO(uint32_t offset, uint32_t crosshair) {
		HUD_SCREEN_UBO hudUBO{};
		hudUBO.model = frame_.crosshairs[crosshair].model;

		void* hudMatrixData;
		unsigned int hudScreenUboDescriptorBindingIndex = resources_.descriptorSetsConfig[DescriptorSetDataLink::HUD_SCREEN].descriptorsBindingsIDs[0];
        vkMapMemory(device, resources_.GPUDescriptors[resources_.descriptorBindingsConfig[hudScreenUboDescriptorBindingIndex].globalDescriptorOffset].GPUBuffer->deviceMemory, sizeof(HUD_SCREEN_UBO) * offset,
					sizeof(HUD_SCREEN_UBO), 0, &hudMatrixData);
        memcpy(hudMatrixData, &hudUBO, sizeof(HUD_SCREEN_UBO));
        vkUnmapMemory(device, resources_.GPUDescriptors[resources_.descriptorBindingsConfig[hudScreenUboDescriptorBindingIndex].globalDescriptorOffset].GPUBuffer->deviceMemory);
	}

	void CVulkanRenderer::updateSdfUBO(uint32_t offset, uint32_t crosshair) {
		SDF_UBO hudUBO{};
		hudUBO.model = frame_.crosshairs[crosshair].model;

		float currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - startTime).count() * 0.001;
		hudUBO.iTime = currentTime;

		void* hudMatrixData;
		unsigned int hudScreenUboDescriptorBindingIndex = resources_.descriptorSetsConfig[DescriptorSetDataLink::SDF_DATA].descriptorsBindingsIDs[0];
        vkMapMemory(device, resources_.GPUDescriptors[resources_.descriptorBindingsConfig[hudScreenUboDescriptorBindingIndex].globalDescriptorOffset].GPUBuffer->deviceMemory, sizeof(SDF_UBO) * offset,
					sizeof(SDF_UBO), 0, &hudMatrixData);
        memcpy(hudMatrixData, &hudUBO, sizeof(SDF_UBO));
        vkUnmapMemory(device, resources_.GPUDescriptors[resources_.descriptorBindingsConfig[hudScreenUboDescriptorBindingIndex].globalDescriptorOffset].GPUBuffer->deviceMemory);
	}

	void CVulkanRenderer::updateCollisionsDebugUBO(uint32_t offset, mat4 model, DescriptorSetDataLink descriptorSetLink) {
		COLLISIONS_DEBUG_UBO collisionsDebugUBO{};
		collisionsDebugUBO.model      = model;
//		collisionsDebugUBO.model      = frame_.collisionsWireframes[crosshair].model;
		collisionsDebugUBO.view       = frame_.viewMatrix;
		collisionsDebugUBO.projection = frame_.projectionMatrix;

//		std::cout << "model: " << collisionsDebugUBO.model << std::endl;

		void* collisionsDebugData;
		unsigned int hudScreenUboDescriptorBindingIndex = resources_.descriptorSetsConfig[descriptorSetLink].descriptorsBindingsIDs[0];
        vkMapMemory(device, resources_.GPUDescriptors[resources_.descriptorBindingsConfig[hudScreenUboDescriptorBindingIndex].globalDescriptorOffset].GPUBuffer->deviceMemory, sizeof(COLLISIONS_DEBUG_UBO) * offset,
					sizeof(COLLISIONS_DEBUG_UBO), 0, &collisionsDebugData);
        memcpy(collisionsDebugData, &collisionsDebugUBO, sizeof(COLLISIONS_DEBUG_UBO));
        vkUnmapMemory(device, resources_.GPUDescriptors[resources_.descriptorBindingsConfig[hudScreenUboDescriptorBindingIndex].globalDescriptorOffset].GPUBuffer->deviceMemory);
	}

	void CVulkanRenderer::updateUBO_UI( const unsigned int currentInventoryRow, const unsigned int currentInventoryColumn, const unsigned int inventory, uint32_t offset ) {
		UI_UBO hudUBO{};

		const unsigned int colSize = frame_.inventories[inventory].col;
		hudUBO.model = frame_.inventories[inventory].slotData[colSize * currentInventoryRow + currentInventoryColumn].model;
		hudUBO.color = frame_.inventories[inventory].slotData[colSize * currentInventoryRow + currentInventoryColumn].color;

		void* hudMatrixData;
		unsigned int uiUboDescriptorBindingIndex = resources_.descriptorSetsConfig[DescriptorSetDataLink::UI].descriptorsBindingsIDs[0];
        vkMapMemory(device, resources_.GPUDescriptors[resources_.descriptorBindingsConfig[uiUboDescriptorBindingIndex].globalDescriptorOffset].GPUBuffer->deviceMemory, sizeof(UI_UBO) * offset,
					sizeof(UI_UBO), 0, &hudMatrixData);
        memcpy(hudMatrixData, &hudUBO, sizeof(UI_UBO));
        vkUnmapMemory(device, resources_.GPUDescriptors[resources_.descriptorBindingsConfig[uiUboDescriptorBindingIndex].globalDescriptorOffset].GPUBuffer->deviceMemory);
	}

	void CVulkanRenderer::updateUBO_IconsUI( uint32_t offset, uint32_t item ) {
		UI_UBO hudUBO{};
		hudUBO.model = frame_.items[item].model;

		void* hudMatrixData;
		unsigned int uiIconsUboDescriptorBindingIndex = resources_.descriptorSetsConfig[DescriptorSetDataLink::UI_ICONS].descriptorsBindingsIDs[0];
        vkMapMemory(device, resources_.GPUDescriptors[resources_.descriptorBindingsConfig[uiIconsUboDescriptorBindingIndex].globalDescriptorOffset].GPUBuffer->deviceMemory, sizeof(UI_UBO) * offset,
					sizeof(UI_UBO), 0, &hudMatrixData);
        memcpy(hudMatrixData, &hudUBO, sizeof(UI_UBO));
        vkUnmapMemory(device, resources_.GPUDescriptors[resources_.descriptorBindingsConfig[uiIconsUboDescriptorBindingIndex].globalDescriptorOffset].GPUBuffer->deviceMemory);
	}

    void CVulkanRenderer::hudRecordCommandBuffer(VkCommandBuffer& commandBuffer, uint32_t imageIndex) {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        // if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        //     throw std::runtime_error("failed to begin recording command buffer!");
        // }


//		CreateEndDebugUtilsLabelEXT(instance, commandBuffer);

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = resources_.renderPasses[SpecificPipeline::HUD_PIPELINE];
        renderPassInfo.framebuffer = swapChainFramebuffers[imageIndex];
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent.height = swapChainExtent.height;
		renderPassInfo.renderArea.extent.width = swapChainExtent.width;

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {{0.5f, 0.2f, 0.2f, 1.0f}};
        clearValues[1].depthStencil = {1.0f, 0};

        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, resources_.pipelineConfigs[SpecificPipeline::HUD_PIPELINE].pipeline);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float) swapChainExtent.width;
        viewport.height = (float) swapChainExtent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = swapChainExtent;
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

		for ( unsigned int i = 0; i < frame_.healthBars.GetSize(); ++i ) {
			unsigned int uiVertexId = frame_.healthBars[i].meshID;
			unsigned int uboIndex = currentFrame * hudUboDescriptorNumber + i;
			updateHudUBO(uboIndex, true, assets_.highest_gltf_Y[uiVertexId], i);
			const unsigned int linkedDescriptorSetID = resources_.pipelineConfigs[SpecificPipeline::HUD_PIPELINE].linkedDescriptorSetIDs[0];
			const DescriptorSet& currentDescriptorSet = resources_.descriptorSetsConfig[linkedDescriptorSetID];
			vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, resources_.pipelineConfigs[SpecificPipeline::HUD_PIPELINE].pipelineLayout,
									0, 1, &(*(resources_.descriptorSetsChunks.GetVectorContainer() + currentDescriptorSet.descriptorSetOffset + uboIndex)), 0, nullptr);

			VkBuffer vertexBuffers[] = {vertexBufferContainer[uiVertexId]};
			VkDeviceSize offsets[] = {0};
			vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

			vkCmdBindIndexBuffer(commandBuffer, indexBufferContainer[uiVertexId], 0, VK_INDEX_TYPE_UINT32);

			unsigned int indicesContainerSize = assets_.aIndices_[uiVertexId].size();

			vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indicesContainerSize), 1, 0, 0, 0);
		}

        vkCmdEndRenderPass(commandBuffer);

        // if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        //     throw std::runtime_error("failed to record command buffer!");
        // }
    }

    void CVulkanRenderer::uiRecordCommandBuffer(VkCommandBuffer& commandBuffer, uint32_t imageIndex) {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        // if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        //     throw std::runtime_error("failed to begin recording command buffer!");
        // }

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = resources_.renderPasses[SpecificPipeline::UI_PIPELINE];
        renderPassInfo.framebuffer = swapChainFramebuffers[imageIndex];
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent.height = swapChainExtent.height;
		renderPassInfo.renderArea.extent.width = swapChainExtent.width;

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {{0.5f, 0.2f, 0.2f, 1.0f}};
        clearValues[1].depthStencil = {1.0f, 0};

        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, resources_.pipelineConfigs[SpecificPipeline::UI_PIPELINE].pipeline);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float) swapChainExtent.width;
        viewport.height = (float) swapChainExtent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = swapChainExtent;
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

		for ( unsigned int i = 0; i < frame_.inventories.GetSize(); ++i ) {
			RenderInventory inventory = frame_.inventories[i];
			unsigned int inventoryTextureID   = inventory.inventoryTextureID;
			unsigned int uiVertexId           = inventory.meshID;
			for ( unsigned int j = 0; j < inventory.row; ++j ) {
				for ( unsigned int m = 0; m < inventory.col; ++m ) {
					unsigned int uboIndex = currentFrame * uiUboDescriptorsNumber + j * inventory.col + m;
					updateUBO_UI(j, m, i, uboIndex);
					const unsigned int linkedDescriptorSetID = resources_.pipelineConfigs[SpecificPipeline::UI_PIPELINE].linkedDescriptorSetIDs[0];
					const DescriptorSet& currentDescriptorSet = resources_.descriptorSetsConfig[linkedDescriptorSetID];
					vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, resources_.pipelineConfigs[SpecificPipeline::UI_PIPELINE].pipelineLayout,
											0, 1, &(*(resources_.descriptorSetsChunks.GetVectorContainer() + currentDescriptorSet.descriptorSetOffset + uboIndex)), 0, nullptr);

					const unsigned int linkedDescriptorSetID1 = resources_.pipelineConfigs[SpecificPipeline::UI_PIPELINE].linkedDescriptorSetIDs[1];
					const DescriptorSet& currentDescriptorSet1 = resources_.descriptorSetsConfig[linkedDescriptorSetID1];
					vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, resources_.pipelineConfigs[SpecificPipeline::UI_PIPELINE].pipelineLayout,
											1, 1, &(*(resources_.descriptorSetsChunks.GetVectorContainer() + currentDescriptorSet1.descriptorSetOffset + MAX_FRAMES_IN_FLIGHT * inventoryTextureID + currentFrame)), 0, nullptr);

					VkBuffer vertexBuffers[] = {vertexBufferContainer[uiVertexId]};
					VkDeviceSize offsets[] = {0};
					vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

					vkCmdBindIndexBuffer(commandBuffer, indexBufferContainer[uiVertexId], 0, VK_INDEX_TYPE_UINT32);

					unsigned int indicesContainerSize = assets_.aIndices_[uiVertexId].size();

					vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indicesContainerSize), 1, 0, 0, 0);
				}
			}
		}

        vkCmdEndRenderPass(commandBuffer);

        // if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        //     throw std::runtime_error("failed to record command buffer!");
        // }
    }

    void CVulkanRenderer::uiIconsRecordCommandBuffer(VkCommandBuffer& commandBuffer, uint32_t imageIndex) {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        // if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        //     throw std::runtime_error("failed to begin recording command buffer!");
        // }

//		CreateEndDebugUtilsLabelEXT(instance, commandBuffer);

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = resources_.renderPasses[SpecificPipeline::UI_ICONS_PIPELINE];
        renderPassInfo.framebuffer = swapChainFramebuffers[imageIndex];
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent.height = swapChainExtent.height;
		renderPassInfo.renderArea.extent.width = swapChainExtent.width;

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {{0.5f, 0.2f, 0.2f, 1.0f}};
        clearValues[1].depthStencil = {1.0f, 0};

        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, resources_.pipelineConfigs[SpecificPipeline::UI_ICONS_PIPELINE].pipeline);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float) swapChainExtent.width;
        viewport.height = (float) swapChainExtent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = swapChainExtent;
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

		for ( unsigned int i = 0; i < frame_.items.GetSize(); ++i ) {
			RenderItem item = frame_.items[i];
			unsigned int uiVertexId = item.meshID;
			unsigned int diffuseTexureID = item.diffuseTexureID;
			unsigned int uboIndex = currentFrame * (resources_.descriptorSetsConfig[UI_ICONS].hostDescriptorNumber / MAX_FRAMES_IN_FLIGHT) + i;

			updateUBO_IconsUI(uboIndex, i);
			const unsigned int linkedDescriptorSetID = resources_.pipelineConfigs[SpecificPipeline::UI_ICONS_PIPELINE].linkedDescriptorSetIDs[0];
			const DescriptorSet& currentDescriptorSet = resources_.descriptorSetsConfig[linkedDescriptorSetID];
			vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, resources_.pipelineConfigs[SpecificPipeline::UI_ICONS_PIPELINE].pipelineLayout,
									0, 1, &(*(resources_.descriptorSetsChunks.GetVectorContainer() + currentDescriptorSet.descriptorSetOffset + uboIndex)), 0, nullptr);

			const unsigned int linkedDescriptorSetID1 = resources_.pipelineConfigs[SpecificPipeline::UI_ICONS_PIPELINE].linkedDescriptorSetIDs[1];
			const DescriptorSet& currentDescriptorSet1 = resources_.descriptorSetsConfig[linkedDescriptorSetID1];
			vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, resources_.pipelineConfigs[SpecificPipeline::UI_ICONS_PIPELINE].pipelineLayout,
									1, 1, &(*(resources_.descriptorSetsChunks.GetVectorContainer() + currentDescriptorSet1.descriptorSetOffset + MAX_FRAMES_IN_FLIGHT * diffuseTexureID + currentFrame)), 0, nullptr);

			VkBuffer vertexBuffers[] = {vertexBufferContainer[uiVertexId]};
			VkDeviceSize offsets[] = {0};
			vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

			vkCmdBindIndexBuffer(commandBuffer, indexBufferContainer[uiVertexId], 0, VK_INDEX_TYPE_UINT32);

			unsigned int indicesContainerSize = assets_.aIndices_[uiVertexId].size();

			vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indicesContainerSize), 1, 0, 0, 0);
		}

        vkCmdEndRenderPass(commandBuffer);

        // if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        //     throw std::runtime_error("failed to record command buffer!");
        // }
    }

    void CVulkanRenderer::hudScreenRecordCommandBuffer(VkCommandBuffer& commandBuffer, uint32_t imageIndex) {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        // if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        //     throw std::runtime_error("failed to begin recording command buffer!");
        // }

//		CreateEndDebugUtilsLabelEXT(instance, commandBuffer);

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = resources_.renderPasses[SpecificPipeline::HUD_SCREEN_PIPELINE];
        renderPassInfo.framebuffer = swapChainFramebuffers[imageIndex];
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent.height = swapChainExtent.height;
		renderPassInfo.renderArea.extent.width = swapChainExtent.width;

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {{0.5f, 0.2f, 0.2f, 1.0f}};
        clearValues[1].depthStencil = {1.0f, 0};

        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, resources_.pipelineConfigs[SpecificPipeline::HUD_SCREEN_PIPELINE].pipeline);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float) swapChainExtent.width;
        viewport.height = (float) swapChainExtent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = swapChainExtent;
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

		for ( unsigned int i = 0; i < frame_.crosshairs.GetSize(); ++i ) {
			RenderCrosshair crosshair = frame_.crosshairs[i];
			unsigned int uiVertexId = crosshair.meshID;

			unsigned int uboIndex = currentFrame * hudScreenUboDescriptorNumber + i;
			updateHudScreenUBO(uboIndex, i);
			const unsigned int linkedDescriptorSetID = resources_.pipelineConfigs[SpecificPipeline::HUD_SCREEN_PIPELINE].linkedDescriptorSetIDs[0];
			const DescriptorSet& currentDescriptorSet = resources_.descriptorSetsConfig[linkedDescriptorSetID];
			vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, resources_.pipelineConfigs[SpecificPipeline::HUD_SCREEN_PIPELINE].pipelineLayout,
									0, 1, &(*(resources_.descriptorSetsChunks.GetVectorContainer() + currentDescriptorSet.descriptorSetOffset + uboIndex)), 0, nullptr);

			VkBuffer vertexBuffers[] = {vertexBufferContainer[uiVertexId]};
			VkDeviceSize offsets[] = {0};
			vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

			vkCmdBindIndexBuffer(commandBuffer, indexBufferContainer[uiVertexId], 0, VK_INDEX_TYPE_UINT32);

			unsigned int indicesContainerSize = assets_.aIndices_[uiVertexId].size();

			vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indicesContainerSize), 1, 0, 0, 0);
		}

        vkCmdEndRenderPass(commandBuffer);

        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to record command buffer!");
        }
    }

    void CVulkanRenderer::sdfRecordCommandBuffer(VkCommandBuffer& commandBuffer, uint32_t imageIndex) {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        // if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        //     throw std::runtime_error("failed to begin recording command buffer!");
        // }

//		CreateEndDebugUtilsLabelEXT(instance, commandBuffer);

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = resources_.renderPasses[SpecificPipeline::SDF_PIPELINE];
        renderPassInfo.framebuffer = swapChainFramebuffers[imageIndex];
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent.height = swapChainExtent.height;
		renderPassInfo.renderArea.extent.width = swapChainExtent.width;

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {{0.5f, 0.2f, 0.2f, 1.0f}};
        clearValues[1].depthStencil = {1.0f, 0};

        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, resources_.pipelineConfigs[SpecificPipeline::SDF_PIPELINE].pipeline);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float) swapChainExtent.width;
        viewport.height = (float) swapChainExtent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = swapChainExtent;
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

		for ( unsigned int i = 0; i < frame_.crosshairs.GetSize(); ++i ) {
			RenderCrosshair crosshair = frame_.crosshairs[i];
			unsigned int uiVertexId = crosshair.meshID;

			unsigned int uboIndex = currentFrame * hudScreenUboDescriptorNumber + i;
			updateSdfUBO(uboIndex, i);
			const unsigned int linkedDescriptorSetID = resources_.pipelineConfigs[SpecificPipeline::SDF_PIPELINE].linkedDescriptorSetIDs[0];
			const DescriptorSet& currentDescriptorSet = resources_.descriptorSetsConfig[linkedDescriptorSetID];
			vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, resources_.pipelineConfigs[SpecificPipeline::SDF_PIPELINE].pipelineLayout,
									0, 1, &(*(resources_.descriptorSetsChunks.GetVectorContainer() + currentDescriptorSet.descriptorSetOffset + uboIndex)), 0, nullptr);

			VkBuffer vertexBuffers[] = {vertexBufferContainer[uiVertexId]};
			VkDeviceSize offsets[] = {0};
			vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

			vkCmdBindIndexBuffer(commandBuffer, indexBufferContainer[uiVertexId], 0, VK_INDEX_TYPE_UINT32);

//			unsigned int indicesContainerSize = assets_.aIndices_[uiVertexId].size();

//			vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indicesContainerSize), 1, 0, 0, 0);
			vkCmdDrawIndexed(commandBuffer, 3, 1, 0, 0, 0);
		}

        vkCmdEndRenderPass(commandBuffer);

        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to record command buffer!");
        }
    }

    void CVulkanRenderer::collisionsDebugRecordCommandBuffer(VkCommandBuffer& commandBuffer, uint32_t imageIndex) {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        // if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        //     throw std::runtime_error("failed to begin recording command buffer!");
        // }

//		CreateEndDebugUtilsLabelEXT(instance, commandBuffer);

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = resources_.renderPasses[SpecificPipeline::COLLISIONS_DEBUG_PIPELINE];
        renderPassInfo.framebuffer = swapChainFramebuffers[imageIndex];
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent.height = swapChainExtent.height;
		renderPassInfo.renderArea.extent.width = swapChainExtent.width;

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {{0.5f, 0.2f, 0.2f, 1.0f}};
        clearValues[1].depthStencil = {1.0f, 0};

        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, resources_.pipelineConfigs[SpecificPipeline::COLLISIONS_DEBUG_PIPELINE].pipeline);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float) swapChainExtent.width;
        viewport.height = (float) swapChainExtent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = swapChainExtent;
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
//		std::cout << "FRAME" << std::endl;

		if( frame_.collisionsWireframes.GetSize() != collisionsWireframesVKBuffers.GetSize() ) {
			vkDeviceWaitIdle(device);
			for( size_t i = 0; i < collisionsWireframesVKBuffers.GetSize(); ++i ) {
				vkDestroyBuffer(device, collisionsWireframesVKBuffers[i], nullptr);
				vkFreeMemory(device, collisionsWireframesVKDeviceMemory[i], nullptr);
				vkDestroyBuffer(device, collisionsWireframesIndicesVKBuffers[i], nullptr);
				vkFreeMemory(device, collisionsWireframesIndicesVKDeviceMemory[i], nullptr);
			}
			vkDeviceWaitIdle(device);
			collisionsWireframesVKBuffers.clear();
			collisionsWireframesVKDeviceMemory.clear();
			collisionsWireframesIndicesVKBuffers.clear();
			collisionsWireframesIndicesVKDeviceMemory.clear();
			collisionsWireframeIndices.clear();

			constexpr int boxIndicesForIndexBuffer[36] =
				{ 0, 1, 2, 3, 0, 2,
				  4, 0, 3, 7, 4, 3,
				  4, 5, 1, 0, 4, 1,
				  1, 5, 6, 2, 1, 6,
				  5, 4, 7, 6, 5, 7,
				  3, 2, 6, 7, 3, 6 };

			for ( unsigned int i = 0; i < 36; ++i )
				collisionsWireframeIndices.push_back(boxIndicesForIndexBuffer[i]);

			for ( unsigned int i = 0; i < frame_.collisionsWireframes.GetSize(); ++i ) {
				RenderCollisionWireframe collisionWireframe = frame_.collisionsWireframes[i];

				core::vector<core::Vertex> vertices;
				unsigned int cube_vertices = 8;

				const GLVM::core::MeshAxisMaxAbsoluteValues meshAxisMaxAbsoluteValues = collisionWireframe.meshAxisMaxAbsoluteValues;
//			const float scale   = collisionWireframe.scale;
				const float half_x = meshAxisMaxAbsoluteValues.origin_offset_x + meshAxisMaxAbsoluteValues.absolute_x;
				const float half_y = meshAxisMaxAbsoluteValues.origin_offset_y + meshAxisMaxAbsoluteValues.absolute_y;
				const float half_z = meshAxisMaxAbsoluteValues.origin_offset_z + meshAxisMaxAbsoluteValues.absolute_z;

				const float bottom_half_x = meshAxisMaxAbsoluteValues.origin_offset_x - meshAxisMaxAbsoluteValues.absolute_x;
				const float bottom_half_y = meshAxisMaxAbsoluteValues.origin_offset_y - meshAxisMaxAbsoluteValues.absolute_y;
				const float bottom_half_z = meshAxisMaxAbsoluteValues.origin_offset_z - meshAxisMaxAbsoluteValues.absolute_z;

				for ( unsigned int i = 0; i < cube_vertices; ++i ) {
					SVertex vertex;
					switch( i ) {
					case 0:
						vertex[0] = half_x;
						vertex[1] = half_y;
						vertex[2] = half_z;
						break;
					case 1:
						vertex[0] = (float)bottom_half_x;
						vertex[1] = half_y;
						vertex[2] = half_z;
						break;
					case 2:
						vertex[0] = (float)bottom_half_x;
						vertex[1] = (float)bottom_half_y;
						vertex[2] = half_z;
						break;
					case 3:
						vertex[0] = half_x;
						vertex[1] = (float)bottom_half_y;
						vertex[2] = half_z;
						break;
					case 4:
						vertex[0] = half_x;
						vertex[1] = half_y;
						vertex[2] = (float)bottom_half_z;
						break;
					case 5:
						vertex[0] = (float)bottom_half_x;
						vertex[1] = half_y;
						vertex[2] = (float)bottom_half_z;
						break;
					case 6:
						vertex[0] = (float)bottom_half_x;
						vertex[1] = (float)bottom_half_y;
						vertex[2] = (float)bottom_half_z;
						break;
					case 7:
						vertex[0] = half_x;
						vertex[1] = (float)bottom_half_y;
						vertex[2] = (float)bottom_half_z;
						break;
					}

					SVertex normal;
					normal[0] = 0;
					normal[1] = 1;
					normal[2] = 0;
					SVertex texture;
					texture[0] = 0;
					texture[1] = 1;

					vertices.Push({{vertex[0], vertex[1], vertex[2]},
								   {normal[0], normal[1], normal[2]},
								   {texture[0], texture[1]},
								   { -1, -1, -1, -1 },
								   { 1, 1, 1, 1 }});
				}

				collisionsWireframesVKBuffers.Push({});;
				collisionsWireframesVKDeviceMemory.Push({});
				createVertexBuffer(collisionsWireframesVKBuffers[i], collisionsWireframesVKDeviceMemory[i], vertices);

				collisionsWireframesIndicesVKBuffers.Push({});
				collisionsWireframesIndicesVKDeviceMemory.Push({});
				createIndexBuffer(collisionsWireframesIndicesVKBuffers[i], collisionsWireframesIndicesVKDeviceMemory[i], collisionsWireframeIndices);
			}
			isCollisionsWireframeBuffersInitialized = true;
		}

		for ( unsigned int i = 0; i < frame_.collisionsWireframes.GetSize(); ++i ) {
			[[maybe_unused]] RenderCollisionWireframe collisionWireframe = frame_.collisionsWireframes[i];
//			RenderCrosshair crosshair = frame_.crosshairs[i];
//			unsigned int uiVertexId = crosshair.meshID;

			unsigned int uboIndex = currentFrame * hudScreenUboDescriptorNumber + i;
			updateCollisionsDebugUBO(uboIndex, collisionWireframe.model, DescriptorSetDataLink::COLLISIONS_DEBUG_DATA);
			const unsigned int linkedDescriptorSetID = resources_.pipelineConfigs[SpecificPipeline::COLLISIONS_DEBUG_PIPELINE].linkedDescriptorSetIDs[0];
			const DescriptorSet& currentDescriptorSet = resources_.descriptorSetsConfig[linkedDescriptorSetID];
			vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, resources_.pipelineConfigs[SpecificPipeline::COLLISIONS_DEBUG_PIPELINE].pipelineLayout,
									0, 1, &(*(resources_.descriptorSetsChunks.GetVectorContainer() + currentDescriptorSet.descriptorSetOffset + uboIndex)), 0, nullptr);


			VkBuffer vertexBuffers[] = {collisionsWireframesVKBuffers[i]};
			VkDeviceSize offsets[] = {0};
			vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

			vkCmdBindIndexBuffer(commandBuffer, collisionsWireframesIndicesVKBuffers[i], 0, VK_INDEX_TYPE_UINT32);

			unsigned int indicesContainerSize = collisionsWireframeIndices.size();

			vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indicesContainerSize), 1, 0, 0, 0);
//			vkCmdDrawIndexed(commandBuffer, 3, 1, 0, 0, 0);
		}

        vkCmdEndRenderPass(commandBuffer);

        // if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        //     throw std::runtime_error("failed to record command buffer!");
        // }
    }

    void CVulkanRenderer::spacialGridDebugRecordCommandBuffer(VkCommandBuffer& commandBuffer, uint32_t imageIndex) {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        // if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        //     throw std::runtime_error("failed to begin recording command buffer!");
        // }

//		CreateEndDebugUtilsLabelEXT(instance, commandBuffer);

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = resources_.renderPasses[SpecificPipeline::SPACIAL_GRID_DEBUG_PIPELINE];
        renderPassInfo.framebuffer = swapChainFramebuffers[imageIndex];
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent.height = swapChainExtent.height;
		renderPassInfo.renderArea.extent.width = swapChainExtent.width;

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {{0.5f, 0.2f, 0.2f, 1.0f}};
        clearValues[1].depthStencil = {1.0f, 0};

        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, resources_.pipelineConfigs[SpecificPipeline::SPACIAL_GRID_DEBUG_PIPELINE].pipeline);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float) swapChainExtent.width;
        viewport.height = (float) swapChainExtent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = swapChainExtent;
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
//		std::cout << "FRAME" << std::endl;

		const float chunkSize = frame_.renderSpacialGrid.chunkSize;

		const u32 spacialGridDepth  = frame_.renderSpacialGrid.halfDepth;
		const u32 spacialGridHeight = frame_.renderSpacialGrid.halfHeight;
		const u32 spacialGridWidth  = frame_.renderSpacialGrid.halfWidth;

		if( !isSpacialGridWireframeBuffersInitialized ) {
			// vkDeviceWaitIdle(device);
			// for( size_t i = 0; i < spacialGridWireframesVKBuffers.GetSize(); ++i ) {
			// 	vkDestroyBuffer(device, spacialGridWireframesVKBuffers[i], nullptr);
			// 	vkFreeMemory(device, spacialGridWireframesVKDeviceMemory[i], nullptr);
			// }
			// vkDeviceWaitIdle(device);
			// spacialGridWireframesVKBuffers.clear();
			// spacialGridWireframesVKDeviceMemory.clear();

			for( u32 i2 = 0; i2 < spacialGridDepth; ++i2 ) {
				for( u32 i3 = 0; i3 < spacialGridHeight; ++i3 ) {
					for( u32 i4 = 0; i4 < spacialGridWidth; ++i4 ) {
						core::vector<core::Vertex> vertices;
						unsigned int cube_vertices = 8;

						const float chunkHalfSize = chunkSize * 0.5f;

						const float half_x = chunkHalfSize;
						const float half_y = chunkHalfSize;
						const float half_z = chunkHalfSize;

						for ( unsigned int i = 0; i < cube_vertices; ++i ) {
							SVertex vertex;
							switch( i ) {
							case 0:
								vertex[0] = half_x;
								vertex[1] = half_y;
								vertex[2] = half_z;
								break;
							case 1:
								vertex[0] = -(float)half_x;
								vertex[1] = half_y;
								vertex[2] = half_z;
								break;
							case 2:
								vertex[0] = -(float)half_x;
								vertex[1] = -(float)half_y;
								vertex[2] = half_z;
								break;
							case 3:
								vertex[0] = half_x;
								vertex[1] = -(float)half_y;
								vertex[2] = half_z;
								break;
							case 4:
								vertex[0] = half_x;
								vertex[1] = half_y;
								vertex[2] = -(float)half_z;
								break;
							case 5:
								vertex[0] = -(float)half_x;
								vertex[1] = half_y;
								vertex[2] = -(float)half_z;
								break;
							case 6:
								vertex[0] = -(float)half_x;
								vertex[1] = -(float)half_y;
								vertex[2] = -(float)half_z;
								break;
							case 7:
								vertex[0] = half_x;
								vertex[1] = -(float)half_y;
								vertex[2] = -(float)half_z;
								break;
							}

							SVertex normal;
							normal[0] = 0;
							normal[1] = 1;
							normal[2] = 0;
							SVertex texture;
							texture[0] = 0;
							texture[1] = 1;

							vertices.Push({{vertex[0], vertex[1], vertex[2]},
										   {normal[0], normal[1], normal[2]},
										   {texture[0], texture[1]},
										   { -1, -1, -1, -1 },
										   { 1, 1, 1, 1 }});
						}

						const u32 index = i2 * spacialGridHeight * spacialGridWidth + i3 * spacialGridWidth + i4;

						spacialGridWireframesVKBuffers.Push({});;
						spacialGridWireframesVKDeviceMemory.Push({});
						createVertexBuffer(spacialGridWireframesVKBuffers[index], spacialGridWireframesVKDeviceMemory[index], vertices);
					}
				}
			}
			isSpacialGridWireframeBuffersInitialized = true;
		}

		for( u32 i2 = 0; i2 < spacialGridDepth; ++i2 ) {
			for( u32 i3 = 0; i3 < spacialGridHeight; ++i3 ) {
				for( u32 i4 = 0; i4 < spacialGridWidth; ++i4 ) {
					const u32 index = i2 * spacialGridHeight * spacialGridWidth + i3 * spacialGridWidth + i4;

					mat4 scale(1.0f);
					mat4 translation(1.0f);

					const float halfSize = chunkSize * 0.5f;

					translation[3][0] = i4 * chunkSize + halfSize - 4.0 + 1.0;
					translation[3][1] = i3 * chunkSize + halfSize + 1.0;
					translation[3][2] = i2 * chunkSize + halfSize - 4.0 + 1.0;
					translation[3][3] = 1.0f;

					unsigned int uboIndex = currentFrame * spacialGridWireFrameUboNumber + index;
					updateCollisionsDebugUBO(uboIndex, scale * translation, DescriptorSetDataLink::SPACIAL_GRID_DEBUG_DATA);
					const unsigned int linkedDescriptorSetID = resources_.pipelineConfigs[SpecificPipeline::SPACIAL_GRID_DEBUG_PIPELINE].linkedDescriptorSetIDs[0];
					const DescriptorSet& currentDescriptorSet = resources_.descriptorSetsConfig[linkedDescriptorSetID];
					vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, resources_.pipelineConfigs[SpecificPipeline::SPACIAL_GRID_DEBUG_PIPELINE].pipelineLayout,
											0, 1, &(*(resources_.descriptorSetsChunks.GetVectorContainer() + currentDescriptorSet.descriptorSetOffset + uboIndex)), 0, nullptr);


					VkBuffer vertexBuffers[] = {spacialGridWireframesVKBuffers[index]};
					VkDeviceSize offsets[] = {0};
					vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

					vkCmdBindIndexBuffer(commandBuffer, collisionsWireframesIndicesVKBuffers[0], 0, VK_INDEX_TYPE_UINT32);

					unsigned int indicesContainerSize = collisionsWireframeIndices.size();

					vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indicesContainerSize), 1, 0, 0, 0);
//			vkCmdDrawIndexed(commandBuffer, 3, 1, 0, 0, 0);
				}
			}
		}

        vkCmdEndRenderPass(commandBuffer);

        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to record command buffer!");
        }
    }

    void CVulkanRenderer::fontRecordCommandBuffer(VkCommandBuffer& commandBuffer, uint32_t imageIndex) {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        // if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        //     throw std::runtime_error("failed to begin recording command buffer!");
        // }

//		CreateEndDebugUtilsLabelEXT(instance, commandBuffer);

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = resources_.renderPasses[SpecificPipeline::FONT_PIPELINE];
        renderPassInfo.framebuffer = swapChainFramebuffers[imageIndex];
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent.height = swapChainExtent.height;
		renderPassInfo.renderArea.extent.width = swapChainExtent.width;

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {{0.5f, 0.2f, 0.2f, 1.0f}};
        clearValues[1].depthStencil = {1.0f, 0};

        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, resources_.pipelineConfigs[SpecificPipeline::FONT_PIPELINE].pipeline);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float) swapChainExtent.width;
        viewport.height = (float) swapChainExtent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = swapChainExtent;
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);


		RenderPlayer player{};
		for( unsigned int playerCounter = 0; playerCounter < frame_.players.GetSize(); ++playerCounter ) {
			player = frame_.players[playerCounter];
		}
			unsigned int currentActorMemoryOffset = currentFrame * fontUboDescriptorNumber;
			for ( unsigned int i = 0; i < frame_.fonts.GetSize(); ++i ) {
				RenderFont font = frame_.fonts[i];
				vec3 playerTragetDirection = font.position - player.position;
				float dotProduct = Dot(playerTragetDirection, player.forward);
				if ( dotProduct <= 0 )
					continue;

				for ( unsigned int j = 0; j < font.font_string.GetSize(); ++j ) {
					unsigned int ascii_code = static_cast<unsigned int>(font.font_string[j]);
					VkBuffer vertexBuffers[] = { fontVertexBufferContainer[ascii_code] };
					VkDeviceSize offsets[] = {0};

					vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
					vkCmdBindIndexBuffer(commandBuffer, fontIndexBufferContainer[ascii_code], 0, VK_INDEX_TYPE_UINT32);

					unsigned int indicesContainerSize = symbol_g_indices.size();
					FONT_UBO fontUBO{};
					vec3 result;
					vec4 pos = vec4(font.position[0],
									font.position[1],
									font.position[2], 1.0f);

					vec4 clipSpacePosition =  pos * frame_.viewMatrix * frame_.projectionMatrix;
					vec3 ndcPosition = vec3(clipSpacePosition[0] / clipSpacePosition[3],
											clipSpacePosition[1] / clipSpacePosition[3],
											clipSpacePosition[2] / clipSpacePosition[3]);

					fontUBO.view = frame_.viewMatrix;
					fontUBO.proj = frame_.projectionMatrix;

					fontUBO.scale    = 0.3f;
					ndcPosition[0] += (float)j * 0.17f * fontUBO.scale;
					ndcPosition[1] -= font.lifeTime / 5.0f;
					fontUBO.position = ndcPosition;

					void* modelMatrixData;
					unsigned int fontUboDescriptorBindingIndex = resources_.descriptorSetsConfig[DescriptorSetDataLink::FONT_RENDER_UBO].descriptorsBindingsIDs[0];
					vkMapMemory(device, resources_.GPUDescriptors[resources_.descriptorBindingsConfig[fontUboDescriptorBindingIndex].globalDescriptorOffset].GPUBuffer->deviceMemory, sizeof(fontUBO) * (currentActorMemoryOffset + j),
								sizeof(fontUBO), 0, &modelMatrixData);
					memcpy(modelMatrixData, &fontUBO, sizeof(fontUBO));
					vkUnmapMemory(device, resources_.GPUDescriptors[resources_.descriptorBindingsConfig[fontUboDescriptorBindingIndex].globalDescriptorOffset].GPUBuffer->deviceMemory);

					const unsigned int linkedDescriptorSetID = resources_.pipelineConfigs[SpecificPipeline::FONT_PIPELINE].linkedDescriptorSetIDs[0];
					const DescriptorSet& currentDescriptorSet = resources_.descriptorSetsConfig[linkedDescriptorSetID];
					vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, resources_.pipelineConfigs[SpecificPipeline::FONT_PIPELINE].pipelineLayout, 0, 1,
											&(*(resources_.descriptorSetsChunks.GetVectorContainer() + currentDescriptorSet.descriptorSetOffset + currentActorMemoryOffset + j)), 0, nullptr);
					const unsigned int linkedDescriptorSetID1 = resources_.pipelineConfigs[SpecificPipeline::FONT_PIPELINE].linkedDescriptorSetIDs[1];
					const unsigned int fontAtlasTextureID = 6;
					const DescriptorSet& currentDescriptorSet1 = resources_.descriptorSetsConfig[linkedDescriptorSetID1];
					vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, resources_.pipelineConfigs[SpecificPipeline::FONT_PIPELINE].pipelineLayout, 1, 1,
											&(*(resources_.descriptorSetsChunks.GetVectorContainer() + currentDescriptorSet1.descriptorSetOffset + MAX_FRAMES_IN_FLIGHT * fontAtlasTextureID + currentFrame)), 0, nullptr);

					vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indicesContainerSize), 1, 0, 0, 0);
				}
				currentActorMemoryOffset += font.font_string.GetSize();
			}
//		}

        vkCmdEndRenderPass(commandBuffer);

        // if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        //     throw std::runtime_error("failed to record command buffer!");
        // }
    }

}
