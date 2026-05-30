// This file is part of Game Loop Versatile Modules (GLVM)
// Copyright © 2024 Maksim Manokhin a.k.a. Yuriorkis_Scream. Contacts: <fellfrostqtw@gmail.com>
// Author: Maksim Manokhin a.k.a. Yuriorkis_Scream
// License: http://opensource.org/licenses/MIT

#include "JsonParser.hpp"
#include "Vector.hpp"
#include "stack.hpp"
#include <chrono>
#include <cstdint>
#include <ostream>
#include <pthread.h>
#include <cassert>

namespace GLVM::Core
{    

    void CJsonParser::ReadFile(const char* _filePath) {
        const char* _pJsonFilePath = _filePath;
        std::ifstream jsonFileInputStream;
        std::stringstream jsonFileOutputStream;
        
        jsonFileInputStream.open(_pJsonFilePath);
        if(jsonFileInputStream.good()) {

            jsonFileOutputStream << jsonFileInputStream.rdbuf();
            jsonFileInputStream.close();
            sJsonFileData_ = jsonFileOutputStream.str();
        } else {
            std::cout << "Error of reading json file" << std::endl;
            return;
        }

        pJsonFileData_ = sJsonFileData_.c_str();
    }
	
    void CJsonParser::Parse() {
		currentChar_ = pJsonFileData_[globalFileCounter_];
		
		while (currentChar_ != '\0') {
			currentChar_ = pJsonFileData_[globalFileCounter_];

			if (currentChar_ == '"' && keyFlag) {
				lastKey_ = StringParse();
				while (currentChar_ == ' ' || currentChar_ == ':') {
				++globalFileCounter_;
				currentChar_ = pJsonFileData_[globalFileCounter_];
				}
			}
			
			if (currentChar_ == '"') {
				bufferString_ = StringParse();

				if (keyFlag) {
					JsonValue jsonString(bufferString_);
					(*stackOfJsonValues_.GetHead()->value.object)[lastKey_.c_str()] = jsonString;
				} else {
 					JsonValue jsonString(bufferString_);
					stackOfJsonValues_.GetHead()->value.array->Push(jsonString);
				}
			} else if ((currentChar_ >= '0' && currentChar_ <= '9') ||
					   currentChar_ == '+' || currentChar_ == '-') {
				bufferString_ = NumberAsStringParse();
				core::vector<char> vector = StringToVectorOfChars(bufferString_);
				double fNumber = 0.0f;
				int iNumber = 0;
				if (IsContainChar(bufferString_, '.')) {
					fNumber = ParseFloating(vector);

					if (keyFlag) {
						JsonValue jsonFloat(fNumber);
						(*stackOfJsonValues_.GetHead()->value.object)[lastKey_.c_str()] = jsonFloat;
					} else {
						JsonValue jsonFloat(fNumber);
						stackOfJsonValues_.GetHead()->value.array->Push(jsonFloat);
					}
				} else {
 					iNumber = ParseInteger(vector);

					if (keyFlag) {
						JsonValue jsonInt(iNumber);
						(*stackOfJsonValues_.GetHead()->value.object)[lastKey_.c_str()] = jsonInt;
					} else {
						JsonValue jsonInt(iNumber);
						stackOfJsonValues_.GetHead()->value.array->Push(jsonInt);
					}
				}

			} else if (currentChar_ == 't' ||
				       currentChar_ == 'f' ||
				       currentChar_ == 'n') {
				std::string boolOrNullString = BoolOrNullParse();

				if (boolOrNullString == "true") {
					if (keyFlag) {
						JsonValue jsonTrue(true);
						(*stackOfJsonValues_.GetHead()->value.object)[lastKey_.c_str()] = jsonTrue;
					} else {
						JsonValue jsonTrue(true);
						stackOfJsonValues_.GetHead()->value.array->Push(jsonTrue);
					}
				} else if (boolOrNullString == "false") {
					if (keyFlag) {
						JsonValue jsonFalse(false);
						(*stackOfJsonValues_.GetHead()->value.object)[lastKey_.c_str()] = jsonFalse;
					} else {
						JsonValue jsonFalse(false);
						stackOfJsonValues_.GetHead()->value.array->Push(jsonFalse);
					}
				} else if (boolOrNullString == "null") {
					if (keyFlag) {
						JsonValue jsonNull;
						jsonNull.type = JSON_NULL;
						jsonNull.value.null = NULL;
						(*stackOfJsonValues_.GetHead()->value.object)[lastKey_.c_str()] = jsonNull;
					} else {
						JsonValue jsonNull;
						jsonNull.type = JSON_NULL;
						jsonNull.value.null = NULL;
						stackOfJsonValues_.GetHead()->value.array->Push(jsonNull);
					}
				}
			} else if (currentChar_ == '{') {
				if (stackOfJsonValues_.GetSize() == 0) {
					root_ = new JsonValue;
					*root_ = CreateJsonHashMap();
					stackOfJsonValues_.Push(root_);
				} else if (keyFlag) {
					JsonValue jsonObject = CreateJsonHashMap();
					(*stackOfJsonValues_.GetHead()->value.object)[lastKey_.c_str()] = jsonObject;
					stackOfJsonValues_.Push(&(*stackOfJsonValues_.GetHead()->value.object)[lastKey_.c_str()]);
				} else if (!keyFlag) {
					JsonValue jsonObject = CreateJsonHashMap();
					stackOfJsonValues_.GetHead()->value.array->Push(jsonObject);
					stackOfJsonValues_.Push(&stackOfJsonValues_.GetHead()->value.array->GetHead());
				}

				keyFlag = true;
			} else if (currentChar_ == '[') {
				if (stackOfJsonValues_.GetSize() == 0) {
					root_ = new JsonValue;
					*root_ = CreateJsonArray();
					stackOfJsonValues_.Push(root_);
				} else if (keyFlag) {
					JsonValue jsonArray = CreateJsonArray();
					(*stackOfJsonValues_.GetHead()->value.object)[lastKey_.c_str()] = jsonArray;
					stackOfJsonValues_.Push(&(*stackOfJsonValues_.GetHead()->value.object)[lastKey_.c_str()]);
				} else if (!keyFlag) {
					JsonValue jsonArray = CreateJsonArray();
					stackOfJsonValues_.GetHead()->value.array->Push(jsonArray);
					stackOfJsonValues_.Push(&stackOfJsonValues_.GetHead()->value.array->GetHead());
				}

				keyFlag = false;
			} else if (currentChar_ == '}') {
				stackOfJsonValues_.Pop();
				if (stackOfJsonValues_.GetSize() && stackOfJsonValues_.GetHead()->type == JSON_OBJECT)
					keyFlag = true;
				else
					keyFlag = false;

			} else if (currentChar_ == ']') {
				stackOfJsonValues_.Pop();
				if (stackOfJsonValues_.GetSize() && stackOfJsonValues_.GetHead()->type == JSON_OBJECT)
					keyFlag = true;
				else
					keyFlag = false;

			} 
			
			++globalFileCounter_;
		}
	}

	JsonValue CJsonParser::CreateJsonHashMap() {
		JsonValue jsonObject;
		jsonObject.type = JSON_OBJECT;
		jsonObject.value.object = new HashMap<JsonValue>;
		return jsonObject;
	}

	JsonValue CJsonParser::CreateJsonArray() {
		JsonValue jsonArray;
		jsonArray.type = JSON_ARRAY;
		jsonArray.value.array = new core::vector<JsonValue>;
		return jsonArray;
	}
	
	std::string CJsonParser::BoolOrNullParse() {
		std::string boolOrNullString = "";
		while (1) {
			currentChar_ = pJsonFileData_[globalFileCounter_];
			if (currentChar_ >= 'a' && currentChar_ <= 'z') {
				boolOrNullString.push_back(currentChar_);
				++globalFileCounter_;
			} else
				return boolOrNullString;
		}
	}
	
	bool CJsonParser::IsContainChar(std::string _string, char _char) {
		for (unsigned int i = 0; i < _string.size(); ++i) {
			if (_string[i] == _char)
				return true;
		}

		return false;
	}
			
	std::string CJsonParser::NumberAsStringParse() {
		std::string numberAsString = "";
		while (1) {
			currentChar_ = pJsonFileData_[globalFileCounter_];
			if ((currentChar_ >= '0' && currentChar_ <= '9') ||
				currentChar_ == '+' || currentChar_ == '-' ||
				currentChar_ == 'e') {
				numberAsString.push_back(currentChar_);
				++globalFileCounter_;
			} else if (currentChar_ == '.') {
				numberAsString.push_back(currentChar_);
				++globalFileCounter_;
			} else
				return numberAsString;
		}
	}
		
 	std::string CJsonParser::StringParse() {
		++globalFileCounter_;
		std::string localBuffer = "";
		while (1) {
			currentChar_ = pJsonFileData_[globalFileCounter_];
			if (currentChar_ == '"') {
				++globalFileCounter_;
				currentChar_ = pJsonFileData_[globalFileCounter_];
				return localBuffer;
			} else {
				localBuffer.push_back(currentChar_);
				++globalFileCounter_;
			}
		}
	}
	
	core::vector<char> CJsonParser::StringToVectorOfChars(std::string _string) {
		core::vector<char> vectorWithChars;
		for (unsigned int i = 0; i < _string.size(); ++i) {
			vectorWithChars.Push(_string[i]);
		}

		return vectorWithChars;
	}
	
	int CJsonParser::ParseInteger(core::vector<char> _word) {
		core::vector<int> baseContainer;

        for (unsigned int i = 0; i < _word.GetSize(); ++i)
            baseContainer.Push(_word[i] - 48);

        int iResult = 0;
        bool negateFlag = false;

        unsigned int baseContainerSize = baseContainer.GetSize();
        for (unsigned int i = 0; i < baseContainerSize; ++i) {
            if (baseContainer[i] == -3 && i == 0) {
				negateFlag = true;
                continue;
            } else if (baseContainer[i] == -5 && i == 0)
                continue;

            iResult += baseContainer[i] * std::pow(10, (baseContainerSize - 1) - i);
        }

		if (negateFlag)
			iResult *= -1;
		
        return iResult;
    }
    
