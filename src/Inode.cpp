#include "Inode.hpp"

Inode::Inode(std::string name, Type type) :
    name(name), type(type), size(0) {}

std::string Inode::Read(const std::vector<std::string>& blockStorage) const {
    std::string result;
    for (int index : dataBlocks) {
        if (index >= 0 && index < static_cast<int>(blockStorage.size())) result += blockStorage[index];
    }
    return result;
}

void Inode::Write(const std::string& data, std::vector<std::string>& blockStorage, std::vector<int>& freeBlocks, bool overwrite) {
    if (overwrite) {
        for (int i : dataBlocks) {
            if (i >= 0 && i < static_cast<int>(blockStorage.size())) {
                blockStorage[i] = "null";
                freeBlocks.push_back(i); 
            }
        }
        dataBlocks.clear();
        size = 0;
    }

    int index;
    if (!freeBlocks.empty()) {
        index = freeBlocks.back();
        freeBlocks.pop_back();
        blockStorage[index] = data;
    } 
    else {
        index = blockStorage.size();
        blockStorage.push_back(data);
    }
    
    dataBlocks.push_back(index);
    size += data.size();
}

bool Inode::IsDirectory() const { return type == Type::DIRECTORY; }