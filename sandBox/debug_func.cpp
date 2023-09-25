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