    double CJsonParser::ParseFloating(core::vector<char> _word) {
		core::vector<int> baseContainer;

        for (unsigned int i = 0; i < _word.GetSize(); ++i)
            baseContainer.Push(_word[i] - 48);

        int integerPart = 0;
        double floatingPart = 0;
		int eNumber = 0;
		core::vector<int> integerPartContainer;
		core::vector<int> floatingPartContainer;
		core::vector<int> ePartContainer;
        bool dotFlag = false;
        bool negateFlag = false;
		bool eFlag = false;
		bool eSign = false;                    ///< false value equal "+" sign;
        unsigned int baseContainerSize = baseContainer.GetSize();

        if (baseContainer[0] == -3)
            negateFlag = true;

        for (unsigned int i = 0; i < baseContainerSize; ++i) {
            if (negateFlag && i == 0)
                continue;
            else if (baseContainer[i] == -5 && i == 0)
                continue;
            else if (baseContainer[i] == -2) {
                dotFlag = true;
                continue;
            } else if (baseContainer[i] == 53) {
				eFlag = true;
				continue;
			}

			if (eFlag) {
				if (baseContainer[i] == -5)
					continue;
				else if (baseContainer[i] == -3) {
					eSign = true;
					continue;
				}

				ePartContainer.Push(baseContainer[i]);
				continue;
			}
			
            if (baseContainer[i] >= 0 && baseContainer[i] <= 9) {
                if (dotFlag)
                    floatingPartContainer.Push(baseContainer[i]);
                else
                    integerPartContainer.Push(baseContainer[i]);
            } else {
                std::cout << "Element is not a number" << std::endl;
                return NAN;
            }
        }

        unsigned int ePartContainerSize = ePartContainer.GetSize();
        for (unsigned int i = 0; i < ePartContainerSize; ++i)
            eNumber += ePartContainer[i] * std::pow(10, (ePartContainerSize - 1) - i);
		
        unsigned int integerPartContainerSize = integerPartContainer.GetSize();
        for (unsigned int i = 0; i < integerPartContainerSize; ++i)
            integerPart += integerPartContainer[i] * std::pow(10, (integerPartContainerSize - 1) - i);

        unsigned int floatingPartContainerSize = floatingPartContainer.GetSize();
        for (unsigned int i = 0; i < floatingPartContainerSize; ++i)
            floatingPart += floatingPartContainer[i] / std::pow(10, i + 1);

        double result = 0;
        result = (double)(integerPart + floatingPart);

		if (eFlag) {
			if (eSign)
				result /= std::pow(10, eNumber);
			else
				result *= std::pow(10, eNumber);
		}
		
        if (negateFlag)
            result *= -1.0f;

        return result;
    }

	void CJsonParser::SearchInJsonArray(core::vector<JsonValue>* arrayValue, const char* key_,
										core::vector<JsonValue>& resultVector) const {
		for ( unsigned int i = 0; i < arrayValue->GetSize(); ++i ) {
			if ( (*arrayValue)[i].type == JSON_OBJECT )
				SearchInJsonObject((*arrayValue)[i].value.object, key_, resultVector);

			if ( (*arrayValue)[i].type == JSON_ARRAY )
				SearchInJsonArray((*arrayValue)[i].value.array, key_, resultVector);
		}
	}
	
	void CJsonParser::SearchInJsonObject(HashMap<JsonValue>* mapValue, const char* key_,
										 core::vector<JsonValue>& resultVector) const {
		for ( unsigned int i = 0; i < mapValue->GetCapacity(); ++i ) {
			if ( mapValue->hashMap_[i] != nullptr ) {
				Node<JsonValue>* current = mapValue->hashMap_[i];
				while ( current != nullptr ) {
					std::string searchKey = key_;
					std::string currentKey = current->key_;
					if ( currentKey == searchKey ) {
						resultVector.Push(current->value_);
					}

					if ( current->value_.type == JSON_OBJECT )
						SearchInJsonObject(current->value_.value.object, key_, resultVector);

					if ( current->value_.type == JSON_ARRAY )
						SearchInJsonArray(current->value_.value.array, key_, resultVector);

					current = current->next_;
				}
			}
		}
	}		
	
	core::vector<JsonValue> CJsonParser::Search(const char* key_) const {
		core::vector<JsonValue> resultVector;
		SearchInJsonObject(root_->value.object, key_, resultVector);
		return resultVector;
	}

	void calculateElementsMemorySize(
		unsigned int indices_elements_count,
		std::string* indices_element_type,
		unsigned int indices_componet_type,
		unsigned int* indices_buffer_view_byte_length ) {
		if( *indices_element_type == "VEC2" ) {
			if( indices_componet_type == 5120 || indices_componet_type == 5121 ) {
				*indices_buffer_view_byte_length = indices_elements_count * 2;
			} else if( indices_componet_type == 5122 || indices_componet_type == 5123 ) {
				*indices_buffer_view_byte_length = indices_elements_count * 4;
			} else if( indices_componet_type == 5125 || indices_componet_type == 5126 ) {
				*indices_buffer_view_byte_length = indices_elements_count * 8;
			}
		} else if( *indices_element_type == "VEC3" ) {
			if( indices_componet_type == 5120 || indices_componet_type == 5121 ) {
				*indices_buffer_view_byte_length = indices_elements_count * 3;
			} else if( indices_componet_type == 5122 || indices_componet_type == 5123 ) {
				*indices_buffer_view_byte_length = indices_elements_count * 6;
			} else if( indices_componet_type == 5125 || indices_componet_type == 5126 ) {
				*indices_buffer_view_byte_length = indices_elements_count * 12;
			}
		} else if( *indices_element_type == "VEC4" ) {
			if( indices_componet_type == 5120 || indices_componet_type == 5121 ) {
				*indices_buffer_view_byte_length = indices_elements_count * 4;
			} else if( indices_componet_type == 5122 || indices_componet_type == 5123 ) {
				*indices_buffer_view_byte_length = indices_elements_count * 8;
			} else if( indices_componet_type == 5125 || indices_componet_type == 5126 ) {
				*indices_buffer_view_byte_length = indices_elements_count * 16;
			}
		} else if( *indices_element_type == "SCALAR" ) {
			if( indices_componet_type == 5120 || indices_componet_type == 5121 ) {
				*indices_buffer_view_byte_length = indices_elements_count;
			} else if( indices_componet_type == 5122 || indices_componet_type == 5123 ) {
				*indices_buffer_view_byte_length = indices_elements_count * 2;
			} else if( indices_componet_type == 5125 || indices_componet_type == 5126 ) {
				*indices_buffer_view_byte_length = indices_elements_count * 4;
			}
		} else if( *indices_element_type == "MAT4" ) {
			*indices_buffer_view_byte_length = indices_elements_count * 64;
		}
	}

	void calculateByteStep( unsigned int indices_componet_type, unsigned int* byte_step ) {
		if( indices_componet_type == 5120 || indices_componet_type == 5121 ) {
			*byte_step = 1;
		} else if( indices_componet_type == 5122 || indices_componet_type == 5123 ) {
			*byte_step = 2;
		} else if( indices_componet_type == 5125 || indices_componet_type == 5126 ) {
			*byte_step = 4;
		}
	}
	
