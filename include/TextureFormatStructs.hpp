#ifndef TEXTURE_FORMAT_STRUCTS_HPP
#define TEXTURE_FORMAT_STRUCTS_HPP

#include <cstdint>
#include <vector>

namespace GLVM::core
{
	/*
	====================================
	Meta data for DDS compressed texture
	====================================
	*/
	
	struct DDS_PIXELFORMAT
	{
		uint32_t size;
		uint32_t flags;
		uint32_t fourCC;
		uint32_t rgbBitCount;
		uint32_t rMask;
		uint32_t gMask;
		uint32_t bMask;
		uint32_t aMask;
	};

	struct DDS_HEADER
	{
		uint32_t size;
		uint32_t flags;

		uint32_t height;
		uint32_t width;

		uint32_t pitchOrLinearSize;
		uint32_t depth;
		uint32_t mipMapCount;

		uint32_t reserved1[11];

		DDS_PIXELFORMAT pixelFormat;

		uint32_t caps;
		uint32_t caps2;
		uint32_t caps3;
		uint32_t caps4;

		uint32_t reserved2;
	};

	struct DDS_HEADER_DXT10
	{
		uint32_t dxgiFormat;
		uint32_t resourceDimension;
		uint32_t miscFlag;
		uint32_t arraySize;
		uint32_t miscFlags2;
	};

	struct DDSData
	{
		uint32_t width = 0;
		uint32_t height = 0;
		uint32_t mipLevels = 1;

		std::vector<std::byte> data;
	};
};

#endif
