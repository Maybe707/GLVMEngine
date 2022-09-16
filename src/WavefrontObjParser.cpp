#include "WavefrontObjParser.hpp"
#include "VectorContainer.hpp"
#include <iterator>

namespace GLVM::Core
{
    // CWaveFrontObjParser* CWaveFrontObjParser::pInstance_ = nullptr;
    // std::mutex CWaveFrontObjParser::Mutex_;
    
    CWaveFrontObjParser::CWaveFrontObjParser() {}

    // CWaveFrontObjParser* CWaveFrontObjParser::GetInstance()
    // {
    //     std::lock_guard<std::mutex> lock(Mutex_);
    //     if(pInstance_ == nullptr)
    //     {
    //         pInstance_ = new CWaveFrontObjParser();
    //     }
    //     return pInstance_;
    // }
    
    GLVM::Core::TCVectorContainer<SVertex> CWaveFrontObjParser::getCoordinateVertices() { return coordinateVertices_; }
    GLVM::Core::TCVectorContainer<SVertex> CWaveFrontObjParser::getTextureVertices() { return textureVertices_; }
    GLVM::Core::TCVectorContainer<SFace> CWaveFrontObjParser::getFaces() { return faces_; }
    
    void CWaveFrontObjParser::ReadFile(const char* _filePath) {
        const char* _pWavefrontObjFile = _filePath;
        std::ifstream WavefrontObjFileInputStream;
        std::stringstream WavefrontObjFileOutputStream;
        
        WavefrontObjFileInputStream.open(_pWavefrontObjFile);
        if(WavefrontObjFileInputStream.good()) {

            WavefrontObjFileOutputStream << WavefrontObjFileInputStream.rdbuf();
            WavefrontObjFileInputStream.close();
            sWavefrontObjFileData = WavefrontObjFileOutputStream.str();
        } else {
            std::cout << "Error of reading wavefront.obj file" << std::endl;
            return;
        }

        pWavefrontObjFileData = sWavefrontObjFileData.c_str();
    }
    
    void CWaveFrontObjParser::ParseFile() {
        unsigned int uiCoordinateVerticesIndex = 0;
        unsigned int uiTextureVerticesIndex = 0;
        unsigned int uiFacesIndex = 0;
        while(pWavefrontObjFileData[uiCounter] != '\0') {
            GLVM::Core::TCVectorContainer<TCVectorContainer<char>> line = Split(pWavefrontObjFileData, ' ', '\n', uiCounter);
            
            if (line[0] == "v") {
                SVertex vertex = ParseVertices(line);
                coordinateVertices_.Push(vertex);
                ++uiCoordinateVerticesIndex;
            }
            if (line[0] == "vt") {
                SVertex vertex = ParseVertices(line);
                textureVertices_.Push(vertex);
                ++uiTextureVerticesIndex;
            }
            if (line[0] == "f") {
                SFace face = ParseFaces(line);
                faces_.Push(face);
                                
                ++uiFacesIndex;
            }
        }
    }

    GLVM::Core::TCVectorContainer<TCVectorContainer<char>> CWaveFrontObjParser::Split(const char* _pWaveFrontObjFileData, const char _separator, const char _exitSymbol, unsigned int& _uiCounter) {
        GLVM::Core::TCVectorContainer<TCVectorContainer<char>> wordsContainer;
        unsigned int outerIndex = 0;
        wordsContainer.Push({});
        
        for(;;++_uiCounter) {
            if (_pWaveFrontObjFileData[_uiCounter] == '#') {
                while(_pWaveFrontObjFileData[_uiCounter] != '\n') {
                    ++_uiCounter;
                }
                continue;
            }
            if (_pWaveFrontObjFileData[_uiCounter] == _separator) {
                wordsContainer[outerIndex].Push('\0');
                wordsContainer.Push({});
                ++outerIndex;
                continue;
            }
            if (_pWaveFrontObjFileData[_uiCounter] == _exitSymbol) {
                ++_uiCounter;
                wordsContainer[outerIndex].Push('\0');
                return wordsContainer;
            }
            wordsContainer[outerIndex].Push(_pWaveFrontObjFileData[_uiCounter]);
        }
    }

    SVertex CWaveFrontObjParser::ParseVertices(GLVM::Core::TCVectorContainer<TCVectorContainer<char>> _wordsContainer) {
        SVertex vertex;
        unsigned int uiVertexIndex = 0;

        unsigned int uiWordsContainerSize = _wordsContainer.GetSize();
        for (int i = 1; i < uiWordsContainerSize; ++i) {
            float floatNumber = ParseFloating(_wordsContainer[i]);
            vertex[uiVertexIndex++] = floatNumber;
        }

        return vertex;
    }

    SFace CWaveFrontObjParser::ParseFaces(GLVM::Core::TCVectorContainer<TCVectorContainer<char>> _wordsContainer) {
        SFace face;
        GLVM::Core::TCVectorContainer<TCVectorContainer<char>> wordsInnerContainer;
        GLVM::Core::TCVectorContainer<char> word;
        
        unsigned int uiWordsContainerSize = _wordsContainer.GetSize();
        for (int i = 1; i < uiWordsContainerSize; ++i) {
            unsigned int counter = 0;
            wordsInnerContainer = Split(_wordsContainer[i].GetVectorContainer(), '/', '\0', counter);
            
            for (int j = 0; j < wordsInnerContainer.GetSize(); ++j) {

                word = wordsInnerContainer[j];

                int iValue = ParseInteger(word);

                face[j].Push(iValue);
            }
        }
        return face;
    }

    int CWaveFrontObjParser::ParseInteger(GLVM::Core::TCVectorContainer<char> _word) {
        GLVM::Core::TCVectorContainer<int> baseContainer;

        for (int i = 0; i < _word.GetSize() - 1; ++i)
            baseContainer.Push(_word[i] - 48);

        int iResult = 0;
        bool negateFlag = false;

        unsigned int baseContainerSize = baseContainer.GetSize();
        for (int i = 0; i < baseContainerSize; ++i) {
            if (negateFlag && i == 0)
                continue;
            else if (baseContainer[i] == -5 && i == 0)
                continue;

            iResult += baseContainer[i] * std::pow(10, (baseContainerSize - 1) - i);
        }

        return iResult;
    }
    
    float CWaveFrontObjParser::ParseFloating(GLVM::Core::TCVectorContainer<char> _word) {
        GLVM::Core::TCVectorContainer<int> baseContainer;

        for (int i = 0; i < _word.GetSize() - 1; ++i)
            baseContainer.Push(_word[i] - 48);

        int integerPart = 0;
        float floatingPart = 0;
        GLVM::Core::TCVectorContainer<int> integerPartContainer;
        GLVM::Core::TCVectorContainer<int> floatingPartContainer;
        bool dotFlag = false;
        bool negateFlag = false;
        unsigned int baseContainerSize = baseContainer.GetSize();

        if (baseContainer[0] == -3)
            negateFlag = true;

        for (int i = 0; i < baseContainerSize; ++i) {
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
        for (int i = 0; i < integerPartContainerSize; ++i)
            integerPart += integerPartContainer[i] * std::pow(10, (integerPartContainerSize - 1) - i);

        unsigned int floatingPartContainerSize = floatingPartContainer.GetSize();
        for (int i = 0; i < floatingPartContainerSize; ++i)
            floatingPart += floatingPartContainer[i] / std::pow(10, i + 1);

        float result = 0;
        result = (float)(integerPart + floatingPart);
        
        if (negateFlag)
            result *= -1.0f;

        return result;
    }
}