	void CJsonParser::LoadGLTF(const char* pathsGLTF_,
							   std::vector<float>& aVertexes_,
							   std::vector<uint32_t>& aIndices_,
							   core::vector<core::vector<mat4>>& jointMatricesPerMesh,
							   core::vector<float>& frames,
							   bool& noAnimations,
							   float& topY) {
		std::cout << "path: " << pathsGLTF_ << std::endl;
		ReadFile(pathsGLTF_);
		Parse();
		
		Core::JsonValue* gltf = GetRoot();
		std::string binary_path = *(*gltf)["buffers"][0]["uri"].value.string;
		int full_byte_size = (*gltf)["buffers"][0]["byteLength"].value.iNumber;;
		std::ifstream in_stream;
		in_stream.open("../gltf/" + binary_path, std::ios::binary);
		char* buffer = new char[full_byte_size];
		in_stream.read(buffer, full_byte_size);
		in_stream.close();

		int indices_index = (*gltf)["meshes"][0]["primitives"][0]["indices"].value.iNumber;
		int indices_buffer_view_index = (*gltf)["accessors"][indices_index]["bufferView"].value.iNumber;

		unsigned int indices_elements_count = (*gltf)["accessors"][indices_index]["count"].value.iNumber;
		std::string* indices_element_type   = (*gltf)["accessors"][indices_index]["type"].value.string;
		unsigned int indices_componet_type  = (*gltf)["accessors"][indices_index]["componentType"].value.iNumber;
//		std::cout << "indices component type: " << indices_componet_type << std::endl;
//		std::cout << "indices elements count: " << indices_elements_count << std::endl;
		unsigned int indices_buffer_view_byte_length = 0;
		unsigned int indices_byte_step = 0;
		calculateElementsMemorySize( indices_elements_count, indices_element_type, indices_componet_type, &indices_buffer_view_byte_length );
		calculateByteStep( indices_componet_type, &indices_byte_step );
		
		[[maybe_unused]] int indices_buffer_view_byte_offset = 0;
		if( (*gltf)["accessors"][indices_index].isObject() == JSON_OBJECT ) {
			HashMap<JsonValue>* ptr = (*gltf)["accessors"][indices_index].value.object;
			if( ptr->Contain("byteOffset") ) {
				indices_buffer_view_byte_offset = (*gltf)["accessors"][indices_index]["byteOffset"].value.iNumber;
			}
		}
 
		[[maybe_unused]] unsigned int indices_byte_length = (*gltf)["bufferViews"][indices_buffer_view_index]["byteLength"].value.iNumber;
		unsigned int indices_byte_offset = (*gltf)["bufferViews"][indices_buffer_view_index]["byteOffset"].value.iNumber;

		core::vector<unsigned int> indices;
//		std::cout << "indices buffer view byte offset: " << indices_buffer_view_byte_offset << std::endl;
//		std::cout << "indices buffer view byte length: " << indices_buffer_view_byte_length << std::endl;
		std::string str = pathsGLTF_;
		for ( unsigned int i = indices_byte_offset + indices_buffer_view_byte_offset; i < indices_byte_offset + indices_buffer_view_byte_offset + indices_buffer_view_byte_length; i += indices_byte_step ) {
			if( indices_componet_type == 5123 ) {
				indices.Push(reinterpret_cast<unsigned short &>(buffer[i]));
			} else if( indices_componet_type == 5125 ) {
				indices.Push(reinterpret_cast<unsigned int &>(buffer[i]));
			}
		}

		// for ( unsigned int i = indices_byte_offset; i < indices_byte_offset + indices_byte_length; i += 2 )
		// 	indices.Push(reinterpret_cast<unsigned short &>(buffer[i]));
		
		int vertices_position_index = (*gltf)["meshes"][0]["primitives"][0]["attributes"]["POSITION"].value.iNumber;
		int vertices_buffer_view_index = (*gltf)["accessors"][vertices_position_index]["bufferView"].value.iNumber;

		unsigned int vertices_elements_count = (*gltf)["accessors"][vertices_position_index]["count"].value.iNumber;
		std::string* vertices_element_type   = (*gltf)["accessors"][vertices_position_index]["type"].value.string;
		unsigned int vertices_componet_type  = (*gltf)["accessors"][vertices_position_index]["componentType"].value.iNumber;
//		std::cout << "vertices component type: " << vertices_componet_type << std::endl;
//		std::cout << "vertices elements count: " << vertices_elements_count << std::endl;
		unsigned int vertices_buffer_view_byte_length = 0;
		unsigned int vertices_byte_step = 0;
		calculateElementsMemorySize( vertices_elements_count, vertices_element_type, vertices_componet_type, &vertices_buffer_view_byte_length );
		calculateByteStep( vertices_componet_type, &vertices_byte_step );

		[[maybe_unused]] int vertices_buffer_view_byte_offset = 0;
		if( (*gltf)["accessors"][vertices_position_index].isObject() == JSON_OBJECT ) {
			HashMap<JsonValue>* ptr = (*gltf)["accessors"][vertices_position_index].value.object;
			if( ptr->Contain("byteOffset") ) {
				vertices_buffer_view_byte_offset = (*gltf)["accessors"][vertices_position_index]["byteOffset"].value.iNumber;
			}
		}

		[[maybe_unused]] int vertices_byte_length = (*gltf)["bufferViews"][vertices_buffer_view_index]["byteLength"].value.iNumber;
		int vertices_byte_offset = (*gltf)["bufferViews"][vertices_buffer_view_index]["byteOffset"].value.iNumber;

		core::vector<float> vertices_position;
		for ( unsigned int i = vertices_byte_offset + vertices_buffer_view_byte_offset; i < vertices_byte_offset + vertices_buffer_view_byte_offset + vertices_buffer_view_byte_length; i += vertices_byte_step ) {
//			std::cout << "value: " << reinterpret_cast<float&>(buffer[i]) << std::endl;
//			printf("%02x ", (unsigned char)buffer[i]);
			vertices_position.Push(reinterpret_cast<float &>(buffer[i]));
			// if( i % 3 == 0 ) {
			// 	std::cout << "next vertex" << std::endl;
			// }
		}

// 		str = pathsGLTF_;
// 		if( str == "../gltf/scene.gltf" ) {
// 			std::cout << "verices position index: " << vertices_position_index << std::endl;
// 			std::cout << "verices buffer view index: " << vertices_buffer_view_index << std::endl;
// 			std::cout << "vertices byte offset: " << vertices_byte_offset << std::endl;
// 			std::cout << "vertices buffer view byte offset: " << vertices_buffer_view_byte_offset << std::endl;
// 			std::cout << "vertices buffer view byte length: " << vertices_buffer_view_byte_length << std::endl;

// //			throw("asdf");
// 		}
		
		int texture_coordinates_index = (*gltf)["meshes"][0]["primitives"][0]["attributes"]["TEXCOORD_0"].value.iNumber;
		int texture_buffer_view_index = (*gltf)["accessors"][texture_coordinates_index]["bufferView"].value.iNumber;

		unsigned int texture_elements_count = (*gltf)["accessors"][texture_coordinates_index]["count"].value.iNumber;
		std::string* texture_element_type   = (*gltf)["accessors"][texture_coordinates_index]["type"].value.string;
		unsigned int texture_componet_type  = (*gltf)["accessors"][texture_coordinates_index]["componentType"].value.iNumber;
//		std::cout << "texture component type: " << texture_componet_type << std::endl;
//		std::cout << "texture elements count: " << texture_elements_count << std::endl;
		unsigned int texture_buffer_view_byte_length = 0;
		unsigned int texture_byte_step = 0;
		calculateElementsMemorySize( texture_elements_count, texture_element_type, texture_componet_type, &texture_buffer_view_byte_length );
		calculateByteStep( texture_componet_type, &texture_byte_step );
		
		[[maybe_unused]] int texture_buffer_view_byte_offset = 0;
		if( (*gltf)["accessors"][texture_coordinates_index].isObject() == JSON_OBJECT ) {
			HashMap<JsonValue>* ptr = (*gltf)["accessors"][texture_coordinates_index].value.object;
			if( ptr->Contain("byteOffset") ) {
				texture_buffer_view_byte_offset = (*gltf)["accessors"][texture_coordinates_index]["byteOffset"].value.iNumber;
			}
		}

		[[maybe_unused]] int texture_byte_length = (*gltf)["bufferViews"][texture_buffer_view_index]["byteLength"].value.iNumber;
		int texture_byte_offset = (*gltf)["bufferViews"][texture_buffer_view_index]["byteOffset"].value.iNumber;

		core::vector<float> texture_coordinates;
		for ( unsigned int i = texture_byte_offset + texture_buffer_view_byte_offset; i < texture_byte_offset + texture_buffer_view_byte_offset + texture_buffer_view_byte_length; i += texture_byte_step )
			texture_coordinates.Push(reinterpret_cast<float &>(buffer[i]));

		int normals_index = (*gltf)["meshes"][0]["primitives"][0]["attributes"]["NORMAL"].value.iNumber;
		int normals_buffer_view_index = (*gltf)["accessors"][normals_index]["bufferView"].value.iNumber;

		unsigned int normals_elements_count = (*gltf)["accessors"][normals_index]["count"].value.iNumber;
		std::string* normals_element_type   = (*gltf)["accessors"][normals_index]["type"].value.string;
		unsigned int normals_componet_type  = (*gltf)["accessors"][normals_index]["componentType"].value.iNumber;
//		std::cout << "normals component type: " << normals_componet_type << std::endl;
//		std::cout << "normals elements count: " << normals_elements_count << std::endl;
		unsigned int normals_buffer_view_byte_length = 0;
		unsigned int normals_byte_step = 0;
		calculateElementsMemorySize( normals_elements_count, normals_element_type, normals_componet_type, &normals_buffer_view_byte_length );
		calculateByteStep( normals_componet_type, &normals_byte_step );

		[[maybe_unused]] int normals_buffer_view_byte_offset = 0;
		if( (*gltf)["accessors"][normals_index].isObject() == JSON_OBJECT ) {
			HashMap<JsonValue>* ptr = (*gltf)["accessors"][normals_index].value.object;
			if( ptr->Contain("byteOffset") ) {
				normals_buffer_view_byte_offset = (*gltf)["accessors"][normals_index]["byteOffset"].value.iNumber;
			}
		}

		[[maybe_unused]] int normals_byte_length = (*gltf)["bufferViews"][normals_buffer_view_index]["byteLength"].value.iNumber;
		int normals_byte_offset = (*gltf)["bufferViews"][normals_buffer_view_index]["byteOffset"].value.iNumber;

		core::vector<float> normals;
		for ( unsigned int i = normals_byte_offset + normals_buffer_view_byte_offset; i < normals_byte_offset + normals_buffer_view_byte_offset + normals_buffer_view_byte_length; i += normals_byte_step ) {
//			std::cout << reinterpret_cast<float&>(buffer[i]) << std::endl;
			normals.Push(reinterpret_cast<float &>(buffer[i]));
			// if( i % 3 == 0 ) {
			// 	std::cout << "next normal" << std::endl;
			// }
		}

// 		if( str == "../gltf/robot0.gltf" ) {
// 			std::cout << "normals position index: " << normals_index << std::endl;
// 			std::cout << "normals buffer view index: " << normals_buffer_view_index << std::endl;
// 			std::cout << "normals byte offset: " << normals_byte_offset << std::endl;
// 			std::cout << "normals buffer view byte offset: " << normals_buffer_view_byte_offset << std::endl;
// 			std::cout << "normals buffer view byte length: " << normals_buffer_view_byte_length << std::endl;
		
// //			throw("asdf");
// 		}
		
		core::vector<Core::JsonValue> skins = Search("skins");
		Core::JsonValue joints;
		core::vector<mat4> globalTransformJointNode;
		core::vector<mat4> inverseBindMatrixSet;
		core::vector<core::vector<mat4>> jointMatrices;
		core::vector<float> weightsContainer;
		core::vector<int> jointsIndices;
		core::vector<core::vector<int>> children;
			
		if ( skins.GetSize() > 0 ) {
			noAnimations = false;
			joints = (*gltf)["skins"][0]["joints"];

			Core::JsonValue nodes = (*gltf)["nodes"];
//			for ( unsigned int i = 0; i < joints.value.array->GetSize(); ++i ) {                  ///< Loop on joints
			for ( unsigned int i = 0; i < nodes.value.array->GetSize(); ++i ) {                  ///< Loop on nodes
//				unsigned int jointIndexMapToNode = (*joints.value.array)[i].value.iNumber;
//				unsigned int jointIndexMapToNode = (*nodes.value.array)[i].value.iNumber;
//				Core::JsonValue node = nodes[jointIndexMapToNode];
				Core::JsonValue node = nodes[i];
//				std::cout << "NODE: " << i << std::endl;
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
							break;
						case 1:
							if ( array[i].isInterger() )
								rotationQuaternion.y = array[i].value.iNumber;
							else if ( array[i].isFloat() )
								rotationQuaternion.y = array[i].value.fNumber;
							break;
						case 2:
							if ( array[i].isInterger() )
								rotationQuaternion.z = array[i].value.iNumber;
							else if ( array[i].isFloat() )
								rotationQuaternion.z = array[i].value.fNumber;
							break;
						case 3:
							if ( array[i].isInterger() )
								rotationQuaternion.w = array[i].value.iNumber;
							else if ( array[i].isFloat() )
								rotationQuaternion.w = array[i].value.fNumber;
							break;
						}
					}

					rotation = rotateQuaternion<float, 4>(rotationQuaternion);
					rotation.SelfTensorTranspose();
				}

