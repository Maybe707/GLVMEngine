// This file is part of Game Loop Versatile Modules (GLVM)
// Copyright © 2024 Maksim Manokhin a.k.a. Yuriorkis_Scream. Contacts: <fellfrostqtw@gmail.com>
// Author: Maksim Manokhin a.k.a. Yuriorkis_Scream
// License: http://opensource.org/licenses/MIT

#include "Assets/AssetLibrary.hpp"
#include "JsonParser.hpp"
#include "Common/CommonFunctions.hpp"
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
namespace GLVM::core {
void AssetLibrary::requireUnprepared() const {
    if (prepared_) throw std::logic_error("File resources are sealed after preparation");
}
const char* AssetLibrary::rememberPath(const char* path) {
    if (!path || !*path) throw std::invalid_argument("Resource path is empty");
    paths_.emplace_back(path);
    return paths_.back().c_str();
}
void AssetLibrary::prepare() {
    requireUnprepared();
    // Failed imports cannot be retried on partially appended geometry.
    prepared_ = true;
    loadWavefrontObj();
    initializeGLTF();
    initializeFontData();
}
    void AssetLibrary::loadWavefrontObj() {
        for (unsigned int m = 0; m < assets_.pathsArray_.size(); ++m) {
            CWaveFrontObjParser parser;
            CWaveFrontObjParser* wavefrontObjParser = &parser;

            wavefrontObjParser->ReadFile(assets_.pathsArray_[m]);
            wavefrontObjParser->ParseFile();

            assets_.aIndices_.emplace_back();
            assets_.aVertices_.emplace_back();
			assets_.highest_gltf_Y.emplace_back();
			assets_.highest_gltf_Y[m] = -999.999f;

			assets_.frames.Push({});
			assets_.jointMatricesPerMesh.Push({});

            unsigned int vertexIndex  = 0;
            unsigned int textureIndex = 0;
			unsigned int normalIndex  = 0;
            unsigned int faceVerticesSize = wavefrontObjParser->getFaces().GetSize();
			assets_.meshAxisLimitingValues.setToDefaultValues();

            for (unsigned int i = 0; i < faceVerticesSize; ++i)
                for (int j = 0; j < 3; ++j) {
                    vertexIndex     = wavefrontObjParser->getFaces()[i][0][j] - 1;
					assets_.aIndices_[m].push_back(i * 3 + j);
                    SVertex vertex  = wavefrontObjParser->getCoordinateVertices()[vertexIndex];
                    textureIndex    = wavefrontObjParser->getFaces()[i][1][j] - 1;
                    SVertex texture = wavefrontObjParser->getTextureVertices()[textureIndex];
					normalIndex     = wavefrontObjParser->getFaces()[i][2][j] - 1;
					SVertex normal  = wavefrontObjParser->getNormals()[normalIndex];

					vec4 jointIndices;
					vec4 weights;

					if ( vertex[1] > assets_.highest_gltf_Y[m] )
						assets_.highest_gltf_Y[m] = vertex[1];

					if ( vertex[0] < assets_.meshAxisLimitingValues.lowest_x ) {
						assets_.meshAxisLimitingValues.lowest_x = vertex[0];
					} else if ( vertex[0] > assets_.meshAxisLimitingValues.highest_x ) {
						assets_.meshAxisLimitingValues.highest_x = vertex[0];
					}

					if ( vertex[1] < assets_.meshAxisLimitingValues.lowest_y ) {
						assets_.meshAxisLimitingValues.lowest_y = vertex[1];
					} else if ( vertex[1] > assets_.meshAxisLimitingValues.highest_y ) {
						assets_.meshAxisLimitingValues.highest_y = vertex[1];
					}

					if ( vertex[2] < assets_.meshAxisLimitingValues.lowest_z ) {
						assets_.meshAxisLimitingValues.lowest_z = vertex[2];
					} else if ( vertex[2] > assets_.meshAxisLimitingValues.highest_z ) {
						assets_.meshAxisLimitingValues.highest_z = vertex[2];
					}

					jointIndices[0] = -1;
					jointIndices[1] = -1;
					jointIndices[2] = -1;
					jointIndices[3] = -1;

					weights[0] = 1.0f;
					weights[1] = 1.0f;
					weights[2] = 1.0f;
					weights[2] = 1.0f;

                    assets_.aVertices_[m].Push({{vertex[0], vertex[1], vertex[2]},
										{normal[0], normal[1], normal[2]},
										{texture[0], texture[1]},
										{jointIndices[0], jointIndices[1], jointIndices[2]},
										{weights[0], weights[1], weights[2]}});
                }
			setMeshBounds( assets_.meshAxisLimitingValues );
			++wavefrontObjCounter;
        }

    }

