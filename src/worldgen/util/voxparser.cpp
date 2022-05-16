#include "voxparser.h"

#include <fstream>
#include <fmt/format.h>
#include <sstream>

template<typename T>
T readPrimitive(std::basic_istream<char> &stream) {
	T result = {};
	stream.read(reinterpret_cast<char *>(&result), sizeof(result));

	if(!stream.good())
		throw std::runtime_error(fmt::format("Invalid data (RP) {}", stream.gcount()).c_str());

	return result;
}

void VOXParser::parseData(std::unique_ptr<std::istream> stream) {
	clear();

	std::string prefix;
	prefix.resize(4);
	stream->read(prefix.data(), prefix.size());
	if(prefix != "VOX ")
		throw std::runtime_error("Provided file is not of the VOX file format");

	const auto fileVersion = readPrimitive<uint32_t>(*stream);
	if(fileVersion != 150 && fileVersion != 200)
		throw std::runtime_error(fmt::format("Unsupported vox file format version ({})", fileVersion).c_str());

	// Process main chunk
	{
		const Chunk mainChunk = readChunk(*stream);
		if(mainChunk.name != "MAIN")
			throw std::runtime_error("MAIN chunk expected");

		if(stream->peek() != EOF)
			throw std::runtime_error("There should be nothing left after the main chunk");

		if(!mainChunk.data.empty())
			throw std::runtime_error("Main chunk should not have any data");

		std::istringstream mb(mainChunk.childrenChunks);

		while(mb.peek() != EOF) {
			const Chunk ch = readChunk(mb);

			if(!ch.childrenChunks.empty())
				throw std::runtime_error("Invalid data (3)");

			std::istringstream cb(ch.data);

			if(ch.name == "XYZI") {
				struct VoxelRec {
					uint8_t x, y, z, id;
				};

				if(!voxels_.empty())
					throw std::runtime_error("Multiple models are not supported");

				auto numVoxels = readPrimitive<uint32_t>(cb);
				while(numVoxels--) {
					const VoxelRec voxel = readPrimitive<VoxelRec>(cb);
					voxels_[voxel.id].push_back(VoxelPos{voxel.x, voxel.y, voxel.z});
				}
			}

				// We don't care about other chunk types
			else
				continue;

			if(cb.peek() != EOF)
				throw std::runtime_error("Invalid data (4)");
		}
	}
}

void VOXParser::clear() {
	voxels_.clear();
}

VOXParser::Chunk VOXParser::readChunk(std::basic_istream<char> &b) {
	Chunk chunk;

	chunk.name.resize(4);
	b.read(chunk.name.data(), chunk.name.size());

	const auto dataSize = readPrimitive<uint32_t>(b);
	const auto childrenSize = readPrimitive<uint32_t>(b);

	auto pos = b.tellg();
	b.seekg(0, std::ios_base::end);
	auto len = b.tellg();
	b.seekg(pos);

	chunk.data.resize(dataSize);
	b.read(chunk.data.data(), dataSize);

	chunk.childrenChunks.resize(childrenSize);
	b.read(chunk.childrenChunks.data(), childrenSize);

	if(!b.good())
		throw std::runtime_error("Invalid data (unexpected EOF)");

	return chunk;
}
