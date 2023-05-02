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
}
