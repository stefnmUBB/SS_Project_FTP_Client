#include "VirtualFS.h"

#include <fstream>

namespace fs = std::filesystem;

VirtualFS::VirtualFS(std::filesystem::path root) : root{ root }
{
	if (!fs::exists(root))
		fs::create_directory(root);
}

std::vector<char> VirtualFS::read(std::filesystem::path relative_path)
{
	std::ifstream f(root / relative_path);
	f.seekg(0, std::ios::end);
	size_t length = f.tellg();
	f.seekg(0, std::ios::beg);
	std::vector<char> buffer(length);
	f.read(buffer.data(), length);
	f.close();
	return buffer;
}

void VirtualFS::write(std::filesystem::path relative_path, std::vector<char> buffer)
{
	std::ofstream f(root / relative_path);
	f.write(buffer.data(), buffer.size());
	f.close();	
}