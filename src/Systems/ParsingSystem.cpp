#include "Systems/ParsingSystem.hpp"

namespace GLVM::core
{
	void ParsingSystem::LoadGLTF() {
		for (unsigned int m = 0; m < pathsGLTF_.GetSize(); ++m) {
			
			Core::CJsonParser parser;
			parser.ReadFile(pathsGLTF_[m]);
			parser.Parse();

			Core::JsonValue* gltf = parser.GetRoot();
			std::string binary_path = *(*gltf)["buffers"][0]["uri"].value.string;
			int full_byte_size = (*gltf)["buffers"][0]["byteLength"].value.iNumber;;
			std::ifstream in_stream;
			in_stream.open("/home/cyberdemon/cyber_code/GLVMEngine/gltf/" + binary_path, std::ios::binary);
 			char* buffer = new char[full_byte_size];
			in_stream.read(buffer, full_byte_size);
			in_stream.close();

			int indices_index = (*gltf)["meshes"][0]["primitives"][0]["indices"].value.iNumber;
			int indices_buffer_view_index = (*gltf)["accessors"][indices_index]["bufferView"].value.iNumber;
			int indices_byte_length = (*gltf)["bufferViews"][indices_buffer_view_index]["byteLength"].value.iNumber;
			int indices_byte_offset = (*gltf)["bufferViews"][indices_buffer_view_index]["byteOffset"].value.iNumber;

			core::vector<unsigned int> indices;
			for ( int i = indices_byte_offset; i < indices_byte_offset + indices_byte_length; i += 2 )
				indices.Push(reinterpret_cast<unsigned short &>(buffer[i]));

			int vertices_position_index = (*gltf)["meshes"][0]["primitives"][0]["attributes"]["POSITION"].value.iNumber;
			int vertices_buffer_view_index = (*gltf)["accessors"][vertices_position_index]["bufferView"].value.iNumber;
			int vertices_byte_length = (*gltf)["bufferViews"][vertices_buffer_view_index]["byteLength"].value.iNumber;
			int vertices_byte_offset = (*gltf)["bufferViews"][vertices_buffer_view_index]["byteOffset"].value.iNumber;

			core::vector<float> vertices_position;
			for ( int i = vertices_byte_offset; i < vertices_byte_offset + vertices_byte_length; i += 4 )
				vertices_position.Push(reinterpret_cast<float &>(buffer[i]));

			int texture_coordinates_index = (*gltf)["meshes"][0]["primitives"][0]["attributes"]["TEXCOORD_0"].value.iNumber;
			int texture_buffer_view_index = (*gltf)["accessors"][texture_coordinates_index]["bufferView"].value.iNumber;
			int texture_byte_length = (*gltf)["bufferViews"][texture_buffer_view_index]["byteLength"].value.iNumber;
			int texture_byte_offset = (*gltf)["bufferViews"][texture_buffer_view_index]["byteOffset"].value.iNumber;

			core::vector<float> texture_coordinates;
			for ( int i = texture_byte_offset; i < texture_byte_offset + texture_byte_length; i += 4 )
				texture_coordinates.Push(reinterpret_cast<float &>(buffer[i]));

			int normals_index = (*gltf)["meshes"][0]["primitives"][0]["attributes"]["NORMAL"].value.iNumber;
			int normals_buffer_view_index = (*gltf)["accessors"][normals_index]["bufferView"].value.iNumber;
			int normals_byte_length = (*gltf)["bufferViews"][normals_buffer_view_index]["byteLength"].value.iNumber;
			int normals_byte_offset = (*gltf)["bufferViews"][normals_buffer_view_index]["byteOffset"].value.iNumber;

			core::vector<float> normals;
			for ( int i = normals_byte_offset; i < normals_byte_offset + normals_byte_length; i += 4 )
				normals.Push(reinterpret_cast<float &>(buffer[i]));

			core::vector<Core::JsonValue> skins = parser.Search("skins");
			Core::JsonValue joints;
			core::vector<mat4> globalTransformJointNode;
			core::vector<mat4> inverseBindMatrixSet;
			core::vector<core::vector<mat4>> jointMatrices;
			core::vector<float> weightsContainer;
			core::vector<int> jointsIndices;
			
			if ( skins.GetSize() > 0 ) {
				joints = (*gltf)["skins"][0]["joints"];

				Core::JsonValue nodes = (*gltf)["nodes"];
				for ( unsigned int i = 0; i < joints.value.array->GetSize(); ++i ) {
					unsigned int index = (*joints.value.array)[i].value.iNumber;
					Core::JsonValue node = nodes[index];
					Quaternion rotationQuaternion;
					mat4 rotation(1.0f);
					mat4 scale(1.0f);
					mat4 translation(1.0f);

					if ( node.value.object->Contain("rotation") ) {
						Core::JsonValue array = (*node.value.object)["rotation"];
						for ( unsigned int i = 0; i < array.value.array->GetSize(); ++i ) {
							switch(i) {
							case 0:
								if ( array[i].isInterger() )
									rotationQuaternion.x = array[i].value.iNumber;
								else if ( array[i].isFloat() )
									rotationQuaternion.x = array[i].value.fNumber;
							case 1:
								if ( array[i].isInterger() )
									rotationQuaternion.y = array[i].value.iNumber;
								else if ( array[i].isFloat() )
									rotationQuaternion.y = array[i].value.fNumber;
							case 2:
								if ( array[i].isInterger() )
									rotationQuaternion.z = array[i].value.iNumber;
								else if ( array[i].isFloat() )
									rotationQuaternion.z = array[i].value.fNumber;
							case 3:
								if ( array[i].isInterger() )
									rotationQuaternion.w = array[i].value.iNumber;
								else if ( array[i].isFloat() )
									rotationQuaternion.w = array[i].value.fNumber;
							}
						}

						rotation = rotateQuaternion<float, 4>(rotationQuaternion);
					}

					if ( node.value.object->Contain("scale") ) {
						Core::JsonValue array = (*node.value.object)["scale"];
						for ( unsigned int i = 0; i < array.value.array->GetSize(); ++i ) {
							if ( array[i].isInterger() )
								scale[i][i] = array[i].value.iNumber;
							else if ( array[i].isFloat() )
								scale[i][i] = array[i].value.fNumber;
 						}
					}

					if ( node.value.object->Contain("translation") ) {
						Core::JsonValue array = (*node.value.object)["translation"];
						for ( unsigned int i = 0; i < array.value.array->GetSize(); ++i ) {
							if ( array[i].isInterger() )
								translation[3][i] = array[i].value.iNumber;
							else if ( array[i].isFloat() )
								translation[3][i] = array[i].value.fNumber;
						}


					}

					mat4 model = translation * scale * rotation;
					globalTransformJointNode.Push(model);
				}

				unsigned int inverseBindMatricesIndex = (*gltf)["skins"][0]["inverseBindMatrices"].value.iNumber;
				unsigned int bufferView = (*gltf)["accessors"][inverseBindMatricesIndex]["bufferView"].value.iNumber;
				unsigned int byteLengthInverseBindMatrices = (*gltf)["bufferViews"][bufferView]["byteLength"].value.iNumber;
				unsigned int byteOffsetInverseBindMatrices = (*gltf)["bufferViews"][bufferView]["byteOffset"].value.iNumber;

				core::vector<float> inverseBindMatricesData;
				for ( unsigned int i = byteOffsetInverseBindMatrices; i < byteOffsetInverseBindMatrices + byteLengthInverseBindMatrices; i += 4 )
					inverseBindMatricesData.Push(reinterpret_cast<float &>(buffer[i]));

				mat4 inverseBindMatrix(0.0f);
				for ( unsigned int n = 0; n < joints.value.array->GetSize(); ++n ) {
					for ( unsigned int g = 0; g < 4; ++g )
						for ( unsigned int j = 0; j < 4; ++j )
							inverseBindMatrix[g][j] = inverseBindMatricesData[n * 16 + g * 4 + j];

					inverseBindMatrixSet.Push(inverseBindMatrix);
				}

				unsigned int joints_index = (*gltf)["meshes"][0]["primitives"][0]["attributes"]["JOINTS_0"].value.iNumber;
				unsigned int joints_buffer_view_index = (*gltf)["accessors"][joints_index]["bufferView"].value.iNumber;
				unsigned int joints_byte_length = (*gltf)["bufferViews"][joints_buffer_view_index]["byteLength"].value.iNumber;
				unsigned int joints_byte_offset = (*gltf)["bufferViews"][joints_buffer_view_index]["byteOffset"].value.iNumber;

				for ( unsigned int i = joints_byte_offset; i < joints_byte_offset + joints_byte_length; ++i )
					jointsIndices.Push(reinterpret_cast<char &>(buffer[i]));

				unsigned int weights_index = (*gltf)["meshes"][0]["primitives"][0]["attributes"]["WEIGHTS_0"].value.iNumber;
				unsigned int weights_buffer_view_index = (*gltf)["accessors"][weights_index]["bufferView"].value.iNumber;
				unsigned int weights_byte_length = (*gltf)["bufferViews"][weights_buffer_view_index]["byteLength"].value.iNumber;
				unsigned int weights_byte_offset = (*gltf)["bufferViews"][weights_buffer_view_index]["byteOffset"].value.iNumber;

				for ( unsigned int i = weights_byte_offset; i < weights_byte_offset + weights_byte_length; i += 4 )
					weightsContainer.Push(reinterpret_cast<float &>(buffer[i]));

			}

			core::vector<Core::JsonValue> animations = parser.Search("animations");

			if ( animations.GetSize() > 0 ) {
				core::vector<Core::JsonValue> samplerIndices;
				core::vector<Core::JsonValue> targetNodes;
				core::vector<Core::JsonValue> targetPaths;
				Core::JsonValue channels = (*gltf)["animations"][0]["channels"];
				for ( unsigned int i = 0; i < channels.value.array->GetSize(); ++i )
					samplerIndices.Push(channels[i]["sampler"]);

				for ( unsigned int i = 0; i < channels.value.array->GetSize(); ++i )
					targetNodes.Push(channels[i]["target"]["node"]);

				for ( unsigned int i = 0; i < channels.value.array->GetSize(); ++i )
					targetPaths.Push(channels[i]["target"]["path"]);

				core::vector<unsigned int> translationSamplerIndices;
				core::vector<unsigned int> rotationSamplerIndices;
				core::vector<unsigned int> scaleSamplerIndices;
				for ( unsigned int i = 0; i < samplerIndices.GetSize(); ++i ) {
					if ( *targetPaths[i].value.string == "translation" ) {
						translationSamplerIndices.Push(samplerIndices[i].value.iNumber);
					} else if ( *targetPaths[i].value.string == "rotation" ) {
						rotationSamplerIndices.Push(samplerIndices[i].value.iNumber);
					} else if ( *targetPaths[i].value.string == "scale" ) {
						scaleSamplerIndices.Push(samplerIndices[i].value.iNumber);
					}
				}

				Core::JsonValue samplers = (*gltf)["animations"][0]["samplers"];
				
				core::vector<unsigned int> translationInputs;
				core::vector<unsigned int> translationOutputs;
				
				for ( unsigned int i = 0; i < translationSamplerIndices.GetSize(); ++i)
					translationInputs.Push(samplers[translationSamplerIndices[i]]["input"].value.iNumber);

				for ( unsigned int i = 0; i < translationSamplerIndices.GetSize(); ++i)
					translationOutputs.Push(samplers[translationSamplerIndices[i]]["output"].value.iNumber);

				core::vector<core::vector<float>> frameInputsTranslation;
				for ( unsigned int i = 0; i < translationInputs.GetSize(); ++i) {
					unsigned int frameBufferViewIndex =
						(*gltf)["accessors"][translationInputs[i]]["bufferView"].value.iNumber;
					unsigned int frameByteLength      =
						(*gltf)["bufferViews"][frameBufferViewIndex]["byteLength"].value.iNumber;
					unsigned int frameByteOffset      =
						(*gltf)["bufferViews"][frameBufferViewIndex]["byteOffset"].value.iNumber;

					core::vector<float> temp;
					for ( unsigned int i = frameByteOffset; i < frameByteOffset + frameByteLength; i += 4 )
						temp.Push(reinterpret_cast<float &>(buffer[i]));

					frameInputsTranslation.Push(temp);
				}

				core::vector<core::vector<float>> translations;
				for ( unsigned int i = 0; i < translationOutputs.GetSize(); ++i) {
					unsigned int outputBufferViewIndex =
						(*gltf)["accessors"][translationOutputs[i]]["bufferView"].value.iNumber;
					unsigned int outputByteLength      =
						(*gltf)["bufferViews"][outputBufferViewIndex]["byteLength"].value.iNumber;
					unsigned int outputByteOffset      =
						(*gltf)["bufferViews"][outputBufferViewIndex]["byteOffset"].value.iNumber;

					core::vector<float> temp;
					for ( unsigned int i = outputByteOffset; i < outputByteOffset + outputByteLength; i += 4 )
						temp.Push(reinterpret_cast<float &>(buffer[i]));

					translations.Push(temp);
				}

				core::vector<unsigned int> rotationInputs;
				core::vector<unsigned int> rotationOutputs;
				
				for ( unsigned int i = 0; i < rotationSamplerIndices.GetSize(); ++i)
					rotationInputs.Push(samplers[rotationSamplerIndices[i]]["input"].value.iNumber);

				for ( unsigned int i = 0; i < rotationSamplerIndices.GetSize(); ++i)
					rotationOutputs.Push(samplers[rotationSamplerIndices[i]]["output"].value.iNumber);

				core::vector<core::vector<float>> frameInputsRotation;
				for ( unsigned int i = 0; i < rotationInputs.GetSize(); ++i) {
					unsigned int frameBufferViewIndex =
						(*gltf)["accessors"][rotationInputs[i]]["bufferView"].value.iNumber;
					unsigned int frameByteLength      =
						(*gltf)["bufferViews"][frameBufferViewIndex]["byteLength"].value.iNumber;
					unsigned int frameByteOffset      =
						(*gltf)["bufferViews"][frameBufferViewIndex]["byteOffset"].value.iNumber;

					core::vector<float> temp;
					for ( unsigned int i = frameByteOffset; i < frameByteOffset + frameByteLength; i += 4 )
						temp.Push(reinterpret_cast<float &>(buffer[i]));

					frameInputsRotation.Push(temp);
				}

				core::vector<core::vector<float>> rotations;
				for ( unsigned int i = 0; i < rotationOutputs.GetSize(); ++i) {
					unsigned int outputBufferViewIndex =
						(*gltf)["accessors"][rotationOutputs[i]]["bufferView"].value.iNumber;
					unsigned int outputByteLength      =
						(*gltf)["bufferViews"][outputBufferViewIndex]["byteLength"].value.iNumber;
					unsigned int outputByteOffset      =
						(*gltf)["bufferViews"][outputBufferViewIndex]["byteOffset"].value.iNumber;

					core::vector<float> temp;
					for ( unsigned int i = outputByteOffset; i < outputByteOffset + outputByteLength; i += 4 )
						temp.Push(reinterpret_cast<float &>(buffer[i]));

					rotations.Push(temp);
				}

				core::vector<unsigned int> scaleInputs;
				core::vector<unsigned int> scaleOutputs;
				
				for ( unsigned int i = 0; i < scaleSamplerIndices.GetSize(); ++i)
					scaleInputs.Push(samplers[scaleSamplerIndices[i]]["input"].value.iNumber);

				for ( unsigned int i = 0; i < translationSamplerIndices.GetSize(); ++i)
					scaleOutputs.Push(samplers[scaleSamplerIndices[i]]["output"].value.iNumber);

				core::vector<core::vector<float>> frameInputsScale;
				for ( unsigned int i = 0; i < scaleInputs.GetSize(); ++i) {
					unsigned int frameBufferViewIndex =
						(*gltf)["accessors"][scaleInputs[i]]["bufferView"].value.iNumber;
					unsigned int frameByteLength      =
						(*gltf)["bufferViews"][frameBufferViewIndex]["byteLength"].value.iNumber;
					unsigned int frameByteOffset      =
						(*gltf)["bufferViews"][frameBufferViewIndex]["byteOffset"].value.iNumber;

					core::vector<float> temp;
					for ( unsigned int i = frameByteOffset; i < frameByteOffset + frameByteLength; i += 4 )
						temp.Push(reinterpret_cast<float &>(buffer[i]));

					frameInputsScale.Push(temp);
				}

				core::vector<core::vector<float>> scales;
				for ( unsigned int i = 0; i < scaleOutputs.GetSize(); ++i) {
					unsigned int outputBufferViewIndex =
						(*gltf)["accessors"][scaleOutputs[i]]["bufferView"].value.iNumber;
					unsigned int outputByteLength      =
						(*gltf)["bufferViews"][outputBufferViewIndex]["byteLength"].value.iNumber;
					unsigned int outputByteOffset      =
						(*gltf)["bufferViews"][outputBufferViewIndex]["byteOffset"].value.iNumber;

					core::vector<float> temp;
					for ( unsigned int i = outputByteOffset; i < outputByteOffset + outputByteLength; i += 4 )
						temp.Push(reinterpret_cast<float &>(buffer[i]));

					scales.Push(temp);
				}

				frames = frameInputsTranslation[0];
				core::vector<core::vector<mat4>> jointMatricesAccumulator;        ///< Delete this sheet!
				
				for ( unsigned int j = 0; j < translations.GetSize(); ++j ) {
					core::vector<float> boneAllFrameTranslations = translations[j];
					core::vector<float> boneAllFrameRotations    = rotations[j];
					core::vector<float> boneAllFrameScales       = scales[j];
					core::vector<mat4>  globalAllFrameNodeMatrix;
					core::vector<mat4>  globalAllFrameNodeMatrixAccumulator;    ///< Delete this sheet!
					for ( unsigned int i = 0; i < frameInputsTranslation[0].GetSize(); ++i ) {
						mat4 frameTranslation(1.0f);
						mat4 frameScale(1.0f);
						for ( unsigned int q = 0; q < 3; ++q ) {
							frameTranslation[3][q] = boneAllFrameTranslations[i * 3 + q];
							frameScale[q][q]       = boneAllFrameScales[i * 3 + q];
						}

						Quaternion frameRotationQuaternion;
						mat4 frameRotation(1.0f);
						frameRotationQuaternion.x = boneAllFrameRotations[i * 4];
						frameRotationQuaternion.y = boneAllFrameRotations[i * 4 + 1];
						frameRotationQuaternion.z = boneAllFrameRotations[i * 4 + 2];
						frameRotationQuaternion.w = boneAllFrameRotations[i * 4 + 3];

						frameRotation = rotateQuaternion<float, 4>(frameRotationQuaternion);
						frameRotation.SelfTensorTranspose();
						
						mat4 globalTransformNodeMatrix = frameScale * frameRotation * frameTranslation;

						globalAllFrameNodeMatrixAccumulator.Push(globalTransformNodeMatrix);

						mat4 rootTransform(1.0f);
						for ( unsigned int b = 0; b < j; ++b ) {
							rootTransform = jointMatricesAccumulator[b][i] * rootTransform;
						}

						mat4 transformTest(0.0f);
						transformTest[0][0] = 0.0f;
						transformTest[0][1] = -1.0f;
						transformTest[1][0] = 1.0f;
						transformTest[2][2] = 1.0f;
						transformTest[3][3] = 1.0f;

						globalAllFrameNodeMatrix.Push(inverseBindMatrixSet[j] * globalTransformNodeMatrix * rootTransform);
					}

					jointMatricesAccumulator.Push(globalAllFrameNodeMatrixAccumulator);
					jointMatrices.Push(globalAllFrameNodeMatrix);
				}

				int maximumJoints     = 4;
				int unitMatricesSize = maximumJoints - jointMatrices.GetSize();
				
				if ( unitMatricesSize > 0 ) {
					for ( int i = 0; i < unitMatricesSize; ++i) {
						core::vector<mat4>  globalAllFrameNodeMatrix;
						for ( unsigned int j = 0; j < frameInputsTranslation[0].GetSize(); ++j ) {
							mat4 unitMatrix(1.0f);
							globalAllFrameNodeMatrix.Push(unitMatrix);
						}

						jointMatrices.Push(globalAllFrameNodeMatrix);
					}
				}

			} else {
				unsigned int maximumJoints = 4;
				for ( unsigned int i = 0; i < maximumJoints; ++i) {
					core::vector<mat4>  globalAllFrameNodeMatrix;
					for ( unsigned int j = 0; j < 1; ++j ) {      ///< If we dont have animations then we have 1 frame
						mat4 unitMatrix(1.0f);
						globalAllFrameNodeMatrix.Push(unitMatrix);
					}

					jointMatrices.Push(globalAllFrameNodeMatrix);
				}
			}
			jointMatricesPerMesh.Push(jointMatrices);

			aVertexes_.emplace_back();
			aIndices_.emplace_back();

			baseIndices = indices;
			
			for ( unsigned int i = 0; i < indices.GetSize(); ++i ) {
				aIndices_[m].push_back(i);

				unsigned int index = indices[i] * 3;
				if ( index + 2 < vertices_position.GetSize() ) {
					vec3 position = { vertices_position[index],
						vertices_position[index + 1],
						vertices_position[index + 2] };

					aVertexes_[m].push_back(position[0]);
					aVertexes_[m].push_back(position[1]);
					aVertexes_[m].push_back(position[2]);
				}

				if ( index + 2 < normals.GetSize() ) {
					vec3 normal = { normals[index],
						normals[index + 1],
						normals[index + 2] };

					aVertexes_[m].push_back(normal[0]);
					aVertexes_[m].push_back(normal[1]);
					aVertexes_[m].push_back(normal[2]);
				}

				index = indices[i] * 2;
				if ( index + 1 < texture_coordinates.GetSize() ) {
					aVertexes_[m].push_back(texture_coordinates[index]);
					aVertexes_[m].push_back(texture_coordinates[index + 1]);
				}

				index = indices[i] * 4;
				if ( index + 3 < jointsIndices.GetSize() ) {
					aVertexes_[m].push_back(jointsIndices[index]);
					aVertexes_[m].push_back(jointsIndices[index + 1]);
					aVertexes_[m].push_back(jointsIndices[index + 2]);
					aVertexes_[m].push_back(jointsIndices[index + 3]);
				}

				if ( index + 3 < weightsContainer.GetSize() ) {
					aVertexes_[m].push_back(weightsContainer[index]);
					aVertexes_[m].push_back(weightsContainer[index + 1]);
					aVertexes_[m].push_back(weightsContainer[index + 2]);
					aVertexes_[m].push_back(weightsContainer[index + 3]);
				}
			}

			SetVertices(aIndices_[m], aVertexes_[m]);
		}
	}
} // namespace GLVM::core
