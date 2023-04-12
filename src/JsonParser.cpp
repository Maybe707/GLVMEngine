#include "JsonParser.hpp"
#include "Vector.hpp"
#include <chrono>
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
				float fNumber = 0.0f;
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
					root_ = CreateJsonHashMap();
					stackOfJsonValues_.Push(root_);
				} else if (keyFlag) {
					JsonValue* jsonObject = CreateJsonHashMap();
					(*stackOfJsonValues_.GetHead()->value.object)[lastKey_.c_str()] = *jsonObject;
					stackOfJsonValues_.Push(&(*stackOfJsonValues_.GetHead()->value.object)[lastKey_.c_str()]);
				} else if (!keyFlag) {
					JsonValue* jsonObject = CreateJsonHashMap();
					stackOfJsonValues_.GetHead()->value.array->Push(*jsonObject);
					stackOfJsonValues_.Push(&stackOfJsonValues_.GetHead()->value.array->GetHead());
				}

				keyFlag = true;
			} else if (currentChar_ == '[') {
				if (stackOfJsonValues_.GetSize() == 0) {
					root_ = CreateJsonArray();
					stackOfJsonValues_.Push(root_);
				} else if (keyFlag) {
					JsonValue* jsonArray = CreateJsonArray();
					(*stackOfJsonValues_.GetHead()->value.object)[lastKey_.c_str()] = *jsonArray;
					stackOfJsonValues_.Push(&(*stackOfJsonValues_.GetHead()->value.object)[lastKey_.c_str()]);
				} else if (!keyFlag) {
					JsonValue* jsonArray = CreateJsonArray();
					stackOfJsonValues_.GetHead()->value.array->Push(*jsonArray);
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

	JsonValue* CJsonParser::CreateJsonHashMap() {
		JsonValue* jsonObject = new JsonValue;
		jsonObject->type = JSON_OBJECT;
		jsonObject->value.object = new HashMap<JsonValue>;
		new (jsonObject->value.object) HashMap<JsonValue>;
		return jsonObject;
	}

	JsonValue* CJsonParser::CreateJsonArray() {
		JsonValue* jsonArray = new JsonValue;
		jsonArray->type = JSON_ARRAY;
		jsonArray->value.array = new core::vector<JsonValue>;
		new (jsonArray->value.array) core::vector<JsonValue>;
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
				currentChar_ == '+' || currentChar_ == '-') {
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
    
    float CJsonParser::ParseFloating(core::vector<char> _word) {
		core::vector<int> baseContainer;

        for (unsigned int i = 0; i < _word.GetSize(); ++i)
            baseContainer.Push(_word[i] - 48);

        int integerPart = 0;
        float floatingPart = 0;
		core::vector<int> integerPartContainer;
		core::vector<int> floatingPartContainer;
        bool dotFlag = false;
        bool negateFlag = false;
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

        unsigned int integerPartContainerSize = integerPartContainer.GetSize();
        for (unsigned int i = 0; i < integerPartContainerSize; ++i)
            integerPart += integerPartContainer[i] * std::pow(10, (integerPartContainerSize - 1) - i);

        unsigned int floatingPartContainerSize = floatingPartContainer.GetSize();
        for (unsigned int i = 0; i < floatingPartContainerSize; ++i)
            floatingPart += floatingPartContainer[i] / std::pow(10, i + 1);

        float result = 0;
        result = (float)(integerPart + floatingPart);
        
        if (negateFlag)
            result *= -1.0f;

        return result;
    }

	JsonValue* CJsonParser::SearchInJsonArray(core::vector<JsonValue>* arrayValue, const char* key_) const {
		for ( unsigned int i = 0; i < arrayValue->GetSize(); ++i ) {
			if ( (*arrayValue)[i].type == JSON_OBJECT ) {
				JsonValue* pValue = SearchInJsonObject((*arrayValue)[i].value.object, key_);
				if ( pValue != nullptr )
					return pValue;
			}

			if ( (*arrayValue)[i].type == JSON_ARRAY ) {
				JsonValue* pValue = SearchInJsonArray((*arrayValue)[i].value.array, key_);
				if ( pValue != nullptr )
					return pValue;
			}
		}

		return nullptr;
	}
	
	JsonValue* CJsonParser::SearchInJsonObject(HashMap<JsonValue>* mapValue, const char* key_) const {
		for ( unsigned int i = 0; i < mapValue->GetCapacity(); ++i ) {
			if ( mapValue->hashMap_[i] != nullptr ) {
				if ( mapValue->hashMap_[i]->key_ == key_ )
					return &mapValue->hashMap_[i]->value_;
						
				if ( mapValue->hashMap_[i]->value_.type == JSON_OBJECT ) {
					JsonValue* pValue = SearchInJsonObject(mapValue->hashMap_[i]->value_.value.object, key_);
					if ( pValue != nullptr )
						return pValue;
				}

				if ( mapValue->hashMap_[i]->value_.type == JSON_ARRAY ) {
					JsonValue* pValue = SearchInJsonArray(mapValue->hashMap_[i]->value_.value.array, key_);
					if ( pValue != nullptr )
						return pValue;
				}
			}
		}

		return nullptr;
	}		
	
	const JsonValue& CJsonParser::Search(const char* key_) const {
		JsonValue* value = SearchInJsonObject(root_->value.object, key_);
		return *value;
	}
}
