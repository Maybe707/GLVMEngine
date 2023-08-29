		namespace cm = GLVM::ecs::components;
		core::vector<Entity> directionalLightEntities      = componentManager->collectLinkedEntities<cm::transform,
																									 cm::directionalLight,
																									 cm::vertex>();

		namespace cm = GLVM::ecs::components;

		core::vector<Entity> linkedEntities      = componentManager->collectLinkedEntities<cm::transform,
																						   cm::material,
																						   cm::vertex>();
		
		core::vector<Entity> viewPositionLinkedEntities = componentManager->collectLinkedEntities<cm::beholder>();
		cm::transform* playerTransformComponent = componentManager->GetComponent<cm::transform>(viewPositionLinkedEntities[0]);

		for ( unsigned int i = 0; i < linkedEntities.GetSize(); ++i ) {
			//              unsigned int uiEntity = (*entitiesContainerTexture)[j];
			unsigned int uiEntity = linkedEntities[i];
			unsigned int uiVertexId = componentManager->GetComponent<ecs::components::vertex>(uiEntity)->vkVertexId_;
			cm::transform* transformComponent = componentManager->GetComponent<cm::transform>(uiEntity);
			unsigned int diffuseTextureIndex = componentManager->GetComponent<cm::material>(uiEntity)->diffuseTextureID_;
			unsigned int specularTextureIndex = componentManager->GetComponent<cm::material>(uiEntity)->specularTextureID_;
			cm::material* materialComponent = componentManager->GetComponent<cm::material>(uiEntity);

			// VkBufferMemoryBarrier bufferMemoryBarrier{};
			// bufferMemoryBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
			// bufferMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			// bufferMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
			// bufferMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			// bufferMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			// bufferMemoryBarrier.buffer = modelMatrixUniformBuffers[MAX_FRAMES_IN_FLIGHT * i + currentFrame];
			// bufferMemoryBarrier.offset = 0;
			// bufferMemoryBarrier.size   = VK_WHOLE_SIZE;

			// vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
			// 					 0, 0, nullptr, 1, &bufferMemoryBarrier, 0, nullptr);
			/// TODO: Second line work with no MAX_FRAMES_IN_FLIGHT define. Its litle bit wierd. Need to figure out why so.
			unsigned int uboIndex = MAX_FRAMES_IN_FLIGHT * i + currentFrame;
			updateMatrixUniformBuffer(uboIndex, transformComponent);
			vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &matrixUboDescriptorSets[uboIndex], 0, nullptr);
			updateViewPositionUniformBuffer(currentFrame, playerTransformComponent);
			vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 1, 1, &viewPositionUboDescriptorSets[currentFrame], 0, nullptr);
			updateMaterialUniformBuffer(uboIndex, materialComponent);
			vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 2, 1, &materialUboDescriptorSets[uboIndex], 0, nullptr);
			updateDirectionalLightUniformBuffer(currentFrame);
			vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 3, 1, &directionalLightUboDescriptorSets[currentFrame], 0, nullptr);
			updatePointLightUniformBuffer(currentFrame);
			vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 4, 1, &pointLightUboDescriptorSets[currentFrame], 0, nullptr);
			updateSpotLightUniformBuffer(currentFrame);
			vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 5, 1, &spotLightUboDescriptorSets[currentFrame], 0, nullptr);

			
			// unsigned int textureID = componentManager->GetComponent<ecs::components::texture>(uiEntity)->id;
			// std::cout << "texture: " << textureID << std::endl;                
			VkBuffer vertexBuffers[] = {vertexBufferContainer[uiVertexId]};
			VkDeviceSize offsets[] = {0};
			vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

			vkCmdBindIndexBuffer(commandBuffer, indexBufferContainer[uiVertexId], 0, VK_INDEX_TYPE_UINT16);

			unsigned int indicesContainerSize = aVertices_[uiVertexId].size();
			vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 6, 1, &diffuseSamplerDescriptorSets[MAX_FRAMES_IN_FLIGHT * diffuseTextureIndex + currentFrame], 0, nullptr);
			vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 7, 1, &specularSamplerDescriptorSets[MAX_FRAMES_IN_FLIGHT * specularTextureIndex + currentFrame], 0, nullptr);
			vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 8, 1, &shadowMapDirectionalLightDescriptorSets[currentFrame], 0, nullptr);
			vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indicesContainerSize), 1, 0, 0, 0);
		}
		
		// for ( unsigned int i = 0; i < directionalLightEntities.GetSize(); ++i ) {
		// 	unsigned int entity = directionalLightEntities[i];
		// 	unsigned int vertexID = componentManager->GetComponent<ecs::components::vertex>(entity)->vkVertexId_;
		// 	VkBuffer shadowMapVertexBuffers[] = {vertexBufferContainer[vertexID]};
		// 	vkCmdBindVertexBuffers(commandBuffer, 0, 1, shadowMapVertexBuffers, shadowMapOffsets);

		// 	vkCmdBindIndexBuffer(commandBuffer, indexBufferContainer[vertexID], 0, VK_INDEX_TYPE_UINT16);

		// 	cm::transform* transformComponent = componentManager->GetComponent<cm::transform>(entity);
		// 	cm::directionalLight* directionalLightComponent = componentManager->GetComponent<cm::directionalLight>(entity);

		// 	unsigned int indicesContainerSize = aVertices_[vertexID].size();
		// 	updateShadowMapMatrixUniformBuffer(currentFrame, transformComponent, directionalLightComponent);
		// 	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, shadowMapPipelineLayout, 0, 1, &shadowMapMatrixUboDescriptorSets[currentFrame], 0, nullptr);
		// 	vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indicesContainerSize), 1, 0, 0, 0);
		// }

		vkCmdEndRenderPass(commandBuffer);

