	void AssetLibrary::calculateMeshBounds(const vec4& animatedVertex) {
		if ( animatedVertex[0] < assets_.meshAxisLimitingValues.lowest_x ) {
			assets_.meshAxisLimitingValues.lowest_x = animatedVertex[0];
		} else if ( animatedVertex[0] > assets_.meshAxisLimitingValues.highest_x ) {
			assets_.meshAxisLimitingValues.highest_x = animatedVertex[0];
		}

		if ( animatedVertex[1] < assets_.meshAxisLimitingValues.lowest_y ) {
			assets_.meshAxisLimitingValues.lowest_y = animatedVertex[1];
		} else if ( animatedVertex[1] > assets_.meshAxisLimitingValues.highest_y ) {
			assets_.meshAxisLimitingValues.highest_y = animatedVertex[1];
		}

		if ( animatedVertex[2] < assets_.meshAxisLimitingValues.lowest_z ) {
			assets_.meshAxisLimitingValues.lowest_z = animatedVertex[2];
		} else if ( animatedVertex[2] > assets_.meshAxisLimitingValues.highest_z ) {
			assets_.meshAxisLimitingValues.highest_z = animatedVertex[2];
		}
	}

	bool AssetLibrary::isModelCacheExists( const std::string& modelFilePath ) {
		std::filesystem::create_directories("../cache/models");
		std::ofstream modelsCache("../cache/models/cache", std::ios::app);

		if( !modelsCache.is_open() ) {
			std::cerr << "Error opening the models cache file" << std::endl;
			throw std::runtime_error("Failed to load mesh cache");
		}

		std::ifstream file("../cache/models/cache");

		std::string line;
		while (std::getline(file, line)) {
			std::istringstream parsed(line);
			std::string cachedPath;
			float highest_x, lowest_x, highest_y, lowest_y, highest_z, lowest_z;
			if ((parsed >> std::quoted(cachedPath) >> highest_x >> lowest_x >> highest_y >> lowest_y >> highest_z >> lowest_z) && cachedPath == modelFilePath) {
				std::cout << "Model with pafile path: " << modelFilePath << " is already exists in cache" << std::endl;

				assets_.meshAxisLimitingValues.highest_x = highest_x;
				assets_.meshAxisLimitingValues.lowest_x  = lowest_x;
				assets_.meshAxisLimitingValues.highest_y = highest_y;
				assets_.meshAxisLimitingValues.lowest_y  = lowest_y;
				assets_.meshAxisLimitingValues.highest_z = highest_z;
				assets_.meshAxisLimitingValues.lowest_z  = lowest_z;

				isAlreadyCached = true;

				modelsCache.close();
				return true;
			}
		}

		modelsCache.close();
		return false;
	}

	void AssetLibrary::writeModelsCache( const std::string& modelFilePath ) {
		std::ofstream modelsCache("../cache/models/cache", std::ios::app);

		if( !modelsCache.is_open() ) {
			std::cerr << "Error opening the models cache file" << std::endl;
			throw std::runtime_error("Failed to load mesh cache");
		}

		std::size_t pos = modelFilePath.find(' ');
		std::string firstPart = (pos == std::string::npos)
			? modelFilePath
			: modelFilePath.substr(0, pos);

		modelsCache << std::quoted(modelFilePath);
		modelsCache << " " << assets_.meshAxisLimitingValues.highest_x << " " <<
			assets_.meshAxisLimitingValues.lowest_x << " " <<
			assets_.meshAxisLimitingValues.highest_y << " " <<
			assets_.meshAxisLimitingValues.lowest_y << " " <<
			assets_.meshAxisLimitingValues.highest_z << " " <<
			assets_.meshAxisLimitingValues.lowest_z << std::endl;

		modelsCache.close();
	}

