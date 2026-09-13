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
    void CVulkanRenderer::createDescriptorSetLayout() {
		for ( int descriptorSetCounter = 0; descriptorSetCounter < DescriptorSetDataLink::DESCRIPTOR_CHUNKS_NUMBER; ++descriptorSetCounter ) {
			DescriptorSet& descriptorSet = resources_.descriptorSetsConfig[descriptorSetCounter];
			std::vector<VkDescriptorSetLayoutBinding> bindings;
			// std::cout << "NEXT DS" << std::endl;
			// std::cout << "binding count: " << descriptorSet.actualLinkedDescriptorBindingsNumber << std::endl;
			for ( u32 j = 0; j < descriptorSet.actualLinkedDescriptorBindingsNumber; ++j ) {
				u32 currentDescriptorBindingID = descriptorSet.descriptorsBindingsIDs[j];
//			u32 currentDescriptorBindingID = j;
//				std::cout << "DS ID: " << currentDescriptorBindingID << std::endl;
				VkDescriptorSetLayoutBinding modelMatrixUboLayout{};
				modelMatrixUboLayout.binding = resources_.descriptorBindingsConfig[currentDescriptorBindingID].binding;
				modelMatrixUboLayout.descriptorCount = resources_.descriptorBindingsConfig[currentDescriptorBindingID].shaderDescriptorsNumber;
				modelMatrixUboLayout.descriptorType = resources_.descriptorBindingsConfig[currentDescriptorBindingID].vkType;
				modelMatrixUboLayout.pImmutableSamplers = nullptr;
				modelMatrixUboLayout.stageFlags = resources_.descriptorBindingsConfig[currentDescriptorBindingID].shaderStageFlag;

				bindings.push_back(modelMatrixUboLayout);
			}

			VkDescriptorSetLayoutCreateInfo layoutInfo{};
			layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			layoutInfo.flags = 0;
			layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
			layoutInfo.pBindings = bindings.data();
//			std::cout << "NUMBER OF BINDINGS: " << static_cast<uint32_t>(bindings.size()) << std::endl;
			if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptorSet.setLayout) != VK_SUCCESS) {
				throw std::runtime_error("failed to create descriptor set layout!");
			}
		}
    }

    void CVulkanRenderer::createMainRenderUniformBuffers() {
		for( unsigned int descriptorSetConfigCounter = 0; descriptorSetConfigCounter <
				 DescriptorSetDataLink::DESCRIPTOR_CHUNKS_NUMBER; ++descriptorSetConfigCounter ) {
			for( unsigned int j = 0; j < resources_.descriptorSetsConfig[descriptorSetConfigCounter].actualLinkedDescriptorBindingsNumber; ++j ) {
				unsigned int descriptorBindingIndex = resources_.descriptorSetsConfig[descriptorSetConfigCounter].descriptorsBindingsIDs[j];
				VkDescriptorType descriptorType = resources_.descriptorBindingsConfig[descriptorBindingIndex].vkType;
				if( descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER ) {
					u32 memory = resources_.descriptorBindingsConfig[descriptorBindingIndex].uboChunkSize * resources_.descriptorSetsConfig[descriptorSetConfigCounter].hostDescriptorNumber;
					// std::cout << "ds binding index: " << descriptorBindingIndex << std::endl;
					// std::cout << "host ds number: " << resources_.descriptorSetsConfig[descriptorBindingIndex].hostDescriptorNumber << std::endl;
					// std::cout << "chunk size: " << resources_.descriptorBindingsConfig[descriptorBindingIndex].uboChunkSize << std::endl;
					// std::cout << "MEMORY: " << memory << std::endl;
					createBuffer(memory, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
								 resources_.GPUDescriptors[resources_.descriptorBindingsConfig[descriptorBindingIndex].globalDescriptorOffset].GPUBuffer->buffer,
								 resources_.GPUDescriptors[resources_.descriptorBindingsConfig[descriptorBindingIndex].globalDescriptorOffset].GPUBuffer->deviceMemory);
				} else {
					continue;
				}
			}
		}
    }

    void CVulkanRenderer::createMainRenderDescriptorPool() {
        std::array<VkDescriptorPoolSize, 2> poolSizes{};

		uint32_t descriptorCount = 65536;
        poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSizes[0].descriptorCount = static_cast<uint32_t>(descriptorCount);
		poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSizes[1].descriptorCount = static_cast<uint32_t>(descriptorCount);

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = static_cast<uint32_t>(descriptorCount);

        if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor pool!");
        }
    }

	void CVulkanRenderer::allocateDescriptorSets( core::vector<VkDescriptorSet>& descriptorSets, VkDescriptorSetLayout setLayout,
												  const unsigned int descriptorSetsNumber, const unsigned int descriptorOffset ) {
		if( descriptorSetsNumber == 0 ) {
			return;
		}

		std::vector<VkDescriptorSetLayout> matrixUboLayouts(descriptorSetsNumber, setLayout);
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = descriptorPool;
		allocInfo.descriptorSetCount = static_cast<uint32_t>(descriptorSetsNumber);
		allocInfo.pSetLayouts = matrixUboLayouts.data();

//		descriptorSets.Resize(descriptorSetsNumber);
		if (vkAllocateDescriptorSets(device, &allocInfo, descriptorSets.GetVectorContainer() + descriptorOffset) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate descriptor sets!");
		}
	}

	void CVulkanRenderer::updateDescriptorSetsUBO( VkBuffer ubo, const VkDeviceSize& uboStructSize, const unsigned int& uboDescriptorsNumber,
												   int uboBinding, [[maybe_unused]] core::vector<VkDescriptorSet>& uboDescriptorSets, const unsigned int offset ) {
		for (size_t i = 0; i < uboDescriptorsNumber; ++i) {
			VkDescriptorBufferInfo modelMatrixBufferInfo = createDescriptorBufferInfo( ubo, uboStructSize, i );
			std::array<VkWriteDescriptorSet, 1> descriptorWrites{};

			descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[0].dstSet = *(resources_.descriptorSetsChunks.GetVectorContainer() + offset + i);
			descriptorWrites[0].dstBinding = uboBinding;
			descriptorWrites[0].dstArrayElement = 0;
			descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrites[0].descriptorCount = 1;
			descriptorWrites[0].pBufferInfo = &modelMatrixBufferInfo;

			vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
		}
	}

	void CVulkanRenderer::updateLightDataDescriptorSets( const DescriptorSet& currentDescriptorSet1 ) {
		const unsigned int linkedDescriptorSetBindingsNumber = currentDescriptorSet1.actualLinkedDescriptorBindingsNumber;
		core::vector<u32> shaderBindings;
		core::vector<u32> descriptorNumberPerBinding;
		core::vector<u32> bindingsIDs;
		for ( size_t j = 0; j < linkedDescriptorSetBindingsNumber; ++j ) {
			shaderBindings.Push( resources_.descriptorBindingsConfig[currentDescriptorSet1.descriptorsBindingsIDs[j]].binding );
			bindingsIDs.Push( currentDescriptorSet1.descriptorsBindingsIDs[j] );
		}

		for ( size_t i = 0; i < currentDescriptorSet1.hostDescriptorNumber; ++i ) {
			core::vector<VkWriteDescriptorSet> descriptorWrites;
			descriptorWrites.Resize( shaderBindings.GetSize() );
			core::vector<VkDescriptorBufferInfo> descriptorBufferInfos;
			core::vector<core::vector<VkDescriptorImageInfo>> descriptorImageInfos;
			for( size_t j = 0; j < shaderBindings.GetSize(); ++j ) {
				if( resources_.descriptorBindingsConfig[bindingsIDs[j]].vkType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER ) {
					descriptorBufferInfos.Push({});

					for( size_t m = 0; m < resources_.descriptorBindingsConfig[bindingsIDs[j]].shaderDescriptorsNumber; ++m ) {
						descriptorBufferInfos[j] = createDescriptorBufferInfo(
							resources_.GPUDescriptors[resources_.descriptorBindingsConfig[bindingsIDs[j]].globalDescriptorOffset].GPUBuffer->buffer,
							resources_.descriptorBindingsConfig[bindingsIDs[j]].uboChunkSize, i );
					}
					descriptorWrites[j].pBufferInfo = descriptorBufferInfos.GetVectorContainer();
				} else if ( resources_.descriptorBindingsConfig[bindingsIDs[j]].vkType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER ) {
					descriptorImageInfos.Push({});

					for( size_t m = 0; m < resources_.descriptorBindingsConfig[bindingsIDs[j]].shaderDescriptorsNumber; ++m ) {
						u32 imageViewIndex = resources_.GPUDescriptors[resources_.descriptorBindingsConfig[bindingsIDs[j]].globalDescriptorOffset + m].GPUImage->views.size() - 1;

						descriptorImageInfos[descriptorImageInfos.GetSize() - 1].Push({});
						descriptorImageInfos[descriptorImageInfos.GetSize() - 1][m] = createDescriptorImageInfo(
							*resources_.GPUDescriptors[resources_.descriptorBindingsConfig[bindingsIDs[j]].globalDescriptorOffset + m].GPUImage,
							VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL, imageViewIndex, textureSampler );
					}
					descriptorWrites[j].pImageInfo = descriptorImageInfos[descriptorImageInfos.GetSize() - 1].GetVectorContainer();
				}
				descriptorWrites[j].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptorWrites[j].dstSet = *(resources_.descriptorSetsChunks.GetVectorContainer() + currentDescriptorSet1.descriptorSetOffset + i);
				descriptorWrites[j].dstBinding = shaderBindings[j];
				descriptorWrites[j].dstArrayElement = 0;
				descriptorWrites[j].descriptorType = resources_.descriptorBindingsConfig[bindingsIDs[j]].vkType;
				descriptorWrites[j].descriptorCount = resources_.descriptorBindingsConfig[bindingsIDs[j]].shaderDescriptorsNumber;
			}

			vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.GetSize()), descriptorWrites.GetVectorContainer(), 0, nullptr);
		}
	}

	void CVulkanRenderer::updateDescriptorSetsCombinedImageSampler( const DescriptorSet& descriptorSet ) {
		core::vector<u32> bindingsIDs;
		for ( size_t j = 0; j < descriptorSet.actualLinkedDescriptorBindingsNumber; ++j ) {
			bindingsIDs.Push( descriptorSet.descriptorsBindingsIDs[j] );
		}

		unsigned int readableTextureDescriptorBindingIndex = resources_.descriptorSetsConfig[DescriptorSetDataLink::RIDABLE_TEXTURES].descriptorsBindingsIDs[0];
		for (size_t i = 0; i < descriptorSet.hostDescriptorNumber; ++i) {
			const unsigned int textureIndex = i / 2;
			constexpr unsigned int textureViewIndex = 0;
			VkDescriptorImageInfo imageInfo = createDescriptorImageInfo( *resources_.GPUDescriptors[resources_.descriptorBindingsConfig[readableTextureDescriptorBindingIndex].globalDescriptorOffset + textureIndex].GPUImage, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, textureViewIndex, textureSampler );
			core::vector<VkWriteDescriptorSet> descriptorWrites{};

			for ( unsigned int j = 0; j < bindingsIDs.GetSize(); ++j ) {
				descriptorWrites.Push({});
				const unsigned int lastElement = descriptorWrites.GetSize() - 1;
				descriptorWrites[lastElement].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptorWrites[lastElement].dstSet = *(resources_.descriptorSetsChunks.GetVectorContainer() + descriptorSet.descriptorSetOffset + i);
				descriptorWrites[lastElement].dstBinding = resources_.descriptorBindingsConfig[bindingsIDs[j]].binding;;
				descriptorWrites[lastElement].dstArrayElement = 0;
				descriptorWrites[lastElement].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				descriptorWrites[lastElement].descriptorCount = 1;
				descriptorWrites[lastElement].pImageInfo = &imageInfo;
			}
			vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.GetSize()), descriptorWrites.GetVectorContainer(), 0, nullptr);
		}
	}

	void CVulkanRenderer::createDescriptorImageInfo( const unsigned int descriptorNumber, VkImageLayout imageLayout,
													 core::vector<VK_Image>& textureImages, const unsigned int imageViewIndex,
													 VkDescriptorImageInfo descriptorImageInfos[] ) {
		for (size_t i = 0; i < descriptorNumber; ++i) {
			descriptorImageInfos[i] = {};
			descriptorImageInfos[i].imageLayout = imageLayout;
			descriptorImageInfos[i].imageView = textureImages[i].views[imageViewIndex];
			descriptorImageInfos[i].sampler = textureSampler;
		}
	}

    void CVulkanRenderer::createMainRenderDescriptorSets() {
		for( unsigned int pipelineCounter = 0; pipelineCounter < SpecificPipeline::PIPELINES_NUMBER; ++pipelineCounter ) {
			for( unsigned int descriptorSetCounter = 0; descriptorSetCounter < resources_.pipelineConfigs[pipelineCounter].actualLinkedDescriptorSetsNumber; ++descriptorSetCounter ) {
				const unsigned int linkedDescriptorSetMatrixUboID = resources_.pipelineConfigs[pipelineCounter].linkedDescriptorSetIDs[descriptorSetCounter];
				const DescriptorSet& currentDescriptorSet0 = resources_.descriptorSetsConfig[linkedDescriptorSetMatrixUboID];
				allocateDescriptorSets( resources_.descriptorSetsChunks, currentDescriptorSet0.setLayout,
										currentDescriptorSet0.hostDescriptorNumber, currentDescriptorSet0.descriptorSetOffset );
				if( currentDescriptorSet0.isTexture ) {
					updateDescriptorSetsCombinedImageSampler( currentDescriptorSet0);
				} else {
					updateLightDataDescriptorSets( currentDescriptorSet0 );
				}
			}
		}
	}

	VkDescriptorBufferInfo CVulkanRenderer::createDescriptorBufferInfo( VkBuffer ubo, const VkDeviceSize& uboStructSize, const VkDeviceSize& offsetStep ) {
		VkDescriptorBufferInfo uboBufferInfo{};
		uboBufferInfo.buffer = ubo;
		uboBufferInfo.offset = offsetStep * uboStructSize;
		uboBufferInfo.range = uboStructSize;

		return uboBufferInfo;
	}

	VkDescriptorImageInfo CVulkanRenderer::createDescriptorImageInfo( const VK_Image& textureImage, VkImageLayout layout, unsigned int textureViewIndex, VkSampler textureSampler ) {
		VkDescriptorImageInfo imageInfo{};
//		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageLayout = layout;
		imageInfo.imageView = textureImage.views[textureViewIndex];
		imageInfo.sampler = textureSampler;

		return imageInfo;
	}

}