				core::vector<int> local_children;
				if ( node.value.object->Contain("children") ) {                             ///< Coollect children indices
					Core::JsonValue array = (*node.value.object)["children"];
					for ( unsigned int i = 0; i < array.value.array->GetSize(); ++i ) {
						local_children.Push(array[i].value.iNumber);
					}

					children.Push(local_children);                                          ///< Linearly put all children to every root joint
				}
				else {
					core::vector<int> emptyChildren;
//					emptyChildren.Push(-1);
					children.Push(emptyChildren);                                           ///< Put emptry pack of children if can find a one
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
				
				mat4 model = translation * scale * rotation;                                 ///< Compute model matrix
				globalTransformJointNode.Push(model);
			}

			unsigned int inverseBindMatricesIndex = (*gltf)["skins"][0]["inverseBindMatrices"].value.iNumber;      ///< Get the inverse bind matrices accessor index
			unsigned int bufferView = (*gltf)["accessors"][inverseBindMatricesIndex]["bufferView"].value.iNumber;

			unsigned int inverse_bind_matrices_elements_count = (*gltf)["accessors"][inverseBindMatricesIndex]["count"].value.iNumber;
			std::string* inverse_bind_matrices_element_type   = (*gltf)["accessors"][inverseBindMatricesIndex]["type"].value.string;
			unsigned int inverse_bind_matrices_componet_type  = (*gltf)["accessors"][inverseBindMatricesIndex]["componentType"].value.iNumber;
//			std::cout << "inverse bind matrix component type: " << inverse_bind_matrices_componet_type << std::endl;
//			std::cout << "inverse bind matrix elements count: " << inverse_bind_matrices_elements_count << std::endl;
			unsigned int inverse_bind_matrices_buffer_view_byte_length = 0;
			unsigned int inverse_bind_matrices_byte_step = 0;
			calculateElementsMemorySize( inverse_bind_matrices_elements_count, inverse_bind_matrices_element_type,
										 inverse_bind_matrices_componet_type, &inverse_bind_matrices_buffer_view_byte_length );
			calculateByteStep( inverse_bind_matrices_componet_type, &inverse_bind_matrices_byte_step );

			[[maybe_unused]] int inverse_bind_matrices_buffer_view_byte_offset = 0;
			if( (*gltf)["accessors"][inverseBindMatricesIndex].isObject() == JSON_OBJECT ) {
				HashMap<JsonValue>* ptr = (*gltf)["accessors"][inverseBindMatricesIndex].value.object;
				if( ptr->Contain("byteOffset") ) {
					inverse_bind_matrices_buffer_view_byte_offset = (*gltf)["accessors"][inverseBindMatricesIndex]["byteOffset"].value.iNumber;
				}
			}

			[[maybe_unused]] unsigned int byteLengthInverseBindMatrices = (*gltf)["bufferViews"][bufferView]["byteLength"].value.iNumber;
			unsigned int byteOffsetInverseBindMatrices = (*gltf)["bufferViews"][bufferView]["byteOffset"].value.iNumber;

			core::vector<float> inverseBindMatricesData;
			for ( unsigned int i = byteOffsetInverseBindMatrices + inverse_bind_matrices_buffer_view_byte_offset; i < byteOffsetInverseBindMatrices + inverse_bind_matrices_buffer_view_byte_offset + inverse_bind_matrices_buffer_view_byte_length; i += inverse_bind_matrices_byte_step )
				inverseBindMatricesData.Push(reinterpret_cast<float &>(buffer[i]));

			mat4 inverseBindMatrix(0.0f);
			for ( unsigned int n = 0; n < joints.value.array->GetSize(); ++n ) {
				for ( unsigned int g = 0; g < 4; ++g )
					for ( unsigned int j = 0; j < 4; ++j ) {
						inverseBindMatrix[g][j] = inverseBindMatricesData[n * 16 + g * 4 + j];            ///< Put row float data into mat4
//						inverseBindMatrix[j][g] = inverseBindMatricesData[n * 16 + g * 4 + j];
					}

				inverseBindMatrixSet.Push(inverseBindMatrix);
			}

			unsigned int joints_index = (*gltf)["meshes"][0]["primitives"][0]["attributes"]["JOINTS_0"].value.iNumber;
			unsigned int joints_buffer_view_index = (*gltf)["accessors"][joints_index]["bufferView"].value.iNumber;

			unsigned int joints_elements_count = (*gltf)["accessors"][joints_index]["count"].value.iNumber;
			std::string* joints_element_type   = (*gltf)["accessors"][joints_index]["type"].value.string;
			unsigned int joints_componet_type  = (*gltf)["accessors"][joints_index]["componentType"].value.iNumber;
//			std::cout << "inverse bind matrix component type: " << inverse_bind_matrices_componet_type << std::endl;
//			std::cout << "inverse bind matrix elements count: " << inverse_bind_matrices_elements_count << std::endl;
			unsigned int joints_buffer_view_byte_length = 0;
			unsigned int joints_byte_step = 0;
			calculateElementsMemorySize( joints_elements_count, joints_element_type,
										 joints_componet_type, &joints_buffer_view_byte_length );
			calculateByteStep( joints_componet_type, &joints_byte_step );
			
			[[maybe_unused]] unsigned int joints_buffer_view_byte_offset = 0;
			if( (*gltf)["accessors"][joints_index].isObject() == JSON_OBJECT ) {
				HashMap<JsonValue>* ptr = (*gltf)["accessors"][joints_index].value.object;
				if( ptr->Contain("byteOffset") ) {
					joints_buffer_view_byte_offset = (*gltf)["accessors"][joints_index]["byteOffset"].value.iNumber;
				}
			}

			[[maybe_unused]] unsigned int joints_byte_length = (*gltf)["bufferViews"][joints_buffer_view_index]["byteLength"].value.iNumber;
			[[maybe_unused]] unsigned int joints_byte_offset = 0;
			if( (*gltf)["bufferViews"][joints_buffer_view_index].isObject() == JSON_OBJECT ) {
				HashMap<JsonValue>* ptr = (*gltf)["bufferViews"][joints_buffer_view_index].value.object;
				if( ptr->Contain("byteOffset") ) {
					joints_byte_offset = (*gltf)["bufferViews"][joints_buffer_view_index]["byteOffset"].value.iNumber;
				}
			}
			
			// std::cout << "joints index: " << joints_index << std::endl;
			// std::cout << "joints buffer view index: " << joints_buffer_view_index << std::endl;
			// std::cout << "joints byte length: " << joints_byte_length << std::endl;
			// std::cout << "joints byte offset: " << joints_byte_offset << std::endl;
			
			for ( unsigned int i = joints_byte_offset + joints_buffer_view_byte_offset; i < joints_byte_offset + joints_buffer_view_byte_offset + joints_buffer_view_byte_length; i += joints_byte_step ) {
				if ( joints_componet_type == 5121 ) {  // UNSIGNED_BYTE
					jointsIndices.Push(reinterpret_cast<unsigned char &>(buffer[i]));
				} else if ( joints_componet_type == 5123 ) {  // UNSIGNED_SHORT
					jointsIndices.Push(reinterpret_cast<unsigned short &>(buffer[i]));
				}
//				jointsIndices.Push(reinterpret_cast<char &>(buffer[i]));
			}

			unsigned int weights_index = (*gltf)["meshes"][0]["primitives"][0]["attributes"]["WEIGHTS_0"].value.iNumber;
			unsigned int weights_buffer_view_index = (*gltf)["accessors"][weights_index]["bufferView"].value.iNumber;

			unsigned int weights_elements_count = (*gltf)["accessors"][weights_index]["count"].value.iNumber;
			std::string* weights_element_type   = (*gltf)["accessors"][weights_index]["type"].value.string;
			unsigned int weights_componet_type  = (*gltf)["accessors"][weights_index]["componentType"].value.iNumber;
//			std::cout << "inverse bind matrix component type: " << inverse_bind_matrices_componet_type << std::endl;
//			std::cout << "inverse bind matrix elements count: " << inverse_bind_matrices_elements_count << std::endl;
			unsigned int weights_buffer_view_byte_length = 0;
			unsigned int weights_byte_step = 0;
			calculateElementsMemorySize( weights_elements_count, weights_element_type,
										 weights_componet_type, &weights_buffer_view_byte_length );
			calculateByteStep( weights_componet_type, &weights_byte_step );

			[[maybe_unused]] int weights_buffer_view_byte_offset = 0;
			if( (*gltf)["accessors"][weights_index].isObject() == JSON_OBJECT ) {
				HashMap<JsonValue>* ptr = (*gltf)["accessors"][weights_index].value.object;
				if( ptr->Contain("byteOffset") ) {
					weights_buffer_view_byte_offset = (*gltf)["accessors"][weights_index]["byteOffset"].value.iNumber;
				}
			}

			[[maybe_unused]] unsigned int weights_byte_length = (*gltf)["bufferViews"][weights_buffer_view_index]["byteLength"].value.iNumber;
			unsigned int weights_byte_offset = (*gltf)["bufferViews"][weights_buffer_view_index]["byteOffset"].value.iNumber;