#define VK_DEBUG_DESCRIPTOR_SET_RED "\x1b[31mVULKAN DEBUG DESCRIPTOR SET\x1b[0m"
#define VK_DEBUG_DESCRIPTOR_SET_LAYOUT_RED "\x1b[31mVULKAN DEBUG DESCRIPTOR SET LAYOUT\x1b[0m"


	void CVulkanRenderer::setDebugObjectNames() {
		VkDebugUtilsObjectNameInfoEXT mainPipelineObjectInfo{};
		mainPipelineObjectInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
		mainPipelineObjectInfo.pObjectName = "Main pipeline";
		mainPipelineObjectInfo.objectType = VK_OBJECT_TYPE_PIPELINE;
		mainPipelineObjectInfo.objectHandle = (uint64_t)mainRenderScenePipeline.pipeline;
		SetDebugObjectName(device, &mainPipelineObjectInfo);

		VkDebugUtilsObjectNameInfoEXT mainPipelineLayoutObjectInfo{};
		mainPipelineLayoutObjectInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
		mainPipelineLayoutObjectInfo.pObjectName = "Main pipeline layout";
		mainPipelineLayoutObjectInfo.objectType = VK_OBJECT_TYPE_PIPELINE_LAYOUT;
		mainPipelineLayoutObjectInfo.objectHandle = (uint64_t)mainRenderScenePipeline.pipelineLayout;
		SetDebugObjectName(device, &mainPipelineObjectInfo);

		VkDebugUtilsObjectNameInfoEXT directionalLightPipelineObjectInfo{};
		directionalLightPipelineObjectInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
		directionalLightPipelineObjectInfo.pObjectName = "Directional light pipeline";
		directionalLightPipelineObjectInfo.objectType = VK_OBJECT_TYPE_PIPELINE;
		directionalLightPipelineObjectInfo.objectHandle = (uint64_t)directionalLightPipeline.pipeline;
		SetDebugObjectName(device, &directionalLightPipelineObjectInfo);

		VkDebugUtilsObjectNameInfoEXT directionalLightPipelineLayoutObjectInfo{};
		directionalLightPipelineLayoutObjectInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
		directionalLightPipelineLayoutObjectInfo.pObjectName = "Directional light pipeline layout";
		directionalLightPipelineLayoutObjectInfo.objectType = VK_OBJECT_TYPE_PIPELINE_LAYOUT;
		directionalLightPipelineLayoutObjectInfo.objectHandle = (uint64_t)directionalLightPipeline.pipelineLayout;
		SetDebugObjectName(device, &directionalLightPipelineLayoutObjectInfo);

		for ( unsigned long i = 0; i < directionalLightPipeline.descriptors.GetSize(); ++i ) {
			VkDebugUtilsObjectNameInfoEXT descriptorSetLayoutObjectInfo{};
			descriptorSetLayoutObjectInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
			std::string layoutName = ConcatIntBetweenTwoStrings(VK_DEBUG_DESCRIPTOR_SET_LAYOUT_RED, " Directional light shadow map descriptor set layout # ", i);
			const char* strLayoutName = layoutName.c_str();
			descriptorSetLayoutObjectInfo.pObjectName = strLayoutName;
			descriptorSetLayoutObjectInfo.objectType = VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT;
			descriptorSetLayoutObjectInfo.objectHandle = (uint64_t)directionalLightPipeline.descriptors[i].setLayout;
			SetDebugObjectName(device, &descriptorSetLayoutObjectInfo);			
		}

		for ( unsigned long i = 0; i < mainRenderScenePipeline.descriptors.GetSize(); ++i ) {
			VkDebugUtilsObjectNameInfoEXT descriptorSetLayoutObjectInfo{};
			descriptorSetLayoutObjectInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
			std::string layoutName = ConcatIntBetweenTwoStrings(VK_DEBUG_DESCRIPTOR_SET_LAYOUT_RED, " Main render descriptor set layout # ", i);
			const char* strLayoutName = layoutName.c_str();
			descriptorSetLayoutObjectInfo.pObjectName = strLayoutName;
			descriptorSetLayoutObjectInfo.objectType = VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT;
			descriptorSetLayoutObjectInfo.objectHandle = (uint64_t)mainRenderScenePipeline.descriptors[i].setLayout;
			SetDebugObjectName(device, &descriptorSetLayoutObjectInfo);			
		}
		
		for ( unsigned long i = 0; i < shadowMapDirectionalLightDescriptorSets.size(); ++i ) {
			VkDebugUtilsObjectNameInfoEXT descriptorSetObjectInfo{};
			descriptorSetObjectInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
			std::string name = ConcatIntBetweenTwoStrings(VK_DEBUG_DESCRIPTOR_SET_RED, " Shadow map descriptor set # ", i);
			const char* strName = name.c_str();
			descriptorSetObjectInfo.pObjectName = strName;
			descriptorSetObjectInfo.objectType = VK_OBJECT_TYPE_DESCRIPTOR_SET;
			descriptorSetObjectInfo.objectHandle = (uint64_t)shadowMapDirectionalLightDescriptorSets[i];
			SetDebugObjectName(device, &descriptorSetObjectInfo);
		}

 		for ( unsigned long i = 0; i < matrixUboDescriptorSets.size(); ++i ) {
			VkDebugUtilsObjectNameInfoEXT descriptorSetObjectInfo{};
			descriptorSetObjectInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
			std::string name = ConcatIntBetweenTwoStrings(VK_DEBUG_DESCRIPTOR_SET_RED, " Main render model matrix descriptor set # ", i);
			const char* strName = name.c_str();
			descriptorSetObjectInfo.pObjectName = strName;
			descriptorSetObjectInfo.objectType = VK_OBJECT_TYPE_DESCRIPTOR_SET;
			descriptorSetObjectInfo.objectHandle = (uint64_t)matrixUboDescriptorSets[i];
			SetDebugObjectName(device, &descriptorSetObjectInfo);
		}

		for ( unsigned long i = 0; i < viewPositionUboDescriptorSets.size(); ++i ) {
			VkDebugUtilsObjectNameInfoEXT descriptorSetObjectInfo{};
			descriptorSetObjectInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
			std::string name = ConcatIntBetweenTwoStrings(VK_DEBUG_DESCRIPTOR_SET_RED, " Main render view position descriptor set # ", i);
			const char* strName = name.c_str();
			descriptorSetObjectInfo.pObjectName = strName;
			descriptorSetObjectInfo.objectType = VK_OBJECT_TYPE_DESCRIPTOR_SET;
			descriptorSetObjectInfo.objectHandle = (uint64_t)viewPositionUboDescriptorSets[i];
			SetDebugObjectName(device, &descriptorSetObjectInfo);
		}

		for ( unsigned long i = 0; i < materialUboDescriptorSets.size(); ++i ) {
			VkDebugUtilsObjectNameInfoEXT descriptorSetObjectInfo{};
			descriptorSetObjectInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
			std::string name = ConcatIntBetweenTwoStrings(VK_DEBUG_DESCRIPTOR_SET_RED, " Main render material descriptor set # ", i);
			const char* strName = name.c_str();
			descriptorSetObjectInfo.pObjectName = strName;
			descriptorSetObjectInfo.objectType = VK_OBJECT_TYPE_DESCRIPTOR_SET;
			descriptorSetObjectInfo.objectHandle = (uint64_t)materialUboDescriptorSets[i];
			SetDebugObjectName(device, &descriptorSetObjectInfo);
		}

		for ( unsigned long i = 0; i < directionalLightUboDescriptorSets.size(); ++i ) {
			VkDebugUtilsObjectNameInfoEXT descriptorSetObjectInfo{};
			descriptorSetObjectInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
			std::string name = ConcatIntBetweenTwoStrings(VK_DEBUG_DESCRIPTOR_SET_RED, " Main render directional light descriptor set # ", i);
			const char* strName = name.c_str();
			descriptorSetObjectInfo.pObjectName = strName;
			descriptorSetObjectInfo.objectType = VK_OBJECT_TYPE_DESCRIPTOR_SET;
			descriptorSetObjectInfo.objectHandle = (uint64_t)directionalLightUboDescriptorSets[i];
			SetDebugObjectName(device, &descriptorSetObjectInfo);
		}

		for ( unsigned long i = 0; i < pointLightUboDescriptorSets.size(); ++i ) {
			VkDebugUtilsObjectNameInfoEXT descriptorSetObjectInfo{};
			descriptorSetObjectInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
			std::string name = ConcatIntBetweenTwoStrings(VK_DEBUG_DESCRIPTOR_SET_RED, " Main render point light descriptor set # ", i);
			const char* strName = name.c_str();
			descriptorSetObjectInfo.pObjectName = strName;
			descriptorSetObjectInfo.objectType = VK_OBJECT_TYPE_DESCRIPTOR_SET;
			descriptorSetObjectInfo.objectHandle = (uint64_t)pointLightUboDescriptorSets[i];
			SetDebugObjectName(device, &descriptorSetObjectInfo);
		}

		for ( unsigned long i = 0; i < spotLightUboDescriptorSets.size(); ++i ) {
			VkDebugUtilsObjectNameInfoEXT descriptorSetObjectInfo{};
			descriptorSetObjectInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
			std::string name = ConcatIntBetweenTwoStrings(VK_DEBUG_DESCRIPTOR_SET_RED, " Main render spot light descriptor set # ", i);
			const char* strName = name.c_str();
			descriptorSetObjectInfo.pObjectName = strName;
			descriptorSetObjectInfo.objectType = VK_OBJECT_TYPE_DESCRIPTOR_SET;
			descriptorSetObjectInfo.objectHandle = (uint64_t)spotLightUboDescriptorSets[i];
			SetDebugObjectName(device, &descriptorSetObjectInfo);
		}

		for ( unsigned long i = 0; i < diffuseSamplerDescriptorSets.size(); ++i ) {
			VkDebugUtilsObjectNameInfoEXT descriptorSetObjectInfo{};
			descriptorSetObjectInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
			std::string name = ConcatIntBetweenTwoStrings(VK_DEBUG_DESCRIPTOR_SET_RED, " Main render diffuse sampler descriptor set # ", i);
			const char* strName = name.c_str();
			descriptorSetObjectInfo.pObjectName = strName;
			descriptorSetObjectInfo.objectType = VK_OBJECT_TYPE_DESCRIPTOR_SET;
			descriptorSetObjectInfo.objectHandle = (uint64_t)diffuseSamplerDescriptorSets[i];
			SetDebugObjectName(device, &descriptorSetObjectInfo);
		}

 		for ( unsigned long i = 0; i < specularSamplerDescriptorSets.size(); ++i ) {
			VkDebugUtilsObjectNameInfoEXT descriptorSetObjectInfo{};
			descriptorSetObjectInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
			std::string name = ConcatIntBetweenTwoStrings(VK_DEBUG_DESCRIPTOR_SET_RED, " Main render specular sampler descriptor set # ", i);
			const char* strName = name.c_str();
			descriptorSetObjectInfo.pObjectName = strName;
			descriptorSetObjectInfo.objectType = VK_OBJECT_TYPE_DESCRIPTOR_SET;
			descriptorSetObjectInfo.objectHandle = (uint64_t)specularSamplerDescriptorSets[i];
			SetDebugObjectName(device, &descriptorSetObjectInfo);
		}

		for ( unsigned long i = 0; i < shadowMapDirectionalLightDescriptorSets.size(); ++i ) {
			VkDebugUtilsObjectNameInfoEXT descriptorSetObjectInfo{};
			descriptorSetObjectInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
			std::string name = ConcatIntBetweenTwoStrings(VK_DEBUG_DESCRIPTOR_SET_RED, " Main render shadow map directional light descriptor set # ", i);
			const char* strName = name.c_str();
			descriptorSetObjectInfo.pObjectName = strName;
			descriptorSetObjectInfo.objectType = VK_OBJECT_TYPE_DESCRIPTOR_SET;
			descriptorSetObjectInfo.objectHandle = (uint64_t)shadowMapDirectionalLightDescriptorSets[i];
			SetDebugObjectName(device, &descriptorSetObjectInfo);
		}
	}
