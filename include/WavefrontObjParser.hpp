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

namespace GLVM::core
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
        GLVM::core::vector<int> vertexIndex;
        GLVM::core::vector<int> textureIndex;
        GLVM::core::vector<int> normalIndex;

        GLVM::core::vector<int>& operator[](const unsigned int _iIndex) {
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
        
        GLVM::core::vector<SVertex> coordinateVertices_;
        GLVM::core::vector<SVertex> textureVertices_;
		GLVM::core::vector<SVertex> normals_;
        GLVM::core::vector<SFace> faces_;

        std::string sWavefrontObjFileData;
        const char* pWavefrontObjFileData;
        unsigned int uiCounter = 0;
    public:
        CWaveFrontObjParser();

//        static CWaveFrontObjParser* GetInstance(); ///< It possibly to get only one instance of this class whith this method.
        
        GLVM::core::vector<SVertex>& getCoordinateVertices();
        GLVM::core::vector<SVertex>& getTextureVertices();
		GLVM::core::vector<SVertex>& getNormals();
        GLVM::core::vector<SFace>& getFaces();
        
        void ReadFile(const char* _filePath);
        void ParseFile();
        GLVM::core::vector<vector<char>> Split(const char* _pWaveFrontObjFileData, const char _separator, const char _exitSymbol, unsigned int& _uiCounter);
        SVertex ParseVertices(GLVM::core::vector<vector<char>> _wordsContainer);
        SFace ParseFaces(GLVM::core::vector<vector<char>> _wordsContainer);
        int ParseInteger(GLVM::core::vector<char> _word);
        float ParseFloating(GLVM::core::vector<char> _word);
    };
}

#endif