			for ( unsigned int i = weights_byte_offset + weights_buffer_view_byte_offset; i < weights_byte_offset + weights_buffer_view_byte_offset + weights_buffer_view_byte_length; i += weights_byte_step )
				weightsContainer.Push(reinterpret_cast<float &>(buffer[i]));
		} else {
			noAnimations = true;
		}

		core::vector<Core::JsonValue> animations = Search("animations");

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
			core::vector<u32> nodesMapTranslations;
			core::vector<u32> nodesMapRotations;
			core::vector<u32> nodesMapScales;
			for ( unsigned int i = 0; i < samplerIndices.GetSize(); ++i ) {
				if ( *targetPaths[i].value.string == "translation" ) {
//					std::cout << "translation sampler index: " << samplerIndices[i].value.iNumber << std::endl;
					translationSamplerIndices.Push(samplerIndices[i].value.iNumber);
					nodesMapTranslations.Push( samplerIndices[i].value.iNumber );
				} else if ( *targetPaths[i].value.string == "rotation" ) {
//					std::cout << "rotation sampler index: " << samplerIndices[i].value.iNumber << std::endl;
					rotationSamplerIndices.Push(samplerIndices[i].value.iNumber);
					nodesMapRotations.Push( samplerIndices[i].value.iNumber );
				} else if ( *targetPaths[i].value.string == "scale" ) {
//					std::cout << "scale sampler index: " << samplerIndices[i].value.iNumber << std::endl;
					scaleSamplerIndices.Push(samplerIndices[i].value.iNumber);
					nodesMapScales.Push( samplerIndices[i].value.iNumber );
				}
			}

			Core::JsonValue samplers = (*gltf)["animations"][0]["samplers"];
				
			core::vector<unsigned int> translationInputs;
			core::vector<unsigned int> translationOutputs;
				
			for ( unsigned int i = 0; i < translationSamplerIndices.GetSize(); ++i) {
				// std::cout << "translation input index: " << samplers[translationSamplerIndices[i]]["input"].value.iNumber << std::endl;
				// std::cout << "i: " << i << std::endl;
				translationInputs.Push(samplers[translationSamplerIndices[i]]["input"].value.iNumber);
			}

			for ( unsigned int i = 0; i < translationSamplerIndices.GetSize(); ++i) {
				// std::cout << "translation output index: " << samplers[translationSamplerIndices[i]]["output"].value.iNumber << std::endl;
				// std::cout << "i: " << i << std::endl;
				translationOutputs.Push(samplers[translationSamplerIndices[i]]["output"].value.iNumber);
			}

			core::vector<core::vector<float>> frameInputsTranslation;
			for ( unsigned int i = 0; i < translationInputs.GetSize(); ++i) {
//				std::cout << "input accessor index: " << translationInputs[i] << std::endl;
				
				unsigned int frameBufferViewIndex =
					(*gltf)["accessors"][translationInputs[i]]["bufferView"].value.iNumber;
				
//				std::cout << "input buffer view index: " << frameBufferViewIndex << std::endl;

				unsigned int elements_count = (*gltf)["accessors"][translationInputs[i]]["count"].value.iNumber;
				[[maybe_unused]] std::string* element_type   = (*gltf)["accessors"][translationInputs[i]]["type"].value.string;
				[[maybe_unused]] unsigned int componet_type  = (*gltf)["accessors"][translationInputs[i]]["componentType"].value.iNumber;
//				std::cout << "elements count: " << elements_count << std::endl;
				unsigned int buffer_view_byte_length = 0;
				unsigned int byte_step = 0;
				calculateElementsMemorySize( elements_count, element_type,
											 componet_type, &buffer_view_byte_length );
				calculateByteStep( componet_type, &byte_step );

				int frameBufferView_buffer_view_byte_offset = 0;
				if( (*gltf)["accessors"][translationInputs[i]].isObject() == JSON_OBJECT ) {
					HashMap<JsonValue>* ptr = (*gltf)["accessors"][translationInputs[i]].value.object;
					if( ptr->Contain("byteOffset") ) {
						frameBufferView_buffer_view_byte_offset = (*gltf)["accessors"][translationInputs[i]]["byteOffset"].value.iNumber;
					}
				}
				
				[[maybe_unused]] unsigned int frameByteLength      =
					(*gltf)["bufferViews"][frameBufferViewIndex]["byteLength"].value.iNumber;
				unsigned int frameByteOffset      =
					(*gltf)["bufferViews"][frameBufferViewIndex]["byteOffset"].value.iNumber;

				core::vector<float> temp;
				// std::cout << "translation inputs buffer view byte length: " << buffer_view_byte_length << std::endl;
				// std::cout << "buffer view inner offset: " << frameBufferView_buffer_view_byte_offset << std::endl;
				// std::cout << "buffer view outer offset: " << frameByteOffset << std::endl;
				for ( unsigned int i = frameByteOffset + frameBufferView_buffer_view_byte_offset; i < frameByteOffset + frameBufferView_buffer_view_byte_offset + buffer_view_byte_length; i += byte_step ) {
//					std::cout << "translation inputs index: " << i << std::endl;
					temp.Push(reinterpret_cast<float &>(buffer[i]));
				}

				// for ( unsigned int i = frameByteOffset; i < frameByteOffset + frameByteLength; i += 4 ) {
				// 	std::cout << "translation inputs index: " << i << std::endl;
				// 	temp.Push(reinterpret_cast<float &>(buffer[i]));
				// }
				
				frameInputsTranslation.Push(temp);
			}

			core::vector<core::vector<float>> translations;
			for ( unsigned int i = 0; i < translationOutputs.GetSize(); ++i) {
//				std::cout << "ouput accessor index: " << translationOutputs[i] << std::endl;
				unsigned int outputBufferViewIndex =
					(*gltf)["accessors"][translationOutputs[i]]["bufferView"].value.iNumber;

//				std::cout << "output buffer view index: " << outputBufferViewIndex << std::endl;

				unsigned int elements_count = (*gltf)["accessors"][translationOutputs[i]]["count"].value.iNumber;
				std::string* element_type   = (*gltf)["accessors"][translationOutputs[i]]["type"].value.string;
				[[maybe_unused]] unsigned int componet_type  = (*gltf)["accessors"][translationOutputs[i]]["componentType"].value.iNumber;
				unsigned int buffer_view_byte_length = 0;
				unsigned int byte_step = 0;
				calculateElementsMemorySize( elements_count, element_type,
											 componet_type, &buffer_view_byte_length );
				calculateByteStep( componet_type, &byte_step );

				int outputBufferView_buffer_view_byte_offset = 0;
				if( (*gltf)["accessors"][translationOutputs[i]].isObject() == JSON_OBJECT ) {
					HashMap<JsonValue>* ptr = (*gltf)["accessors"][translationOutputs[i]].value.object;
					if( ptr->Contain("byteOffset") ) {
						outputBufferView_buffer_view_byte_offset = (*gltf)["accessors"][translationOutputs[i]]["byteOffset"].value.iNumber;
					}
				}

				[[maybe_unused]] unsigned int outputByteLength      =
					(*gltf)["bufferViews"][outputBufferViewIndex]["byteLength"].value.iNumber;
				unsigned int outputByteOffset      =
					(*gltf)["bufferViews"][outputBufferViewIndex]["byteOffset"].value.iNumber;

				core::vector<float> temp;
				// std::cout << "translation outputs buffer view byte length: " << buffer_view_byte_length << std::endl;
				// std::cout << "buffer view inner offset: " << outputBufferView_buffer_view_byte_offset << std::endl;
				// std::cout << "buffer view outer offset: " << outputByteOffset << std::endl;
				for ( unsigned int i = outputByteOffset + outputBufferView_buffer_view_byte_offset; i < outputByteOffset + outputBufferView_buffer_view_byte_offset + buffer_view_byte_length; i += byte_step ) {
//					std::cout << "translation ouputs index: " << i << std::endl;
					temp.Push(reinterpret_cast<float &>(buffer[i]));
				}

				// for ( unsigned int i = outputByteOffset; i < outputByteOffset + outputByteLength; i += 4 ) {
				// 	std::cout << "translation ouputs index: " << i << std::endl;
				// 	temp.Push(reinterpret_cast<float &>(buffer[i]));
				// }
				
				translations.Push(temp);
			}

			core::vector<unsigned int> rotationInputs;
			core::vector<unsigned int> rotationOutputs;
				
			for ( unsigned int i = 0; i < rotationSamplerIndices.GetSize(); ++i) {
				// std::cout << "rotation input sampler index: " << rotationSamplerIndices[i] << std::endl;
				// std::cout << "rotation input index: " << samplers[rotationSamplerIndices[i]]["input"].value.iNumber << std::endl;	
				rotationInputs.Push(samplers[rotationSamplerIndices[i]]["input"].value.iNumber);
			}

			for ( unsigned int i = 0; i < rotationSamplerIndices.GetSize(); ++i) {
				// std::cout << "rotation output sampler index: " << rotationSamplerIndices[i] << std::endl;
				// std::cout << "rotation output index: " << samplers[rotationSamplerIndices[i]]["output"].value.iNumber << std::endl;	
				rotationOutputs.Push(samplers[rotationSamplerIndices[i]]["output"].value.iNumber);
			}

			core::vector<core::vector<float>> frameInputsRotation;
			for ( unsigned int i = 0; i < rotationInputs.GetSize(); ++i) {
//				std::cout << "input accessor index: " << rotationInputs[i] << std::endl;
				unsigned int frameBufferViewIndex =
					(*gltf)["accessors"][rotationInputs[i]]["bufferView"].value.iNumber;

//				std::cout << "input buffer view index: " << frameBufferViewIndex << std::endl;

				unsigned int elements_count = (*gltf)["accessors"][rotationInputs[i]]["count"].value.iNumber;
				std::string* element_type   = (*gltf)["accessors"][rotationInputs[i]]["type"].value.string;
				[[maybe_unused]] unsigned int componet_type  = (*gltf)["accessors"][rotationInputs[i]]["componentType"].value.iNumber;
				unsigned int buffer_view_byte_length = 0;
				unsigned int byte_step = 0;
				calculateElementsMemorySize( elements_count, element_type,
											 componet_type, &buffer_view_byte_length );
				calculateByteStep( componet_type, &byte_step );

				int frameBufferView_buffer_view_byte_offset = 0;
				if( (*gltf)["accessors"][rotationInputs[i]].isObject() == JSON_OBJECT ) {
					HashMap<JsonValue>* ptr = (*gltf)["accessors"][rotationInputs[i]].value.object;
					if( ptr->Contain("byteOffset") ) {
						frameBufferView_buffer_view_byte_offset = (*gltf)["accessors"][rotationInputs[i]]["byteOffset"].value.iNumber;
					}
				}
				
				[[maybe_unused]] unsigned int frameByteLength      =
					(*gltf)["bufferViews"][frameBufferViewIndex]["byteLength"].value.iNumber;
				unsigned int frameByteOffset      =
					(*gltf)["bufferViews"][frameBufferViewIndex]["byteOffset"].value.iNumber;

				core::vector<float> temp;
				// std::cout << "rotation inputs buffer view byte length: " << buffer_view_byte_length << std::endl;
				// std::cout << "buffer view inner offset: " << frameBufferView_buffer_view_byte_offset << std::endl;
				// std::cout << "buffer view outer offset: " << frameByteOffset << std::endl;
				for ( unsigned int i = frameByteOffset + frameBufferView_buffer_view_byte_offset; i < frameByteOffset + frameBufferView_buffer_view_byte_offset + buffer_view_byte_length; i += byte_step ) {
//					std::cout << "rotation input index: " << i << std::endl;
					temp.Push(reinterpret_cast<float &>(buffer[i]));
				}

				// for ( unsigned int i = frameByteOffset; i < frameByteOffset + frameByteLength; i += 4 ) {
				// 	std::cout << "rotation input index: " << i << std::endl;
				// 	temp.Push(reinterpret_cast<float &>(buffer[i]));
				// }

				frameInputsRotation.Push(temp);
			}

			core::vector<core::vector<float>> rotations;
			for ( unsigned int i = 0; i < rotationOutputs.GetSize(); ++i) {
//				std::cout << "output accessor index: " << rotationOutputs[i] << std::endl;
				unsigned int outputBufferViewIndex =
					(*gltf)["accessors"][rotationOutputs[i]]["bufferView"].value.iNumber;

//				std::cout << "output buffer view index: " << outputBufferViewIndex << std::endl;

				unsigned int elements_count = (*gltf)["accessors"][rotationOutputs[i]]["count"].value.iNumber;
				std::string* element_type   = (*gltf)["accessors"][rotationOutputs[i]]["type"].value.string;
				[[maybe_unused]] unsigned int componet_type  = (*gltf)["accessors"][rotationOutputs[i]]["componentType"].value.iNumber;
				unsigned int buffer_view_byte_length = 0;
				unsigned int byte_step = 0;
				calculateElementsMemorySize( elements_count, element_type,
											 componet_type, &buffer_view_byte_length );
				calculateByteStep( componet_type, &byte_step );

				int outputBufferView_buffer_view_byte_offset = 0;
				if( (*gltf)["accessors"][rotationOutputs[i]].isObject() == JSON_OBJECT ) {
					HashMap<JsonValue>* ptr = (*gltf)["accessors"][rotationOutputs[i]].value.object;
					if( ptr->Contain("byteOffset") ) {
						outputBufferView_buffer_view_byte_offset = (*gltf)["accessors"][rotationOutputs[i]]["byteOffset"].value.iNumber;
					}
				}
//				std::cout << "OFFSETISHEE!!" << outputBufferView_buffer_view_byte_offset << std::endl;
				[[maybe_unused]] unsigned int outputByteLength      =
					(*gltf)["bufferViews"][outputBufferViewIndex]["byteLength"].value.iNumber;
				unsigned int outputByteOffset      =
					(*gltf)["bufferViews"][outputBufferViewIndex]["byteOffset"].value.iNumber;

				core::vector<float> temp;
				// std::cout << "rotation outputs buffer view byte length: " << buffer_view_byte_length << std::endl;
				// std::cout << "buffer view inner offset: " << outputBufferView_buffer_view_byte_offset << std::endl;
				// std::cout << "buffer view outer offset: " << outputByteOffset << std::endl;
				for ( unsigned int i = outputByteOffset + outputBufferView_buffer_view_byte_offset; i < outputByteOffset + outputBufferView_buffer_view_byte_offset + buffer_view_byte_length; i += byte_step ) {
//					std::cout << "rotation output index: " << i << std::endl;
					temp.Push(reinterpret_cast<float &>(buffer[i]));
				}

				// for ( unsigned int i = outputByteOffset; i < outputByteOffset + outputByteLength; i += 4 ) {
				// 	std::cout << "rotation output index: " << i << std::endl;
				// 	temp.Push(reinterpret_cast<float &>(buffer[i]));
				// }
				
				rotations.Push(temp);
			}
			
			core::vector<unsigned int> scaleInputs;
			core::vector<unsigned int> scaleOutputs;
				
			for ( unsigned int i = 0; i < scaleSamplerIndices.GetSize(); ++i)
				scaleInputs.Push(samplers[scaleSamplerIndices[i]]["input"].value.iNumber);

			for ( unsigned int i = 0; i < scaleSamplerIndices.GetSize(); ++i)
				scaleOutputs.Push(samplers[scaleSamplerIndices[i]]["output"].value.iNumber);

			core::vector<core::vector<float>> frameInputsScale;
			for ( unsigned int i = 0; i < scaleInputs.GetSize(); ++i) {
				unsigned int frameBufferViewIndex =
					(*gltf)["accessors"][scaleInputs[i]]["bufferView"].value.iNumber;

				unsigned int elements_count = (*gltf)["accessors"][scaleInputs[i]]["count"].value.iNumber;
				std::string* element_type   = (*gltf)["accessors"][scaleInputs[i]]["type"].value.string;
				[[maybe_unused]] unsigned int componet_type  = (*gltf)["accessors"][scaleInputs[i]]["componentType"].value.iNumber;
				[[maybe_unused]] unsigned int buffer_view_byte_length = 0;
				unsigned int byte_step = 0;
				calculateElementsMemorySize( elements_count, element_type,
											 componet_type, &buffer_view_byte_length );
				calculateByteStep( componet_type, &byte_step );

				int frameBufferView_buffer_view_byte_offset = 0;
				if( (*gltf)["accessors"][scaleInputs[i]].isObject() == JSON_OBJECT ) {
					HashMap<JsonValue>* ptr = (*gltf)["accessors"][scaleInputs[i]].value.object;
					if( ptr->Contain("byteOffset") ) {
						frameBufferView_buffer_view_byte_offset = (*gltf)["accessors"][scaleInputs[i]]["byteOffset"].value.iNumber;
					}
				}

				[[maybe_unused]] unsigned int frameByteLength      =
					(*gltf)["bufferViews"][frameBufferViewIndex]["byteLength"].value.iNumber;
				unsigned int frameByteOffset      =
					(*gltf)["bufferViews"][frameBufferViewIndex]["byteOffset"].value.iNumber;

				core::vector<float> temp;
				for ( unsigned int i = frameByteOffset + frameBufferView_buffer_view_byte_offset; i < frameByteOffset + frameBufferView_buffer_view_byte_offset + buffer_view_byte_length; i += byte_step )
					temp.Push(reinterpret_cast<float &>(buffer[i]));

				// for ( unsigned int i = frameByteOffset; i < frameByteOffset + frameBufferView_buffer_view_byte_offset + frameByteLength; i += 4 )
				// 	temp.Push(reinterpret_cast<float &>(buffer[i]));
				
				frameInputsScale.Push(temp);
			}

			core::vector<core::vector<float>> scales;
			for ( unsigned int i = 0; i < scaleOutputs.GetSize(); ++i) {
				unsigned int outputBufferViewIndex =
					(*gltf)["accessors"][scaleOutputs[i]]["bufferView"].value.iNumber;

				unsigned int elements_count = (*gltf)["accessors"][scaleOutputs[i]]["count"].value.iNumber;
				std::string* element_type   = (*gltf)["accessors"][scaleOutputs[i]]["type"].value.string;
				[[maybe_unused]] unsigned int componet_type  = (*gltf)["accessors"][scaleOutputs[i]]["componentType"].value.iNumber;
				[[maybe_unused]] unsigned int buffer_view_byte_length = 0;
				unsigned int byte_step = 0;
				calculateElementsMemorySize( elements_count, element_type,
											 componet_type, &buffer_view_byte_length );
				calculateByteStep( componet_type, &byte_step );

				int outputBufferView_buffer_view_byte_offset = 0;
				if( (*gltf)["accessors"][scaleOutputs[i]].isObject() == JSON_OBJECT ) {
					HashMap<JsonValue>* ptr = (*gltf)["accessors"][scaleOutputs[i]].value.object;
					if( ptr->Contain("byteOffset") ) {
						outputBufferView_buffer_view_byte_offset = (*gltf)["accessors"][scaleOutputs[i]]["byteOffset"].value.iNumber;
					}
				}

				[[maybe_unused]] unsigned int outputByteLength      =
					(*gltf)["bufferViews"][outputBufferViewIndex]["byteLength"].value.iNumber;
				unsigned int outputByteOffset      =
					(*gltf)["bufferViews"][outputBufferViewIndex]["byteOffset"].value.iNumber;

				core::vector<float> temp;
				// for ( unsigned int i = outputByteOffset + outputBufferView_buffer_view_byte_offset; i < outputByteOffset + outputBufferView_buffer_view_byte_offset + buffer_view_byte_length; i += 4 )
				// 	temp.Push(reinterpret_cast<float &>(buffer[i]));

				for ( unsigned int i = outputByteOffset + outputBufferView_buffer_view_byte_offset; i < outputByteOffset + outputBufferView_buffer_view_byte_offset + buffer_view_byte_length; i += byte_step )
					temp.Push(reinterpret_cast<float &>(buffer[i]));
				
				scales.Push(temp);
			}

			/// Searching for root joins WITH GOAT GOTO OPERATOR!!!
			core::vector<int> root_joins;
