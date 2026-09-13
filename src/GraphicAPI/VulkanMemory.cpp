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
    void CVulkanRenderer::createTextureImage() {
		uint32_t texWidth, texHeight;
		[[maybe_unused]] uint32_t texChannels;

		unsigned int readableTextureDescriptorBindingIndex = resources_.descriptorSetsConfig[DescriptorSetDataLink::RIDABLE_TEXTURES].descriptorsBindingsIDs[0];
        for(unsigned int i = 0; i < assets_.initializeTextureData_.size(); ++i)
        {
			VkDeviceSize imageSize{};
			unsigned char* pixels;
			[[maybe_unused]] const char* path_to_stb_image = nullptr;

			#ifndef STB_IMAGE_IMPLEMENTATION
            imageSize = assets_.initializeTextureData_[i].dat_length_;
            pixels = assets_.initializeTextureData_[i].u_iData_;
            texWidth = assets_.initializeTextureData_[i].iWidth_;
            texHeight = assets_.initializeTextureData_[i].iHeight_;
			#endif

			#ifdef STB_IMAGE_IMPLEMENTATION
			path_to_stb_image = assets_.initializeTextureData_[i].path_to_image;
			pixels = stbi_load(path_to_stb_image, reinterpret_cast<int*>(&texWidth), reinterpret_cast<int*>(&texHeight),
							   reinterpret_cast<int*>(&texChannels), STBI_rgb_alpha);
			imageSize = texWidth * texHeight * 4;
			#endif

			if (!pixels) {
                throw std::runtime_error("failed to load texture image!");
            }

            VkBuffer stagingBuffer;
            VkDeviceMemory stagingBufferMemory;
            createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

            void* data;
            vkMapMemory(device, stagingBufferMemory, 0, imageSize, 0, &data);
            memcpy(data, pixels, static_cast<size_t>(imageSize));
            vkUnmapMemory(device, stagingBufferMemory);
			VK_Image textureImage = {
				.image = VkImage{},
				.deviceMemory = VkDeviceMemory{},
				.viewType = VK_IMAGE_VIEW_TYPE_2D,
				.createFlags  = 0,
				.memoryPropertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
				.usageFlags = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
				.aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT,
				.format = VK_FORMAT_R8G8B8A8_SRGB,
				.tiling = VK_IMAGE_TILING_OPTIMAL,
				.arrayLayers = 1,
				.width = texWidth,
				.height = texHeight
			};

            createImage(textureImage);

            transitionImageLayout(textureImage.image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
            copyBufferToImage(stagingBuffer, textureImage.image, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
            transitionImageLayout(textureImage.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

//			resources_.GPUDescriptors[resources_.descriptorBindingsConfig[readableTextureDescriptorBindingIndex].globalDescriptorOffset + i].GPUImage = new VK_Image;
			*resources_.GPUDescriptors[resources_.descriptorBindingsConfig[readableTextureDescriptorBindingIndex].globalDescriptorOffset + i].GPUImage = textureImage;

            vkDestroyBuffer(device, stagingBuffer, nullptr);
            vkFreeMemory(device, stagingBufferMemory, nullptr);
        }
    }

	void CVulkanRenderer::initializeGameLevelVertices() {
		for ( unsigned int m = 0; m < assets_.levelGeneratedVertices.size(); ++m ) {
			assets_.aVertices_.push_back(assets_.levelGeneratedVertices[m]);
			assets_.aIndices_.push_back(assets_.levelGeneratedIndices[m]);
			assets_.jointMatricesPerMesh.Push({});
			assets_.frames.Push({});
			for( int i = 0; i < 64; ++i ) {
				assets_.frames[assets_.frames.GetSize() - 1].Push(0.0f);
			}
			int maximumJoints     = 64;
			core::vector<core::vector<mat4>> jointMatrices;
			for ( int i = 0; i < maximumJoints; ++i) {
				core::vector<mat4>  globalAllFrameNodeMatrix;
				int numberOfFrames = 64;
				for ( int j = 0; j < numberOfFrames; ++j ) {
					mat4 unitMatrix(1.0f);
					globalAllFrameNodeMatrix.Push(unitMatrix);
				}

				jointMatrices.Push(globalAllFrameNodeMatrix);
			}
			assets_.jointMatricesPerMesh[assets_.jointMatricesPerMesh.GetSize() - 1] = jointMatrices;

			uint32_t nextIndexGLTF = wavefrontObjCounter + gltfCounter + m;

			vertexBufferContainer.emplace_back();
			vertexBufferMemoryContainer.emplace_back();
			createVertexBuffer(vertexBufferContainer[nextIndexGLTF], vertexBufferMemoryContainer[nextIndexGLTF], assets_.aVertices_[nextIndexGLTF]);

			indexBufferContainer.emplace_back();
			indexBufferMemoryContaner.emplace_back();
			createIndexBuffer(indexBufferContainer[nextIndexGLTF], indexBufferMemoryContaner[nextIndexGLTF], assets_.aIndices_[nextIndexGLTF]);
		}
	}

	void CVulkanRenderer::initializeVertexBuffersWithWavefrontData() {
        for (unsigned int m = 0; m < assets_.pathsArray_.size(); ++m) {
			vertexBufferContainer.emplace_back();
			vertexBufferMemoryContainer.emplace_back();
			createVertexBuffer(vertexBufferContainer[m], vertexBufferMemoryContainer[m], assets_.aVertices_[m]);

			indexBufferContainer.emplace_back();
			indexBufferMemoryContaner.emplace_back();
			createIndexBuffer(indexBufferContainer[m], indexBufferMemoryContaner[m], assets_.aIndices_[m]);
			++wavefrontObjCounter;
		}
	}

	void CVulkanRenderer::initializeVertexBuffersWithGLTFData() {
		for (unsigned int m = 0; m < assets_.pathsGLTF_.GetSize(); ++m) {
			uint32_t nextIndexGLTF = wavefrontObjCounter + m;
			vertexBufferContainer.emplace_back();
			vertexBufferMemoryContainer.emplace_back();
			createVertexBuffer(vertexBufferContainer[nextIndexGLTF], vertexBufferMemoryContainer[nextIndexGLTF], assets_.aVertices_[nextIndexGLTF]);

			indexBufferContainer.emplace_back();
			indexBufferMemoryContaner.emplace_back();
			createIndexBuffer(indexBufferContainer[nextIndexGLTF], indexBufferMemoryContaner[nextIndexGLTF], assets_.aIndices_[nextIndexGLTF]);
			++gltfCounter;
		}
	}

	void CVulkanRenderer::initializeVertexBuffersWithFontData() {
        fontVertexBufferContainer.resize(128);
        fontVertexBufferMemoryContainer.resize(128);
        fontIndexBufferContainer.resize(128);
        fontIndexBufferMemoryContaner.resize(128);
		for ( unsigned int i = 0; i < assets_.symbolGVerticesContainer.GetSize(); ++i ) {
				const unsigned int nextBufferIndex = assets_.fontIndicesContainer[i];
				core::vector<Vertex> symbol_g_vertices = assets_.symbolGVerticesContainer[i];

				createVertexBuffer(fontVertexBufferContainer[nextBufferIndex], fontVertexBufferMemoryContainer[nextBufferIndex], symbol_g_vertices);
				createIndexBuffer(fontIndexBufferContainer[nextBufferIndex], fontIndexBufferMemoryContaner[nextBufferIndex], symbol_g_indices);
		}
	}

	void CVulkanRenderer::clearVK_Image( VK_Image* textureImages ) {
		vkDestroySampler(device, textureImages->sampler, nullptr);
		for ( unsigned int j = 0; j < textureImages->views.size(); ++j )
			vkDestroyImageView(device, textureImages->views[j], nullptr);

		textureImages->views.clear();

		vkDestroyImage(device, textureImages->image, nullptr);
		vkFreeMemory(device, textureImages->deviceMemory, nullptr);
        textureImages->image = VK_NULL_HANDLE;
        textureImages->deviceMemory = VK_NULL_HANDLE;
        textureImages->sampler = VK_NULL_HANDLE;
	}

    void CVulkanRenderer::createImageViews() {
        swapChainImageViews.resize(swapChainImages.size());

        for (uint32_t i = 0; i < swapChainImages.size(); i++) {
			VK_Image swapChainImage		 = {
				.image				 = swapChainImages[i],
				.viewType			 = VK_IMAGE_VIEW_TYPE_2D,
				.aspectFlags         = VK_IMAGE_ASPECT_COLOR_BIT,
				.format				 = swapChainImageFormat,
				.red                 = VK_COMPONENT_SWIZZLE_IDENTITY,
				.green               = VK_COMPONENT_SWIZZLE_IDENTITY,
				.blue                = VK_COMPONENT_SWIZZLE_IDENTITY,
				.alpha               = VK_COMPONENT_SWIZZLE_IDENTITY,
				.arrayLayers         = 1,
				.width               = swapChainExtent.width,
				.height              = swapChainExtent.height
			};

            swapChainImageViews[i] = createImageView(swapChainImage, 0, 1);
        }
    }

    void CVulkanRenderer::createDepthResources() {
        VkFormat depthFormat = findDepthFormat();

		VK_Image depthImage		 = {
			.image				 = VkImage{},
			.deviceMemory		 = VkDeviceMemory{},
			.viewType			 = VK_IMAGE_VIEW_TYPE_2D,
			.createFlags		 = 0,
			.memoryPropertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			.usageFlags			 = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			.aspectFlags         = VK_IMAGE_ASPECT_DEPTH_BIT,
			.format				 = depthFormat,
			.tiling				 = VK_IMAGE_TILING_OPTIMAL,
			.arrayLayers		 = 1,
			.width				 = swapChainExtent.width,
			.height				 = swapChainExtent.height,
		};

		createImage(depthImage);
		mainDepthPipelineImage = depthImage.image;
		mainDepthPipelineImageMemory = depthImage.deviceMemory;
        mainDepthImageView = createImageView(depthImage, 0, 1);
    }

	void CVulkanRenderer::createDirectionalLightShadowMapDepthResources() {
		unsigned int directionalLightDescriptorBindingIndex = resources_.descriptorSetsConfig[DescriptorSetDataLink::MAIN_RENDER_LIGHT_DATA_UBO].descriptorsBindingsIDs[1];
		for ( unsigned int i = 0; i < DIRECTIONAL_LIGHTS_NUMBER; ++i ) {
			VK_Image depthImage		 = {
				.image				 = VkImage{},
				.deviceMemory		 = VkDeviceMemory{},
				.viewType			 = VK_IMAGE_VIEW_TYPE_2D,
				.createFlags		 = 0,
				.memoryPropertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
				.usageFlags			 = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
				.aspectFlags         = VK_IMAGE_ASPECT_DEPTH_BIT,
				.format				 = findDepthFormat(),
				.tiling				 = VK_IMAGE_TILING_OPTIMAL,
				.arrayLayers		 = 1,
				.width				 = swapChainExtent.width,
				.height				 = swapChainExtent.height,
			};

			createImage(depthImage);

			VkCommandBuffer commandBuffer = beginSingleTimeCommands(mainRenderCommandPool);

			VkImageMemoryBarrier barrier{};
			barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			barrier.newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
			barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			barrier.image = depthImage.image;
			barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
			barrier.subresourceRange.baseMipLevel = 0;
			barrier.subresourceRange.levelCount = 1;
			barrier.subresourceRange.baseArrayLayer = 0;
			barrier.subresourceRange.layerCount = 1;

			VkPipelineStageFlags sourceStage;
			VkPipelineStageFlags destinationStage;

			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = 0;

			sourceStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

			vkCmdPipelineBarrier(
				commandBuffer,
				sourceStage, destinationStage,
				0,
				0, nullptr,
				0, nullptr,
				1, &barrier
				);

			endSingleTimeCommands(mainRenderCommandPool, commandBuffer);

			depthImage.views.push_back(createImageView(depthImage, 0, 1));
			vkDebugUtils::setImageDebugObjectName(device, depthImage, "directional light");
			*resources_.GPUDescriptors[resources_.descriptorBindingsConfig[directionalLightDescriptorBindingIndex].globalDescriptorOffset + i].GPUImage = depthImage;
		}
	}

	void CVulkanRenderer::createSpotLightShadowMapDepthResources() {
		unsigned int spotLightDescriptorBindingIndex = resources_.descriptorSetsConfig[DescriptorSetDataLink::MAIN_RENDER_LIGHT_DATA_UBO].descriptorsBindingsIDs[3];
		for ( unsigned int i = 0; i < SPOT_LIGHTS_NUMBER; ++i ) {
			VK_Image depthImage		 = {
				.image				 = VkImage{},
				.deviceMemory		 = VkDeviceMemory{},
				.viewType			 = VK_IMAGE_VIEW_TYPE_2D,
				.createFlags		 = 0,
				.memoryPropertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
				.usageFlags			 = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
				.aspectFlags         = VK_IMAGE_ASPECT_DEPTH_BIT,
				.format				 = findDepthFormat(),
				.tiling				 = VK_IMAGE_TILING_OPTIMAL,
				.arrayLayers		 = 1,
				.width				 = swapChainExtent.width,
				.height				 = swapChainExtent.height,
			};

			createImage(depthImage);

			VkCommandBuffer commandBuffer = beginSingleTimeCommands(mainRenderCommandPool);

			VkImageMemoryBarrier barrier{};
			barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			barrier.newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
			barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			barrier.image = depthImage.image;
			barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
			barrier.subresourceRange.baseMipLevel = 0;
			barrier.subresourceRange.levelCount = 1;
			barrier.subresourceRange.baseArrayLayer = 0;
			barrier.subresourceRange.layerCount = 1;

			VkPipelineStageFlags sourceStage;
			VkPipelineStageFlags destinationStage;

			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = 0;

			sourceStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

			vkCmdPipelineBarrier(
				commandBuffer,
				sourceStage, destinationStage,
				0,
				0, nullptr,
				0, nullptr,
				1, &barrier
				);

			endSingleTimeCommands(mainRenderCommandPool, commandBuffer);

			depthImage.views.push_back(createImageView(depthImage, 0, 1));
			vkDebugUtils::setImageDebugObjectName(device, depthImage, "spot light");
			*resources_.GPUDescriptors[resources_.descriptorBindingsConfig[spotLightDescriptorBindingIndex].globalDescriptorOffset + i].GPUImage = depthImage;
		}
	}

	void CVulkanRenderer::createPointLightShadowMapDepthResources() {
		unsigned int descriptorBindingIndex = resources_.descriptorSetsConfig[DescriptorSetDataLink::MAIN_RENDER_LIGHT_DATA_UBO].descriptorsBindingsIDs[2];
		for ( unsigned int i = 0; i < POINT_LIGHTS_NUMBER; ++i ) {
			VK_Image depthImage		 = {
				.image				 = VkImage{},
				.deviceMemory		 = VkDeviceMemory{},
				.viewType			 = VK_IMAGE_VIEW_TYPE_2D,
				.createFlags		 = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT,
				.memoryPropertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
				.usageFlags			 = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
				.aspectFlags         = VK_IMAGE_ASPECT_DEPTH_BIT,
				.format				 = findDepthFormat(),
				.tiling				 = VK_IMAGE_TILING_OPTIMAL,
				.arrayLayers		 = 6,
				.width				 = SHADOW_MAP_SIZE,
				.height				 = SHADOW_MAP_SIZE
			};

			createImage(depthImage);

			for ( unsigned int j = 0; j < 6; ++j ) {
				VkCommandBuffer commandBuffer = beginSingleTimeCommands(mainRenderCommandPool);

				VkImageMemoryBarrier barrier{};
				barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
				barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
				barrier.newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
//				barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
				barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
				barrier.image = depthImage.image;
				barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
				barrier.subresourceRange.baseMipLevel = 0;
				barrier.subresourceRange.levelCount = 1;
				barrier.subresourceRange.baseArrayLayer = 0;
				barrier.subresourceRange.layerCount = 6;

				VkPipelineStageFlags sourceStage;
				VkPipelineStageFlags destinationStage;

				barrier.srcAccessMask = 0;
				barrier.dstAccessMask = 0;

				sourceStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
				destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

				vkCmdPipelineBarrier(
					commandBuffer,
					sourceStage, destinationStage,
					0,
					0, nullptr,
					0, nullptr,
					1, &barrier
					);

				endSingleTimeCommands(mainRenderCommandPool, commandBuffer);


				depthImage.views.push_back(createImageView(depthImage, j, 1));
			}

			vkDebugUtils::setImageDebugObjectName(device, depthImage, "point light laryer");
			*resources_.GPUDescriptors[resources_.descriptorBindingsConfig[descriptorBindingIndex].globalDescriptorOffset + i].GPUImage = depthImage;
		}

		for ( unsigned int i = 0; i < POINT_LIGHTS_NUMBER; ++i ) {
		    (*resources_.GPUDescriptors[resources_.descriptorBindingsConfig[descriptorBindingIndex].globalDescriptorOffset + i].GPUImage).viewType = VK_IMAGE_VIEW_TYPE_CUBE;

			vkDebugUtils::setImageDebugObjectName(device, *resources_.GPUDescriptors[resources_.descriptorBindingsConfig[descriptorBindingIndex].globalDescriptorOffset + i].GPUImage, "point light cube");
			(*resources_.GPUDescriptors[resources_.descriptorBindingsConfig[descriptorBindingIndex].globalDescriptorOffset + i].GPUImage).views.push_back(
				createImageView(*resources_.GPUDescriptors[resources_.descriptorBindingsConfig[descriptorBindingIndex].globalDescriptorOffset + i].GPUImage, 0, 6));
		}
	}

    VkFormat CVulkanRenderer::findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {
        for (VkFormat format : candidates) {
            VkFormatProperties props;
            vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

            if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
                return format;
            } else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
                return format;
            }
        }

        throw std::runtime_error("failed to find supported format!");
    }

    VkFormat CVulkanRenderer::findDepthFormat() {
        return findSupportedFormat(
            {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
            VK_IMAGE_TILING_OPTIMAL,
            VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
            );
    }

    bool CVulkanRenderer::hasStencilComponent(VkFormat format) {
        return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
    }

    void CVulkanRenderer::createTextureImageView() {
		unsigned int readableTextureDescriptorBindingIndex = resources_.descriptorSetsConfig[DescriptorSetDataLink::RIDABLE_TEXTURES].descriptorsBindingsIDs[0];
        for(unsigned int i = 0; i < assets_.initializeTextureData_.size(); ++i) {
			VK_Image* image = resources_.GPUDescriptors[resources_.descriptorBindingsConfig[readableTextureDescriptorBindingIndex].globalDescriptorOffset + i].GPUImage;
			image->views.push_back(createImageView(*image, 0, 1));
		}
    }

    void CVulkanRenderer::createTextureSampler() {
		VkPhysicalDeviceProperties properties{};
		vkGetPhysicalDeviceProperties(physicalDevice, &properties);

		VkSamplerCreateInfo samplerInfo{};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_NEAREST;
		samplerInfo.minFilter = VK_FILTER_NEAREST;
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.anisotropyEnable = VK_TRUE;
		samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

		textureSampler = {};              /// TODO: Is it realy need here?
		if (vkCreateSampler(device, &samplerInfo, nullptr, &textureSampler) != VK_SUCCESS) {
			throw std::runtime_error("failed to create texture sampler!");
		}
    }

    VkImageView CVulkanRenderer::createImageView(VK_Image image, uint32_t baseArrayLayers, uint32_t layerCount) {
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = image.image;
        viewInfo.viewType = image.viewType;
        viewInfo.format = image.format;
		viewInfo.components.r = image.red;
		viewInfo.components.g = image.green;
		viewInfo.components.b = image.blue;
		viewInfo.components.a = image.alpha;
        viewInfo.subresourceRange.aspectMask = image.aspectFlags;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = baseArrayLayers;
        viewInfo.subresourceRange.layerCount = layerCount;

        VkImageView imageView;
        if (vkCreateImageView(device, &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
            throw std::runtime_error("failed to create texture image view!");
        }

        return imageView;
    }

    void CVulkanRenderer::createImage(VK_Image& image) {
        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = image.width;
        imageInfo.extent.height = image.height;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = image.arrayLayers;
        imageInfo.format = image.format;
        imageInfo.tiling = image.tiling;
        imageInfo.usage = image.usageFlags;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.flags = image.createFlags;

        if (vkCreateImage(device, &imageInfo, nullptr, &image.image) != VK_SUCCESS) {
            throw std::runtime_error("failed to create image!");
        }

        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(device, image.image, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, image.memoryPropertyFlags);

        if (vkAllocateMemory(device, &allocInfo, nullptr, &image.deviceMemory) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate image memory!");
        }

        vkBindImageMemory(device, image.image, image.deviceMemory, 0);
    }

    void CVulkanRenderer::transitionImageLayout(VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout) {
        VkCommandBuffer commandBuffer = beginSingleTimeCommands(mainRenderCommandPool);

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

        endSingleTimeCommands(mainRenderCommandPool, commandBuffer);
    }

    void CVulkanRenderer::copyBufferToImage(VkBuffer& buffer, VkImage image, uint32_t width, uint32_t height) {
        VkCommandBuffer commandBuffer = beginSingleTimeCommands(mainRenderCommandPool);

        VkBufferImageCopy region{};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;
        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;
        region.imageOffset = {0, 0, 0};
        region.imageExtent = {
            width,
            height,
            1
        };

        vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

        endSingleTimeCommands(mainRenderCommandPool, commandBuffer);
    }

    void CVulkanRenderer::createVertexBuffer(VkBuffer& _vertexBuffer, VkDeviceMemory& _vertexBufferMemory, core::vector<Vertex>& _vertices) {
        VkDeviceSize bufferSize = sizeof(_vertices[0]) * _vertices.GetSize();

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

        void* data;
        vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, _vertices.GetVectorContainer(), (size_t) bufferSize);
        vkUnmapMemory(device, stagingBufferMemory);

        createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _vertexBuffer, _vertexBufferMemory);

        copyBuffer(stagingBuffer, _vertexBuffer, bufferSize);

        vkDestroyBuffer(device, stagingBuffer, nullptr);
        vkFreeMemory(device, stagingBufferMemory, nullptr);
    }

    void CVulkanRenderer::createIndexBuffer(VkBuffer& _indexBuffer, VkDeviceMemory& _indexBufferMemory, const std::vector<uint32_t>& _indices) {
        VkDeviceSize bufferSize = sizeof(_indices[0]) * _indices.size();

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

        void* data;
        vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, _indices.data(), (size_t) bufferSize);
        vkUnmapMemory(device, stagingBufferMemory);

        createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _indexBuffer, _indexBufferMemory);

        copyBuffer(stagingBuffer, _indexBuffer, bufferSize);

        vkDestroyBuffer(device, stagingBuffer, nullptr);
        vkFreeMemory(device, stagingBufferMemory, nullptr);
    }

	// void CVulkanRenderer::createMemoryArenaBuffers(VkBuffer buffer, VkDeviceMemory deviceMemory, VkDeviceSize,
	// 	) {

	// }

	void CVulkanRenderer::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) {
		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to create buffer!");
        }

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

		i32 result = vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory);
        if (result != VK_SUCCESS) {
			std::cout << "result" << result << std::endl;
            throw std::runtime_error("failed to allocate buffer memory!");
        }

        vkBindBufferMemory(device, buffer, bufferMemory, 0);
    }

    VkCommandBuffer CVulkanRenderer::beginSingleTimeCommands(VkCommandPool& commandPool) {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = commandPool;
        allocInfo.commandBufferCount = 1;

        VkCommandBuffer commandBuffer;
        vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(commandBuffer, &beginInfo);

        return commandBuffer;
    }

    void CVulkanRenderer::endSingleTimeCommands(VkCommandPool& commandPool, VkCommandBuffer& commandBuffer) {
        vkEndCommandBuffer(commandBuffer);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(graphicsQueue);

        vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
    }

    void CVulkanRenderer::copyBuffer(VkBuffer& srcBuffer, VkBuffer& dstBuffer, VkDeviceSize size) {
        VkCommandBuffer commandBuffer = beginSingleTimeCommands(mainRenderCommandPool);

        VkBufferCopy copyRegion{};
        copyRegion.size = size;
        vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

        endSingleTimeCommands(mainRenderCommandPool, commandBuffer);
    }

    uint32_t CVulkanRenderer::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
//            if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties && memProperties.memoryTypes[i].heapIndex == 0) {
			if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
        }

        throw std::runtime_error("failed to find suitable memory type!");
    }

    void CVulkanRenderer::createCommandBuffers(VkCommandPool& commandPool, std::vector<VkCommandBuffer>& commandBuffers,
											   uint32_t commandBuffersNumber, VkCommandBufferLevel commandBufferLevelFlag) {
        commandBuffers.resize(commandBuffersNumber * MAX_FRAMES_IN_FLIGHT);

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = commandPool;
        allocInfo.level = commandBufferLevelFlag;
        allocInfo.commandBufferCount = (uint32_t) commandBuffers.size();

        if (vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate command buffers!");
        }
    }

}
