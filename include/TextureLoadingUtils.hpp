#ifndef TEXTURE_LOADING_UTILS_HPP
#define TEXTURE_LOADING_UTILS_HPP

#include <fstream>
#include <vector>
#include <stdexcept>
#include <cstring>
#include "typenames.hpp"
#include "TextureFormatStructs.hpp"

namespace GLVM::core {
	constexpr uint32_t makeFourCC( char a, char b, char c, char d);
	DDSData loadDDS(const char* filename);
}; ///< namespace GLVM::core

#endif