//			for ( unsigned int s = 0; s < joints.value.array->GetSize(); ++s ) {
//				int current_joint = (*joints.value.array)[s].value.iNumber;

			Core::JsonValue nodes = (*gltf)["nodes"];
			for ( unsigned int s = 0; s < nodes.value.array->GetSize(); ++s ) {
				int current_joint = s;
				
				for ( unsigned w = 0; w < children.GetSize(); ++w ) {
					for ( unsigned q = 0; q < children[w].GetSize(); ++q ) {
						if ( children[w][q] == current_joint )
							goto most_scary_operator_of_all_time;                                        ///< Yes. This is what we all deserve
					}
				}
				root_joins.Push(current_joint);     ///< If we execute this line then this joint index ectualy the root
				
			  most_scary_operator_of_all_time:                                                           ///< Not so scary at all. Am i right?
				continue;
			}

			for( unsigned int i = 0; i < root_joins.GetSize(); ++i ) {
				std::cout << "root joint: " << root_joins[i] << std::endl;
			}
			
//			frames = frameInputsTranslation[0];
			core::vector<core::vector<mat4>> animatedJointMatricesAccumulator;        ///< Delete this sheet!

			core::vector<core::vector<unsigned int>> joints_bones;
			for ( unsigned int w = 0; w < root_joins.GetSize(); ++w ) {     ///< Loop on parent joints
				core::vector<core::vector<unsigned int>> nodes_bones;
				unsigned int currentRoot = root_joins[w];
				core::stack<u32> node_stack;
				node_stack.push(currentRoot);                               ///< Start from root joint

				core::stack<u32> deepness_stack;
//				std::cout << "file path: " << pathsGLTF_ << std::endl;
				traversalBones(children, joints, node_stack, deepness_stack, nodes_bones);

				for ( unsigned int e = 0; e < nodes_bones.GetSize(); ++e ) {
					joints_bones.Push(nodes_bones[e]);
				}
			}

			// for( unsigned int i = 0; i < joints_bones.GetSize(); ++i ) {
			// 	std::cout << "next node" << std::endl;
			// 	for( unsigned int j = 0; j < joints_bones[i].GetSize(); ++j ) {
			// 		std::cout << "joint: " << joints_bones[i][j] << std::endl;
			// 	}
			// }

