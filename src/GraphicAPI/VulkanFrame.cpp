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
	void CVulkanRenderer::executeSecondaryCommandBuffer( VkRenderPass renderPass, VkFramebuffer frameBuffer, VkExtent2D extent,
														 VkCommandBuffer primaryCommandBuffer, VkCommandBuffer secondaryCommandBuffer ) {
		VkClearValue shadowMapClearValues[1];
		shadowMapClearValues[0].depthStencil.depth = 1.0f;
		shadowMapClearValues[0].depthStencil.stencil = 0;

		VkRenderPassBeginInfo shadowMapRenderPassInfo{};
		shadowMapRenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		shadowMapRenderPassInfo.pNext = NULL;
		shadowMapRenderPassInfo.renderPass = renderPass;
		shadowMapRenderPassInfo.framebuffer = frameBuffer;
		shadowMapRenderPassInfo.renderArea.offset.x = 0;
		shadowMapRenderPassInfo.renderArea.offset.y = 0;
		shadowMapRenderPassInfo.renderArea.extent.width = extent.width;
		shadowMapRenderPassInfo.renderArea.extent.height = extent.height;
		shadowMapRenderPassInfo.clearValueCount = 1;
		shadowMapRenderPassInfo.pClearValues = shadowMapClearValues;

		vkCmdBeginRenderPass(primaryCommandBuffer, &shadowMapRenderPassInfo,
							 VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);
		vkCmdExecuteCommands(primaryCommandBuffer, 1,
							 &secondaryCommandBuffer);
		vkCmdEndRenderPass(primaryCommandBuffer);
	}

    void CVulkanRenderer::recordCommandBuffer(VkCommandBuffer& commandBuffer, uint32_t imageIndex) {
        // VkCommandBufferBeginInfo beginInfo{};
        // beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        // if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        //     throw std::runtime_error("failed to begin recording command buffer!");
        // }

		namespace cm = GLVM::ecs::components;
//		vkDebugUtils::CreateEndDebugUtilsLabelEXT(instance, commandBuffer);

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = resources_.renderPasses[SpecificPipeline::MAIN_RENDER_PIPELINE];
        renderPassInfo.framebuffer = swapChainFramebuffers[imageIndex];
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent.height = swapChainExtent.height;
		renderPassInfo.renderArea.extent.width = swapChainExtent.width;

		for( unsigned int playerIndex = 0; playerIndex < frame_.players.GetSize(); ++playerIndex ) {
			updateViewPositionUniformBuffer(currentFrame, playerIndex);
		}

        std::array<VkClearValue, 2> clearValues{};
		if( frame_.players.GetSize() == 0 ) {
			clearValues[0].color = {{0.7f, 0.2f, 0.2f, 1.0f}};   ///< Player death screen
		} else {
			clearValues[0].color = {{0.2f, 0.2f, 0.2f, 1.0f}};
		}
        clearValues[1].depthStencil = {1.0f, 0};

        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, resources_.pipelineConfigs[SpecificPipeline::MAIN_RENDER_PIPELINE].pipeline);

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

		for ( unsigned int i = 0; i < frame_.actors.GetSize(); ++i ) {
			RenderActor actor = frame_.actors[i];
			unsigned int uiVertexId = actor.meshID;
			unsigned int diffuseTextureIndex = actor.diffuseTextureIndex;
			unsigned int specularTextureIndex = actor.specularTextureIndex;

			unsigned int uboIndex = currentFrame * matrixUboDescriptorsNumber + i;
			updateMatrixUniformBuffer(uboIndex, i);
			const unsigned int linkedDescriptorSetID = resources_.pipelineConfigs[SpecificPipeline::MAIN_RENDER_PIPELINE].linkedDescriptorSetIDs[0];
			const DescriptorSet& currentDescriptorSet = resources_.descriptorSetsConfig[linkedDescriptorSetID];
			vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, resources_.pipelineConfigs[SpecificPipeline::MAIN_RENDER_PIPELINE].pipelineLayout,
									0, 1, &(*(resources_.descriptorSetsChunks.GetVectorContainer() + currentDescriptorSet.descriptorSetOffset + uboIndex)), 0, nullptr);

			const unsigned int linkedDescriptorSetID1 = resources_.pipelineConfigs[SpecificPipeline::MAIN_RENDER_PIPELINE].linkedDescriptorSetIDs[1];
			const DescriptorSet& currentDescriptorSet1 = resources_.descriptorSetsConfig[linkedDescriptorSetID1];
			vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, resources_.pipelineConfigs[SpecificPipeline::MAIN_RENDER_PIPELINE].pipelineLayout,
									1, 1, &(*(resources_.descriptorSetsChunks.GetVectorContainer() + currentDescriptorSet1.descriptorSetOffset + currentFrame)), 0, nullptr);

			VkBuffer vertexBuffers[] = {vertexBufferContainer[uiVertexId]};
			VkDeviceSize offsets[] = {0};
			vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

			vkCmdBindIndexBuffer(commandBuffer, indexBufferContainer[uiVertexId], 0, VK_INDEX_TYPE_UINT32);

			unsigned int indicesContainerSize = assets_.aIndices_[uiVertexId].size();

			const unsigned int linkedDescriptorSetID2 = resources_.pipelineConfigs[SpecificPipeline::MAIN_RENDER_PIPELINE].linkedDescriptorSetIDs[2];
			const DescriptorSet& currentDescriptorSet2 = resources_.descriptorSetsConfig[linkedDescriptorSetID2];
			vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, resources_.pipelineConfigs[SpecificPipeline::MAIN_RENDER_PIPELINE].pipelineLayout, 2, 1,
									&(*(resources_.descriptorSetsChunks.GetVectorContainer() + currentDescriptorSet2.descriptorSetOffset + MAX_FRAMES_IN_FLIGHT * specularTextureIndex + currentFrame)), 0, nullptr);
			const unsigned int linkedDescriptorSetID3 = resources_.pipelineConfigs[SpecificPipeline::MAIN_RENDER_PIPELINE].linkedDescriptorSetIDs[3];
			const DescriptorSet& currentDescriptorSet3 = resources_.descriptorSetsConfig[linkedDescriptorSetID3];
			vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, resources_.pipelineConfigs[SpecificPipeline::MAIN_RENDER_PIPELINE].pipelineLayout, 3, 1,
									&(*(resources_.descriptorSetsChunks.GetVectorContainer() + currentDescriptorSet3.descriptorSetOffset + MAX_FRAMES_IN_FLIGHT * diffuseTextureIndex + currentFrame)), 0, nullptr);

			vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indicesContainerSize), 1, 0, 0, 0);
		}
