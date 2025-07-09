#include "Inode.hpp"

Inode::Inode(std::string name, Type type) :
    name(name), type(type) {}

std::string Inode::Read(const std::vector<std::string>& blockStorage) const {
    std::string result;
    for (int index : dataBlocks) {
        if (index >= 0 && index < static_cast<int>(blockStorage.size())) result += blockStorage[index];
    }
    return result;
}

void Inode::Write(const std::string& data, std::vector<std::string>& blockStorage, bool overwrite) {
    if (overwrite) {
        dataBlocks.clear();
        size = 0;
    }
    
    blockStorage.push_back(data);
    dataBlocks.push_back(blockStorage.size() - 1);
    size += data.size();
}

bool Inode::IsDirectory() const { return type == Type::DIRECTORY; }