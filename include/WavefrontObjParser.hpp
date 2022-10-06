#ifndef WAVEFRONT_OBJ_PARSER
#define WAVEFRONT_OBJ_PARSER

#include <string>
#include "VectorContainer.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <vector>
#include <cmath>
#include <cassert>
#include <mutex>

#include <chrono>
#include <thread>

namespace GLVM::Core
{
    struct SVertex
    {
        float x;
        float y;
        float z;

        float& operator[](const unsigned int _iIndex) {
            assert(_iIndex < 3 && _iIndex >= 0 && "Wrong index");
            switch(_iIndex) {
            default:
            case 0:
                return x;
            case 1:
                return y;
            case 2:
                return z;
            }
        }
    };

    struct SFace
    {
        GLVM::Core::TCVectorContainer<int> vertexIndex;
        GLVM::Core::TCVectorContainer<int> textureIndex;
        GLVM::Core::TCVectorContainer<int> normalIndex;

        GLVM::Core::TCVectorContainer<int>& operator[](const unsigned int _iIndex) {
            assert(_iIndex < 3 && _iIndex >= 0 && "Wrong index");
            switch(_iIndex) {
            default:
            case 0:
                return vertexIndex;
            case 1:
                return textureIndex;
            case 2:
                return normalIndex;
            }
        }
    };
    
    class CWaveFrontObjParser
    {
        // static CWaveFrontObjParser* pInstance_;
        // static std::mutex  Mutex_;
        
        GLVM::Core::TCVectorContainer<SVertex> coordinateVertices_;
        GLVM::Core::TCVectorContainer<SVertex> textureVertices_;
		GLVM::Core::TCVectorContainer<SVertex> normals_;
        GLVM::Core::TCVectorContainer<SFace> faces_;

        std::string sWavefrontObjFileData;
        const char* pWavefrontObjFileData;
        unsigned int uiCounter = 0;
    public:
        CWaveFrontObjParser();

//        static CWaveFrontObjParser* GetInstance(); ///< It possibly to get only one instance of this class whith this method.
        
        GLVM::Core::TCVectorContainer<SVertex> getCoordinateVertices();
        GLVM::Core::TCVectorContainer<SVertex> getTextureVertices();
		GLVM::Core::TCVectorContainer<SVertex> getNormals();
        GLVM::Core::TCVectorContainer<SFace> getFaces();
        
        void ReadFile(const char* _filePath);
        void ParseFile();
        GLVM::Core::TCVectorContainer<TCVectorContainer<char>> Split(const char* _pWaveFrontObjFileData, const char _separator, const char _exitSymbol, unsigned int& _uiCounter);
        SVertex ParseVertices(GLVM::Core::TCVectorContainer<TCVectorContainer<char>> _wordsContainer);
        SFace ParseFaces(GLVM::Core::TCVectorContainer<TCVectorContainer<char>> _wordsContainer);
        int ParseInteger(GLVM::Core::TCVectorContainer<char> _word);
        float ParseFloating(GLVM::Core::TCVectorContainer<char> _word);
    };
}

#endif