//			std::cout << "size of joints_bones: " << joints_bones.GetSize() << std::endl;


			/*
			  ================================================================
			  This logic related to joints that has inverseBindMatrices
			  ================================================================
			*/

			// std::cout << "size of translations: " << translations.GetSize() << std::endl;
			// std::cout << "size of rotations: " << rotations.GetSize() << std::endl;
			// std::cout << "size of scales: " << scales.GetSize() << std::endl;
			
			[[maybe_unused]] const u32 transformationsMax = translations.GetSize() > scales.GetSize() ?
				(translations.GetSize() > rotations.GetSize()
				 ? translations.GetSize() : rotations.GetSize()) :
				(scales.GetSize() > rotations.GetSize() ? scales.GetSize() : rotations.GetSize());

			u32 translationFramesNumber = 0;
			if( frameInputsTranslation.GetSize() > 0 ) {
				translationFramesNumber = frameInputsTranslation[0].GetSize();
			}

			u32 rotationFramesNumber = 0;
			if( frameInputsRotation.GetSize() > 0 ) {
				rotationFramesNumber = frameInputsRotation[0].GetSize();
			}

			u32 scaleFramesNumber = 0;
			if( frameInputsScale.GetSize() > 0 ) {
				scaleFramesNumber = frameInputsScale[0].GetSize();
			}

			const u32 framesMax = translationFramesNumber > scaleFramesNumber ?
				(translationFramesNumber > rotationFramesNumber
				 ? translationFramesNumber : rotationFramesNumber) :
				(scaleFramesNumber > rotationFramesNumber ? scaleFramesNumber : rotationFramesNumber);
			
			// [[maybe_unused]] const u32 framesMax = frameInputsTranslation[0].GetSize() > frameInputsScale[0].GetSize() ?
			// 	(frameInputsTranslation[0].GetSize() > frameInputsRotation[0].GetSize()
			// 	 ? frameInputsTranslation[0].GetSize() : frameInputsRotation[0].GetSize()) :
			// 	(frameInputsScale[0].GetSize() > frameInputsRotation[0].GetSize() ? frameInputsScale[0].GetSize() : frameInputsRotation[0].GetSize());
			
			std::cout << "tranformations max: " << transformationsMax << std::endl;
			std::cout << "frames max: " << framesMax << std::endl;

