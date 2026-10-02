#include "TextureLoadingUtils.hpp"
#include "TextureFormatStructs.hpp"

namespace GLVM::core {

	constexpr uint32_t makeFourCC( char a, char b, char c, char d) {
		return
			static_cast<uint32_t>(a) |
			(static_cast<uint32_t>(b) << 8) |
			(static_cast<uint32_t>(c) << 16) |
			(static_cast<uint32_t>(d) << 24);
	}
	
	DDSData loadDDS(const char* filename) {
		std::ifstream file(
			filename,
			std::ios::binary | std::ios::ate
			);

		if (!file)
			throw std::runtime_error("Failed to open DDS");

		const std::streamsize fileSize = file.tellg();

		file.seekg(0);

		char magic[4];

		file.read(magic, 4);

		if (std::memcmp(magic, "DDS ", 4) != 0)
			throw std::runtime_error("Not a DDS file");

		DDS_HEADER header{};

		file.read(reinterpret_cast<char*>(&header), sizeof(header));

		if (header.size != 124)
			throw std::runtime_error("Invalid DDS header");

		DDSData result;

		result.width = header.width;
		result.height = header.height;

		result.mipLevels = header.mipMapCount ? header.mipMapCount : 1;

		constexpr uint32_t FOURCC_DX10 = makeFourCC('D', 'X', '1', '0');

		bool hasDX10 = header.pixelFormat.fourCC == FOURCC_DX10;
		DDS_HEADER_DXT10 dx10{};
		if( hasDX10 ) {
			file.read(reinterpret_cast<char*>(&dx10), sizeof(dx10));
		}

		constexpr uint32_t DXGI_FORMAT_BC7_UNORM = 98;
		constexpr uint32_t DXGI_FORMAT_BC7_UNORM_SRGB = 99;

		if (dx10.dxgiFormat != DXGI_FORMAT_BC7_UNORM &&
			dx10.dxgiFormat != DXGI_FORMAT_BC7_UNORM_SRGB) {
			throw std::runtime_error(
				"DDS is not BC7"
				);
		}

		const std::streamsize dataSize = fileSize - file.tellg();

		result.data.resize(dataSize);
		file.read(reinterpret_cast<char*>(result.data.data()), dataSize);

		return result;
	}
	
}; ///< namespace GLVM::core