//		}

        vkCmdEndRenderPass(commandBuffer);

        // if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        //     throw std::runtime_error("failed to record command buffer!");
        // }
    }

    void CVulkanRenderer::createSyncObjects(std::vector<VkSemaphore>& imageAvailableSemaphores,
											std::vector<VkSemaphore>& renderFinishedSemaphores,
											std::vector<VkFence>& inFlightFences) {
        imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        renderFinishedSemaphores.resize(swapChainImages.size());
        inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
            if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
                vkCreateFence(device, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to create synchronization objects for a frame!");
            }
        }

		for (size_t i = 0; i < swapChainImages.size(); ++i) {
            if ( vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ) {
                throw std::runtime_error("failed to create synchronization objects for a frame!");
            }
        }
    }

    void CVulkanRenderer::updateMatrixUniformBuffer(uint32_t offset, unsigned int actor) {
        ModelMatrixUBO modelMatrixUBO{};

		modelMatrixUBO.model = frame_.actors[actor].modelMatrix;

        modelMatrixUBO.view = frame_.viewMatrix;
        modelMatrixUBO.proj = frame_.projectionMatrix;

		/// Start of animation logic
		// for ( unsigned int j = 0; j < MAX_JOINTS_NUMBER; ++j ) {
		// 	modelMatrixUBO.jointMatrices[j] = frame_.actors[actor].jointMatrices[j];
		// }
		for ( unsigned int j = 0; j < frame_.actors[actor].jointMatrices.GetSize(); ++j ) {
			modelMatrixUBO.jointMatrices[j] = frame_.actors[actor].jointMatrices[j];
		}
		/// End of animation logic

		modelMatrixUBO.ambient = frame_.actors[actor].ambient;
		modelMatrixUBO.shininess = frame_.actors[actor].shininess;

		for ( uint32_t i = 0; i < directionalLightNumber; ++i )
			modelMatrixUBO.dirSpaceMatrix[i] = dirLightSpaceMatrix[i];

		for ( uint32_t i = 0; i < spotLightNumber; ++i )
			modelMatrixUBO.spotSpaceMatrix[i] = spotLightSpaceMatrix[i];

		modelMatrixUBO.directionalLightsNumber = directionalLightNumber;
		modelMatrixUBO.spotLightsNumber        = spotLightNumber;

        void* modelMatrixData;
        vkMapMemory(device, resources_.GPUDescriptors[DescriptorSetDataLink::MAIN_RENDER_MATRIX_UBO].GPUBuffer->deviceMemory, sizeof(modelMatrixUBO) * offset,
					sizeof(modelMatrixUBO), 0, &modelMatrixData);
        memcpy(modelMatrixData, &modelMatrixUBO, sizeof(modelMatrixUBO));
        vkUnmapMemory(device, resources_.GPUDescriptors[DescriptorSetDataLink::MAIN_RENDER_MATRIX_UBO].GPUBuffer->deviceMemory);
    }

	void CVulkanRenderer::updateViewPositionUniformBuffer( uint32_t currentImage, uint32_t playerIndex ) {
		LightData lightDataUBO{};
		lightDataUBO.viewPosition = frame_.players[playerIndex].position;

		DirectionalLight directionalLight{};
		directionalLightNumber = frame_.directionalLights.GetSize();
		assert(directionalLightNumber <= 4 && "Directional lights number greater then 4");
		for ( unsigned int i = 0; i < directionalLightNumber; ++i ) {
			RenderDirectionalLight dirLight = frame_.directionalLights[i];

			directionalLight.position  = dirLight.position;
			directionalLight.direction = dirLight.direction;
			directionalLight.ambient   = dirLight.ambient;
			directionalLight.diffuse   = dirLight.diffuse;
			directionalLight.specular  = dirLight.specular;

			lightDataUBO.directionalLights[i] = directionalLight;
		}
		lightDataUBO.directionalLightsArraySize = directionalLightNumber;

		pointLightNumber = frame_.pointLights.GetSize();
		assert(pointLightNumber <= POINT_LIGHTS_NUMBER && "Point lights number greater than 32");
		for ( unsigned int i = 0; i < pointLightNumber; ++i ) {
			RenderPointLight pointLight = frame_.pointLights[i];
			PointLight pointLightUBO{};

			pointLightUBO.position  = pointLight.position;
			pointLightUBO.ambient   = pointLight.ambient;
			pointLightUBO.diffuse   = pointLight.diffuse;
			pointLightUBO.specular  = pointLight.specular;
			pointLightUBO.constant  = pointLight.constant;
			pointLightUBO.linear    = pointLight.linear;
			pointLightUBO.quadratic = pointLight.quadratic;

			lightDataUBO.pointLights[i] = pointLightUBO;
		}
		lightDataUBO.pointLightsArraySize = pointLightNumber;
		lightDataUBO.farPlane = 100.0f;

		SpotLight spotLightUBO{};
		spotLightNumber = frame_.spotLights.GetSize();
		assert(spotLightNumber <= 8 && "Spot light number greater then 8");
		for ( unsigned int i = 0; i < spotLightNumber; ++i ) {
			RenderSpotLight spotLight = frame_.spotLights[i];

			spotLightUBO.position    = spotLight.position;
			spotLightUBO.direction   = spotLight.direction;
			spotLightUBO.cutOff      = std::cos(Radians(spotLight.cutOff));
			spotLightUBO.outerCutOff = std::cos(Radians(spotLight.outerCutOff));
			spotLightUBO.ambient     = spotLight.ambient;
			spotLightUBO.diffuse     = spotLight.diffuse;
			spotLightUBO.specular    = spotLight.specular;
			spotLightUBO.constant    = spotLight.constant;
			spotLightUBO.linear      = spotLight.linear;
			spotLightUBO.quadratic   = spotLight.quadratic;

			lightDataUBO.spotLights[i] = spotLightUBO;
		}
		lightDataUBO.spotLightArraySize = spotLightNumber;

		std::random_device rd;
		std::mt19937 mersenne(rd());
		std::uniform_int_distribution<int> distributionTileIndex(0, INDIRECT_TEXTURE_HEIGHT * INDIRECT_TEXTURE_WIDTH);

		if( print == true ) {
		for( int i = 0; i < INDIRECT_TEXTURE_HEIGHT * INDIRECT_TEXTURE_WIDTH / 4 + 1; ++i )
			for( int j = 0; j < 4; ++j ) {
				int randomTileIndex = distributionTileIndex(mersenne);
//				randomTileIndex = 20;
				indirectTexture[i][j] = randomTileIndex;
				// if( print )
				// 	std::cout << "element: " << i * 4 + j << " value: " << indirectTexture[i][j] << std::endl;

				// std::cout << "index: " << i * INDIRECT_TEXTURE_WIDTH * 4 + j * 4 + 3 << std::endl;
				// lightDataUBO.indirectTexture[i * INDIRECT_TEXTURE_WIDTH * 4 + j * 4 + 3] = randomTileIndex;
//				std::cout << "element: " << i * INDIRECT_TEXTURE_WIDTH + j << " equal: " << lightDataUBO.indirectTexture[i * INDIRECT_TEXTURE_WIDTH + j] << std::endl;
			}
		}
		print = false;
		// if( print == true )
		// 	print = false;

		lightDataUBO.tilesetTilesCount = vec2(TILESET_ROW, TILESET_COLUMN);
		lightDataUBO.tilesRaw = 8;
		lightDataUBO.tilesColumn = 8;
		for( int i = 0; i < INDIRECT_TEXTURE_HEIGHT * INDIRECT_TEXTURE_WIDTH / 4 + 1; ++i ) {
			lightDataUBO.indirectTexture[i] = indirectTexture[i];
		}

        void* data;
		unsigned int lightDataUboDescriptorBindingIndex = resources_.descriptorSetsConfig[DescriptorSetDataLink::MAIN_RENDER_LIGHT_DATA_UBO].descriptorsBindingsIDs[0];
        vkMapMemory(device, resources_.GPUDescriptors[resources_.descriptorBindingsConfig[lightDataUboDescriptorBindingIndex].globalDescriptorOffset].GPUBuffer->deviceMemory, sizeof(lightDataUBO) * currentImage,
					sizeof(lightDataUBO), 0, &data);
        memcpy(data, &lightDataUBO, sizeof(lightDataUBO));
        vkUnmapMemory(device, resources_.GPUDescriptors[resources_.descriptorBindingsConfig[lightDataUboDescriptorBindingIndex].globalDescriptorOffset].GPUBuffer->deviceMemory);
	}

    void CVulkanRenderer::mainRenderDrawFrame() {
		namespace cm = GLVM::ecs::components;
#ifdef VK_USE_PLATFORM_WIN32_KHR
        if (IsIconic(Window->GetModernWindowHWND()))
            return;
#endif
#if defined(VK_USE_PLATFORM_WAYLAND_KHR) || defined(VK_USE_PLATFORM_WIN32_KHR)
        if (Window->width && Window->height &&
            (Window->width != swapChainExtent.width || Window->height != swapChainExtent.height))
            recreateSwapChain();
#endif
        vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

        uint32_t imageIndex;
		/* vkAcquireNextImageKHR give index of image that WILL BE SOON available for rendering and signal imageAvailablesemaphore when its so.
		   GraphicsQueue waint for this semaphore bacause we pass it in submitInfo.
		 */
        VkResult result = vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            recreateSwapChain();
            return;
        } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("failed to acquire swap chain image!");
        }

        vkResetFences(device, 1, &inFlightFences[currentFrame]);
        vkResetCommandBuffer(mainRenderCommandBuffers[currentFrame], /*VkCommandBufferResetFlagBits*/ 0);
		// directionalLightRecordCoomandBuffer(directionalLightSecondaryCommandBuffers, currentFrame);
		// spotLightRecordCommandBuffer(spotLightSecondaryCommandBuffers, currentFrame);
		// pointLightRecordCommandBuffer(pointLightSecondaryCommandBuffers, currentFrame);

		auto future1 = renderThreadPool->enqueue([this]() {
			directionalLightRecordCoomandBuffer(directionalLightSecondaryCommandBuffers, this->currentFrame);
		});

		auto future2 = renderThreadPool->enqueue([this]() {
			spotLightRecordCommandBuffer(spotLightSecondaryCommandBuffers, this->currentFrame);
		});

		auto future3 = renderThreadPool->enqueue([this]() {
			pointLightRecordCommandBuffer(pointLightSecondaryCommandBuffers, this->currentFrame);
		});

		future1.wait();
		future2.wait();
		future3.wait();
        future1.get();
        future2.get();
        future3.get();

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(mainRenderCommandBuffers[currentFrame], &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("failed to begin recording command buffer!");
        }
		for ( uint32_t directionalLightCounter = 0; directionalLightCounter < frame_.directionalLights.GetSize(); ++ directionalLightCounter ) {
			executeSecondaryCommandBuffer( resources_.renderPasses[SpecificPipeline::DIRECTIONAL_LIGHT_PIPELINE], directionalLightShadowMapFrameBuffers[directionalLightCounter],
										   swapChainExtent, mainRenderCommandBuffers[currentFrame], directionalLightSecondaryCommandBuffers[currentFrame * directionalLightNumber + directionalLightCounter] );
		}
		for ( uint32_t spotLightCounter = 0; spotLightCounter < frame_.spotLights.GetSize(); ++ spotLightCounter ) {
			executeSecondaryCommandBuffer( resources_.renderPasses[SpecificPipeline::SPOT_LIGHT_PIPELINE], spotLightShadowMapFrameBuffers[spotLightCounter],
										   swapChainExtent, mainRenderCommandBuffers[currentFrame], spotLightSecondaryCommandBuffers[currentFrame * spotLightNumber + spotLightCounter] );
		}
		for ( uint32_t pointLightCounter = 0; pointLightCounter < frame_.pointLights.GetSize(); ++pointLightCounter ) {
			uint32_t maxCubeMapLayers = 6;
			for ( uint32_t cubeMapLayerCounter = 0; cubeMapLayerCounter < maxCubeMapLayers; ++cubeMapLayerCounter ) {
				VkExtent2D extent;
				extent.width  = SHADOW_MAP_SIZE;
				extent.height = SHADOW_MAP_SIZE;
				executeSecondaryCommandBuffer( resources_.renderPasses[SpecificPipeline::POINT_LIGHT_PIPELINE], pointLightShadowMapFrameBuffers[pointLightCounter][cubeMapLayerCounter],
											   extent, mainRenderCommandBuffers[currentFrame], pointLightSecondaryCommandBuffers[currentFrame * pointLightNumber * maxCubeMapLayers +
																																		  pointLightCounter * maxCubeMapLayers + cubeMapLayerCounter] );
			}
		}

        recordCommandBuffer(mainRenderCommandBuffers[currentFrame], imageIndex);
		hudRecordCommandBuffer(mainRenderCommandBuffers[currentFrame], imageIndex);
		fontRecordCommandBuffer(mainRenderCommandBuffers[currentFrame], imageIndex);
		if ( frame_.isInventoryOpened ) {
			uiRecordCommandBuffer(mainRenderCommandBuffers[currentFrame], imageIndex);
			uiIconsRecordCommandBuffer(mainRenderCommandBuffers[currentFrame], imageIndex);
		}

		if( frame_.isDebugCollisitionsActive ) {
			collisionsDebugRecordCommandBuffer(mainRenderCommandBuffers[currentFrame], imageIndex);
		}

		hudScreenRecordCommandBuffer(mainRenderCommandBuffers[currentFrame], imageIndex);
//		sdfRecordCommandBuffer(mainRenderCommandBuffers[currentFrame], imageIndex);

//		spacialGridDebugRecordCommandBuffer(mainRenderCommandBuffers[currentFrame], imageIndex);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		/// GraphicsQueue wait for swapchain image when its become available.
        VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &mainRenderCommandBuffers[currentFrame];

        VkSemaphore signalSemaphores[] = {renderFinishedSemaphores[imageIndex]};
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
            throw std::runtime_error("failed to submit draw command buffer!");
        }

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        VkSwapchainKHR swapChains[] = {swapChain};
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;

        presentInfo.pImageIndices = &imageIndex;

        result = vkQueuePresentKHR(presentQueue, &presentInfo);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
            framebufferResized = false;
            recreateSwapChain();
        } else if (result != VK_SUCCESS) {
            throw std::runtime_error("failed to present swap chain image!");
        }

        currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
//		currentFrame = 0;
    }

}
