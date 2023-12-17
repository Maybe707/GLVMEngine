#ifndef PARSING_SYSTEM
#define PARSING_SYSTEM

#include "JsonParser.hpp"
#include "Vector.hpp"

namespace GLVM::core
{
	class ParsingSystem {
		core::vector<const char*> pathsGLTF_;
		core::vector<float> frames;
		std::vector<std::vector<float>> aVertexes_;
		std::vector<std::vector<unsigned int>> aIndices_;
	public:
		void LoadGLTF();
	};
} // namespace GLVM::core

#endif