	void AssetLibrary::initializeGLTF() {
		core::vector<bool> animationFlags;
		for (unsigned int m = 0; m < assets_.pathsGLTF_.GetSize(); ++m) {
			Core::CJsonParser jsonParser;
			assets_.aVertexesTemp_.emplace_back();
			assets_.aIndices_.emplace_back();
			assets_.frames.Push({});
			assets_.jointMatricesPerMesh.Push({});
			animationFlags.Push({});
			assets_.highest_gltf_Y.emplace_back();
			uint32_t nextIndexGLTF = wavefrontObjCounter + m;
			jsonParser.LoadGLTF(assets_.pathsGLTF_[m], assets_.aVertexesTemp_[m], assets_.aIndices_[nextIndexGLTF],
								assets_.jointMatricesPerMesh[nextIndexGLTF], assets_.frames[nextIndexGLTF],
								animationFlags[m], assets_.highest_gltf_Y[nextIndexGLTF]);
		}

		for (unsigned int m = 0; m < assets_.pathsGLTF_.GetSize(); ++m) {
//            aIndices_.emplace_back();
//            aVertices_.emplace_back();
			assets_.aVertices_.emplace_back();
			assets_.meshAxisLimitingValues.setToDefaultValues();

			isAlreadyCached = false;
			isModelCacheExists( assets_.pathsGLTF_[m] );

			int stepOffset = 0;
			if ( animationFlags[m] )
				stepOffset = 8;
			else
				stepOffset = 16;

			for ( unsigned int n = 0; n < assets_.aVertexesTemp_[m].size(); n += stepOffset ) {
				SVertex vertex;
				vertex[0] = assets_.aVertexesTemp_[m][n];
			    vertex[1] = assets_.aVertexesTemp_[m][n + 1];
				vertex[2] = assets_.aVertexesTemp_[m][n + 2];
				SVertex normal;
				normal[0] = assets_.aVertexesTemp_[m][n + 3];
				normal[1] = assets_.aVertexesTemp_[m][n + 4];
				normal[2] = assets_.aVertexesTemp_[m][n + 5];
				SVertex texture;
				texture[0] = assets_.aVertexesTemp_[m][n + 6];
				texture[1] = assets_.aVertexesTemp_[m][n + 7];

				vec4 joinIndices;
				vec4 weights;
				if ( animationFlags[m] ) {
					joinIndices[0] = -1;
					joinIndices[1] = -1;
					joinIndices[2] = -1;
					joinIndices[3] = -1;

					weights[0] = 1;
					weights[1] = 1;
					weights[2] = 1;
					weights[3] = 1;

				} else {
					joinIndices[0] = assets_.aVertexesTemp_[m][n + 8];
					joinIndices[1] = assets_.aVertexesTemp_[m][n + 9];
					joinIndices[2] = assets_.aVertexesTemp_[m][n + 10];
					joinIndices[3] = assets_.aVertexesTemp_[m][n + 11];

					weights[0] = assets_.aVertexesTemp_[m][n + 12];
					weights[1] = assets_.aVertexesTemp_[m][n + 13];
					weights[2] = assets_.aVertexesTemp_[m][n + 14];
					weights[3] = assets_.aVertexesTemp_[m][n + 15];
				}

				uint32_t nextIndexGLTF = wavefrontObjCounter + m;
				assets_.aVertices_[nextIndexGLTF].Push({{vertex[0], vertex[1], vertex[2]},
										 {normal[0], normal[1], normal[2]},
										 {texture[0], texture[1]},
										 {joinIndices[0], joinIndices[1], joinIndices[2], joinIndices[3]},
										 {weights[0], weights[1], weights[2], weights[3]}});

				if( isAlreadyCached ) {
					continue;
				}

				vec4 animatedVertex = vec4(vertex[0], vertex[1], vertex[2], 1.0);
				if( !animationFlags[m] && assets_.jointMatricesPerMesh[nextIndexGLTF].GetSize() > 0 ) {
					for( unsigned int frame = 0; frame < assets_.jointMatricesPerMesh[nextIndexGLTF][0].GetSize(); ++frame ) {
						mat4 skinMatrix =
							(assets_.jointMatricesPerMesh[nextIndexGLTF][int(joinIndices[0])][frame] * weights[0]) +
							(assets_.jointMatricesPerMesh[nextIndexGLTF][int(joinIndices[1])][frame] * weights[1]) +
							(assets_.jointMatricesPerMesh[nextIndexGLTF][int(joinIndices[2])][frame] * weights[2]) +
							(assets_.jointMatricesPerMesh[nextIndexGLTF][int(joinIndices[3])][frame] * weights[3]);

						animatedVertex = vec4(vertex[0], vertex[1], vertex[2], 1.0) * skinMatrix;
						calculateMeshBounds( animatedVertex );
					}
				} else {
					calculateMeshBounds( animatedVertex );
				}
			}

			if( !isAlreadyCached ) {
				writeModelsCache( assets_.pathsGLTF_[m] );
			}
			setMeshBounds( assets_.meshAxisLimitingValues );
		}
	}