//			frames = framesMax;
			
			const u32 translationsSize = translations.GetSize();
			const u32 rotationsSize    = rotations.GetSize();
			const u32 scalesSize       = scales.GetSize();
			for( unsigned int i = 0; i < transformationsMax; ++i ) {
				if( i >= translationsSize ) {
					core::vector<float> temp;
					for( u32 i1 = 0; i1 < framesMax; ++i1 ) {
						temp.Push( 0.0f );
						temp.Push( 0.0f );
						temp.Push( 0.0f );
					}
					translations.Push( temp );
				}

				if( i >= rotationsSize ) {
					core::vector<float> temp;
					for( u32 i1 = 0; i1 < framesMax; ++i1 ) {
						temp.Push( 0.0f );
						temp.Push( 0.0f );
						temp.Push( 0.0f );
						temp.Push( 1.0f );
					}
					rotations.Push( temp );
				}

				if( i >= scalesSize ) {
					core::vector<float> temp;
					for( u32 i1 = 0; i1 < framesMax; ++i1 ) {
						temp.Push( 1.0f );
					}
					scales.Push( temp );
				}

				while( translations[i].GetSize() < framesMax * 3 ) {
					translations[i].Push( 0.0f );
					translations[i].Push( 0.0f );
					translations[i].Push( 0.0f );
				}

				while( rotations[i].GetSize() < framesMax * 4 ) {
					rotations[i].Push( 0.0f );
					rotations[i].Push( 0.0f );
					rotations[i].Push( 0.0f );
					rotations[i].Push( 1.0f );
				}

				while( scales[i].GetSize() < framesMax ) {
					scales[i].Push( 1.0f );
				}
			}

			frames = frameInputsRotation[0];
			for ( unsigned int j = 0; j < transformationsMax; ++j ) {
				core::vector<float> boneAllFrameTranslations = translations[j];
				core::vector<float> boneAllFrameRotations    = rotations[j];
				core::vector<float> boneAllFrameScales;
				if ( scales.GetSize() > 0 )
					boneAllFrameScales       = scales[j];
				core::vector<mat4>  globalAllFrameNodeMatrix;
				core::vector<mat4>  globalAllFrameNodeMatrixAccumulator;
				for ( unsigned int i = 0; i < framesMax; ++i ) {
					mat4 frameTranslation(1.0f);
					mat4 frameScale(1.0f);
					for ( unsigned int q = 0; q < 3; ++q ) {
//						std::cout << "bone translation array size: " << boneAllFrameTranslations.GetSize() << std::endl;
//						std::cout << "index: " << i * 3 + q << std::endl;
						frameTranslation[3][q] = boneAllFrameTranslations[i * 3 + q];
						if ( scales.GetSize() > 0 ) {
							while( i * 3 + q >= boneAllFrameScales.GetSize() ) {
								boneAllFrameScales.Push( 1.0f );
							}
							frameScale[q][q]       = boneAllFrameScales[i * 3 + q];
						}
					}

					Quaternion frameRotationQuaternion;
					mat4 frameRotation(1.0f);

					// if( boneAllFrameRotations.GetSize() == i * 4 ) {
					// 	boneAllFrameRotations.Push( 0.0f );
					// 	boneAllFrameRotations.Push( 0.0f );
					// 	boneAllFrameRotations.Push( 0.0f );
					// 	boneAllFrameRotations.Push( 1.0f );
					// }
					
					frameRotationQuaternion.x = boneAllFrameRotations[i * 4];
					frameRotationQuaternion.y = boneAllFrameRotations[i * 4 + 1];
					frameRotationQuaternion.z = boneAllFrameRotations[i * 4 + 2];
					frameRotationQuaternion.w = boneAllFrameRotations[i * 4 + 3];

					frameRotation = rotateQuaternion<float, 4>(frameRotationQuaternion);
					frameRotation.SelfTensorTranspose();

					mat4 globalTransformNodeMatrix = frameScale * frameRotation * frameTranslation;
					globalAllFrameNodeMatrixAccumulator.Push(globalTransformNodeMatrix);
				}
				animatedJointMatricesAccumulator.Push(globalAllFrameNodeMatrixAccumulator);
			}

			core::vector<core::vector<mat4>> resultJointMatricesAccumulator;
			for( unsigned int i = 0; i < joints_bones.GetSize(); ++i ) {
				for( unsigned int j = 0; j < joints_bones[i].GetSize(); ++j ) {
					const u32 currentJoint = joints_bones[i][j];
					const u32 isExists     = getJointIndex(joints, currentJoint); ///< Is currentJoint exists in array related to frame animations

					if( isExists == UINT32_MAX || isExists >= animatedJointMatricesAccumulator.GetSize() ) {
						core::vector<mat4> temp;
						for( unsigned int v = 0; v < framesMax; ++v ) {
							mat4 unit;
							temp.Push( unit );
						}

						resultJointMatricesAccumulator.Push( temp );
					} else {
						resultJointMatricesAccumulator.Push( animatedJointMatricesAccumulator[isExists] );
					}
				}
			}

			for ( unsigned int j = 0; j < joints_bones.GetSize(); ++j ) {
				const u32 currentJoint = joints_bones[j][0];
				const u32 isExists     = getJointIndex(joints, currentJoint); ///< Is currentJoint exists in array related to frame animations

				if( isExists == UINT32_MAX ) {
					mat4 unit( 1.0f );
					inverseBindMatrixSet.Push( unit );
				}
			}
			
			for ( unsigned int j = 0; j < nodes.value.array->GetSize(); ++j ) {
//			for ( unsigned int j = 0; j < translations.GetSize(); ++j ) {
				core::vector<mat4>  globalAllFrameNodeMatrix;

				// Guard before the inner loop
				// if ( j >= inverseBindMatrixSet.GetSize() ) {
				// 	jointMatrices.Push(globalAllFrameNodeMatrix);
				// 	continue;
				// }
				
				for ( unsigned int i = 0; i < framesMax; ++i ) {
					mat4 rootTransform(1.0f);
					// while( joints_bones.GetSize() <= j ) {
					// 	joints_bones.Push( {} );
					// }
					
					for ( unsigned int b = 0; b < joints_bones[j].GetSize() - 1; ++b ) {
 						// while( joints_bones[j].GetSize() <= b ) {
						// 	joints_bones[j].Push( {} );
						// }
						
						// const u32 innerIndex = joints_bones[j][b];
 						// while( resultJointMatricesAccumulator.GetSize() <= innerIndex ) {
						// 	resultJointMatricesAccumulator.Push( {} );
						// }

						// while( resultJointMatricesAccumulator[joints_bones[j][b]].GetSize() <= i ) {
						// 	mat4 unit(1.0f);
						// 	resultJointMatricesAccumulator[joints_bones[j][b]].Push( unit );
						// }
						
						rootTransform = resultJointMatricesAccumulator[joints_bones[j][b]][i] * rootTransform;
					}
					// if ( j >= jointMatricesAccumulator.GetSize() || i >= jointMatricesAccumulator[j].GetSize() ) {
					// 	mat4 unit(1.0f);
					// 	globalAllFrameNodeMatrix.Push(unit);
					// 	continue;
					// }

//					const u32 isJointExist = getJointIndex(joints, 

					// std::cout << "j: " << j << std::endl;
					// std::cout << "i: " << i << std::endl;
					// std::cout << "inv mat size: " << inverseBindMatrixSet.GetSize() << std::endl;
					// std::cout << "res acum size outer: " << resultJointMatricesAccumulator.GetSize() << std::endl;
					// std::cout << "res acum size inner: " << resultJointMatricesAccumulator[j].GetSize() << std::endl;

					globalAllFrameNodeMatrix.Push(inverseBindMatrixSet[j] * resultJointMatricesAccumulator[j][i] * rootTransform);
				}
				jointMatrices.Push(globalAllFrameNodeMatrix);
			}

			
			// int maximumJoints    = 128;
			// int unitMatricesSize = maximumJoints - jointMatrices.GetSize();

			// if ( unitMatricesSize > 0 ) {
			// 	for ( int i = 0; i < unitMatricesSize; ++i) {
			// 		core::vector<mat4>  globalAllFrameNodeMatrix;
			// 		for ( unsigned int j = 0; j < frameInputsTranslation[0].GetSize(); ++j ) {
			// 			mat4 unitMatrix(1.0f);
			// 			globalAllFrameNodeMatrix.Push(unitMatrix);
			// 		}

			// 		jointMatrices.Push(globalAllFrameNodeMatrix);
			// 	}
			// }
		}

		// while( jointMatrices.GetSize() < 256 ) {
		// 	mat4 unit(1.0f);
		// 	core::vector<mat4> vec;
		// 	for( int i = 0; i < 64; ++i ) {
		// 		vec.Push( unit );
		// 	}
		// 	jointMatrices.Push( vec );
		// }

		jointMatricesPerMesh = jointMatrices;
		topY = -999.999f;
		for ( uint32_t i = 0; i < indices.GetSize(); ++i ) {
			aIndices_.push_back(i);

			unsigned int index = indices[i] * 3;
			if ( index + 2 < vertices_position.GetSize() ) {
				vec3 position = { vertices_position[index],
					vertices_position[index + 1],
					vertices_position[index + 2] };

				if ( position[1] > topY )
					topY = position[1];
				
				aVertexes_.push_back(position[0]);
				aVertexes_.push_back(position[1]);
				aVertexes_.push_back(position[2]);
			}

			if ( index + 2 < normals.GetSize() ) {
				vec3 normal = { normals[index],
					normals[index + 1],
					normals[index + 2] };

				aVertexes_.push_back(normal[0]);
				aVertexes_.push_back(normal[1]);
				aVertexes_.push_back(normal[2]);
			}

			index = indices[i] * 2;
			if ( index + 1 < texture_coordinates.GetSize() ) {
				aVertexes_.push_back(texture_coordinates[index]);
				aVertexes_.push_back(texture_coordinates[index + 1]);
			}

			index = indices[i] * 4;
			if ( index + 3 < jointsIndices.GetSize() ) {
				aVertexes_.push_back(jointsIndices[index]);
				aVertexes_.push_back(jointsIndices[index + 1]);
				aVertexes_.push_back(jointsIndices[index + 2]);
				aVertexes_.push_back(jointsIndices[index + 3]);
			}

			if ( index + 3 < weightsContainer.GetSize() ) {
				aVertexes_.push_back(weightsContainer[index]);
				aVertexes_.push_back(weightsContainer[index + 1]);
				aVertexes_.push_back(weightsContainer[index + 2]);
				aVertexes_.push_back(weightsContainer[index + 3]);
			}
		}

		delete [] buffer;
		buffer = nullptr;
	}

	/*
	========================================================================================
	@brief Collect all branches on every dipness level and put it in a row

	@param children       Contain all children of every root node
	@param joints         Use for access to main array of joints in "skins"
	@param node_stack     Keep all joints in current branch and has most deep as last elemnt
	@param deepness_stack Hold current deepness level of child tree in branch as element and
	                      value of an element is children counter
	========================================================================================
	*/
	
	void CJsonParser::traversalBones( core::vector<core::vector<int>> children,
									  Core::JsonValue joints,
									  core::stack<u32> node_stack,
									  core::stack<u32> deepness_stack,
									  core::vector<core::vector<u32>>& result ) {
		u32 topJointIndex = 0;
		if ( !node_stack.empty() ) {
//			topJointIndex = getJointIndex(joints, node_stack.top());      ///< Pass array of all joints and root joint and return index of root joint in array
			topJointIndex = node_stack.top();
		}
		
		if ( node_stack.size() > deepness_stack.size() ) {
			u32 firstChild = 0;                                           ///< First 0 level start from
			deepness_stack.push(firstChild);
		}

		if ( deepness_stack.empty() )                                     ///< Main exit check
			return;

		u32 nextNodeIndex = 0;
		if ( topJointIndex != UINT32_MAX && !children[topJointIndex].empty() ) {      ///< Check current root joint has any children. Children maps linearly with root joint array index
			if ( deepness_stack.top() > 0 && deepness_stack.top() == children[topJointIndex].GetSize() ) {   ///< Check if on last child level
				deepness_stack.pop();
				node_stack.pop();
				traversalBones( children, joints, node_stack, deepness_stack, result );
				return;
			}

			if ( deepness_stack.top() > 0 && deepness_stack.top() < children[topJointIndex].GetSize() ) {    ///< Check if not on last child level
				nextNodeIndex = children[topJointIndex][deepness_stack.top()];
				node_stack.push(nextNodeIndex);

				core::vector<u32> current_node_indices;
				for ( u32 i = 0; i < node_stack.size(); ++i ) {
//					u32 currentJoinIndex = getJointIndex(joints, node_stack[i]);
					current_node_indices.Push(node_stack[i]);
				}

				/* result.Push(current_node_indices); *///< Maybe this line has to be here
				
				++deepness_stack.top();
				traversalBones( children, joints, node_stack, deepness_stack, result );
				return;
			} else {
				core::vector<u32> current_node_indices;
				for ( u32 i = 0; i < node_stack.size(); ++i ) {
//					u32 currentJoinIndex = getJointIndex(joints, node_stack[i]);
					current_node_indices.Push(node_stack[i]);
				}
		
				result.Push(current_node_indices);
			
				nextNodeIndex = children[topJointIndex][deepness_stack.top()];
				node_stack.push(nextNodeIndex);
				++deepness_stack.top();
				traversalBones( children, joints, node_stack, deepness_stack, result );
				return;
			}
		} else {
			core::vector<u32> current_node_indices;
			if( topJointIndex == UINT32_MAX ) {
				current_node_indices.Push( node_stack.top() );
				result.Push(current_node_indices);
				return;
			}
			
			for ( u32 i = 0; i < node_stack.size(); ++i ) {
//				u32 currentJoinIndex = getJointIndex(joints, node_stack[i]);
				current_node_indices.Push(node_stack[i]);
			}

			result.Push(current_node_indices);
			
			deepness_stack.pop();
			node_stack.pop();
			traversalBones( children, joints, node_stack, deepness_stack, result );
			return;
		}
	}

	core::vector<core::vector<unsigned int>> CJsonParser::makeRenderJointsIndices(core::vector<core::vector<unsigned int>>& input) {
		core::vector<core::vector<unsigned int>> result;

		bool accumulatorFlag = false;
		bool innerFlag  = false;
		unsigned int accumulator = input[0][0];
		for ( unsigned int i = 0; i < input.GetSize(); ++i ) {
			for ( unsigned int j = 0; j < input[i].GetSize(); ++j ) {
				core::vector<unsigned int> inner;
				for ( unsigned int v = 0; v < j + 1; ++v ) {
					if ( input[i][j] == accumulator && accumulatorFlag ) {
						innerFlag = false;
						continue;
					} else {
						innerFlag = true;
						inner.Push(input[i][v]);

						if ( accumulatorFlag == false )
							accumulatorFlag = true;
					}
				}

				if ( innerFlag )
					result.Push(inner);
			}
		}

		return result;
	}

	bool CJsonParser::containsElemnt(core::vector<core::vector<unsigned int>> container, unsigned int element) {
		bool flag = false;
		
		for ( unsigned int i = 0; i < container.GetSize(); ++i ) {
			for ( unsigned int j = 0; j < container[i].GetSize(); ++j ) {
				if ( container[i][j] == element )
					return true;
			}
		}

		return flag;
	}
	
	u32 CJsonParser::getJointIndex(Core::JsonValue joints, i32 searchingIndex) {
		for ( unsigned int i = 0; i < joints.value.array->GetSize(); ++i ) {
			int currentJointIndex = (*joints.value.array)[i].value.iNumber;

			if ( currentJointIndex == searchingIndex )
				return i;
		}

		return -1;
	}

	CJsonParser::~CJsonParser() {
		delete root_;
	}
}