	void AssetLibrary::initializeFontData() {
		static constexpr char glyphs[128]  = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K',
			'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
			'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',
			'p', 'q', 'r', 's', 't', 'u', 'v', 'y', 'x', 'y', 'z', '0', '1', '2', '3',
			'4', '5', '6', '7', '8', '9', '.', ',', '"', '"', '\'', '\'', '"', '"', '\'', '\'',
			'?', '!', '_', '$', '(', ')', '+', '-', '/', ':', ';', '<', '>', '=', '[', ']', '\\'};

		constexpr float fontStep = 1.0 / 12;
		constexpr unsigned int glyph_row = 7;
		constexpr unsigned int glyph_column = 12;



		for ( unsigned int i = 0; i < glyph_row; ++i )
			for ( unsigned int j = 0; j < glyph_column; ++j ) {
				core::vector<Vertex> symbol_g_vertices;
					symbol_g_vertices.Push({{-0.5f, 0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {fontStep * j, fontStep * i + fontStep}, {0.0f, 0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f, 0.0f}});
					symbol_g_vertices.Push({{0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 0.0f}, {fontStep * j + fontStep, fontStep * i + fontStep}, {0.0f, 0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f, 0.0f}});
					symbol_g_vertices.Push({{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 0.0f}, {fontStep * j, fontStep * i}, {0.0f, 0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f, 0.0f}});
					symbol_g_vertices.Push({{0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {fontStep * j + fontStep, fontStep * i}, {0.0f, 0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f, 0.0f}});
				unsigned int currentBufferIndex = i * glyph_column + j;

				bool exitFlag = false;
				const unsigned int nextBufferIndex = static_cast<const unsigned int>(glyphs[currentBufferIndex]);
				for ( unsigned int n = 0; n < assets_.fontIndicesContainer.size(); ++n ) {                 ///< TODO: Fix gabage algo
					if ( nextBufferIndex == assets_.fontIndicesContainer[n] )
						exitFlag = true;
				}

				if ( exitFlag )
					continue;

				assets_.symbolGVerticesContainer.Push(symbol_g_vertices);
				assets_.fontIndicesContainer.push_back(nextBufferIndex);
			}
	}

	ecs::TextureHandle AssetLibrary::LoadTextureFromFile(const char* path_to_texture) {
        requireUnprepared();
		uint32_t textureID = assets_.initializeTextureData_.size();
		ecs::TextureHandle textureHandle;
		textureHandle.id = textureID;
		assets_.initializeTextureData_.push_back({ .path_to_image = rememberPath(path_to_texture) });
		textureHandlers.Push(textureHandle);

		return textureHandle;
	}

	ecs::TextureHandle AssetLibrary::LoadTextureFromAddress(unsigned int iWidth, unsigned int iHeight,
								  unsigned int dat_length, unsigned char* u_iData) {
        requireUnprepared();
		uint32_t textureID = assets_.initializeTextureData_.size();
		ecs::TextureHandle textureHandle;
		textureHandle.id = textureID;
		assets_.initializeTextureData_.push_back({ .iWidth_ = iWidth, .iHeight_ = iHeight, .dat_length_ = dat_length, .u_iData_ = u_iData});
		textureHandlers.Push(textureHandle);

		return textureHandle;
    }

	ecs::components::MeshHandle AssetLibrary::LoadMeshFromFile_OBJ(const char* _pathToMesh) {
        requireUnprepared();
		ecs::components::MeshHandle meshHandle;
		meshHandle.id = meshID;
        if (assets_.pathsGLTF_.GetSize())
            throw std::logic_error("Register OBJ meshes before glTF meshes");
        assets_.pathsArray_.push_back(rememberPath(_pathToMesh));
		meshHandlers.Push(meshHandle);
		++meshID;

		return meshHandle;
    }

	ecs::components::MeshHandle AssetLibrary::LoadMeshFromFile_GLTF(const char* pathToMesh) {
        requireUnprepared();
		ecs::components::MeshHandle meshHandle;
		meshHandle.id = meshID;
        assets_.pathsGLTF_.Push(rememberPath(pathToMesh));
		meshHandlers.Push(meshHandle);
		++meshID;

		return meshHandle;
	}

	ecs::components::MeshHandle AssetLibrary::LoadMesh() {
		ecs::components::MeshHandle meshHandle;
		meshHandle.id = meshID;
		meshHandlers.Push(meshHandle);
		++meshID;

		return meshHandle;
	}

}
